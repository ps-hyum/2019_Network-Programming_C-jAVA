package main;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

import Frame.ServerFrame;

public class ServerThread extends Thread {

	ServerFrame sf;
	private Socket clntSock = null;
	private Socket clntfileSock = null;
	private Socket clntprocessSock = null;
	FileSendThread fst;
	InputStream is;
	OutputStream os;
	InputStream isf;
	OutputStream osf;
	InputStream isp;
	PrintWriter pw;
	BufferedReader br;
	BufferedReader brp;
	PCs pm;
	int pcNum;
	UserMap um;
	User user;
	LoginUserMap lum;
	RecvProcessThread rpt;

	String kakao = "KakaoTalk.exe";
	String powerReq = "powerReq";
	String sigininReq = "signinReq";
	String findidReq = "findidReq";
	String findpwdReq = "findpwdReq";
	String filltimeReq = "filltimeReq";
	String loginReq = "loginReq";
	String menuReq = "menuReq";
	String chatReq = "chatReq";
	String logoutReq = "logoutReq";

	String okRep = "okRep";
	String noRep = "noRep";
	String chatRep = "chatRep";
	String captureReq = "captureReq";
	String filedownRep = "filedownRep";
	String processReq = "processReq";
	String timeReq = "timeReq";
	String filedownReq = "filedownReq";
	
	int chatExist = 0;
	
	public ServerThread(Socket socket, PCs pm, UserMap um, LoginUserMap lum, ServerFrame sf, Socket filesocket,  Socket processsocket) {
		this.clntSock = socket;
		this.clntfileSock = filesocket;
		this.clntprocessSock = processsocket;
		this.pm = pm;
		this.um = um;
		this.lum = lum;
		this.sf = sf;
		try {
			is = clntSock.getInputStream();
			os = clntSock.getOutputStream();
			isf = clntfileSock.getInputStream();
			osf = clntfileSock.getOutputStream();
			isp = clntprocessSock.getInputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}
		br = new BufferedReader(new InputStreamReader(is));
		pw = new PrintWriter(new OutputStreamWriter(os));
		brp = new BufferedReader(new InputStreamReader(isp));
	}

	@Override
	public void run() {
		rpt = new RecvProcessThread(this, this.brp);
		rpt.start();
		try {
			while (true) {
				String msg = null;
				String recvStr = null;
				System.out.println("ready to get msg");
				msg = this.inStream();
				System.out.println("msg : " + msg);
				if(msg.length() < 100)
				recvStr = this.inStream();
				System.out.println("recvStr " + recvStr);

				// pc선택, 회원가입, id찾기, pw찾기, 시간충전, 로그인, 채팅, 음식주문
				if (msg.equals(powerReq)) {
					this.outStream(msg);
					// pc번호 받음
					if (this.pcReq(recvStr))
						this.outStream(okRep);
					else
						this.outStream(noRep);
				} else if (msg.equals(sigininReq)) {
					this.outStream(msg);
					// 이름, 아이디, 비밀먼호, 주민번호
					if (this.signinReq(recvStr))
						this.outStream(okRep);
					else
						this.outStream(noRep);
				} else if (msg.equals(findidReq)) {
					this.outStream(msg);
					String result = this.findIDReq(recvStr);
					if (result == null)
						this.outStream(noRep);
					else {
						this.outStream(okRep);
						this.outStream(result);
					}
				} else if (msg.equals(findpwdReq)) {
					this.outStream(msg);
					String result = this.findPWDReq(recvStr);
					if (result == null)
						this.outStream(noRep);
					else {
						this.outStream(okRep);
						this.outStream(result);
					}
				} else if (msg.equals(loginReq)) {
					this.outStream(msg);
					if (this.loginReq(recvStr)) {
						this.outStream(okRep);
						fst = new FileSendThread(this);
					} else
						this.outStream(noRep);
				} else if (msg.equals(menuReq)) {
					outStream(msg);
					sf.menuDial(pcNum, recvStr);
					String[] arr = recvStr.split("``");
					pm.getPC(pcNum).getUser().setMenuMoney(Integer.parseInt(arr[0]));
					pm.getPC(pcNum).getUser().setTotMoney();
					outStream(okRep);
				} else if (msg.equals(chatReq)) {
					if(sf.getChatFrame(pcNum) == null) {
						sf.clientChatReq(pcNum);
					}
					sf.getChatFrame(pcNum).setVisible(true);
					sf.getChatFrame(pcNum).getChat(recvStr);
				}
				// 로그아웃
				else if (msg.equals(logoutReq)) {
					this.outStream(msg);
					this.logoutReq();
					break;
				} else if (msg.equals(timeReq)) {
					this.timeReq(recvStr);
				} else if (msg.equals(processReq)) {
					this.processReq(recvStr);
				} else if (msg.equals(captureReq)) {
					long fileSize = Long.parseLong(recvStr);
					ServerFileThread sft = new ServerFileThread(fileSize, pcNum, isf);
					sft.run();
					sft = null;
				} else {
					System.out.println("something wrong\n");
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				this.netClose();
			} catch (Exception e) {
				e.printStackTrace();
			}
			if(clntSock.isClosed())
				System.out.println("소켓 닫힘");
			System.out.println(is);
		}
	}
	
	public FileSendThread getFST() {
		return this.fst;
	}
	
	public void logoutReq() {
		sf.getSeatFrame(pcNum).defaultID();;
		lum.deleteUsing(pm.getPC(pcNum).getUser().getId());
		pm.getPC(pcNum).logoutUser();
	}

	public void processReq(String str) {
		pm.getPC(pcNum).setProcess(str);
		sf.setProcesses(str);
		if(str.contains(kakao)) {
			outStream(logoutReq);
		}
	}

	public void timeReq(String el) {
		user.setUseTime(el);
	}

	// pc선택, 회원가입, id찾기, pw찾기, 시간충전, 로그인, 채팅, 음식주문
	public boolean pcReq(String str) {
		pcNum = Integer.parseInt(str);
		return pm.availPC(pcNum);
	}

	public boolean signinReq(String str) {
//		0: name / 1: id / 2: password / 3:int Pid; //주민 앞번호
		String[] strArr = str.split("``");
		boolean result = !um.checkAvlID(strArr[1]);
		if (result) {
			um.updateUser(strArr[0], strArr[1], strArr[2], Integer.parseInt(strArr[3]));
		}
		return result;
	}

	public String findIDReq(String str) {
		// 0: name, / 1: Pid
		String[] strArr = str.split("``");
		String name = strArr[0];
		int Pid = Integer.parseInt(strArr[1]);
		String findResult = um.findID(name, Pid);
		return findResult;
	}

	public String findPWDReq(String str) {
		// 0: id, / 1: Pid
		String[] strArr = str.split("``");
		String id = strArr[0];
		int Pid = Integer.parseInt(strArr[1]);
		String findResult = um.findPWD(id, Pid);
		return findResult;
	}

	public boolean loginReq(String str) {
		String[] strArr = str.split("``");
		String id = strArr[0];
		String pwd = strArr[1];
		user = um.getUser(id);
		// 없는 아이디
		if (user == null)
			return false;
		// 있는 아이디
		else {
			// 있는데 사용중인 아이디
			if (lum.checkUsing(id)) {
				return false;
			}
			if (user.getPassword().equals(pwd)) {
				pm.setUser(pcNum, user, this);
				lum.updateUsing(id);
				sf.getSeatFrame(pcNum).setID(id);
				return true;
			} else {
				return false;
			}
		}
	}

	public InputStream getIsf() {
		return this.isf;
	}

	public OutputStream getOsf() {
		return this.osf;
	}


	public String inStream() throws Exception {
		String re = br.readLine();
		re = this.reMsg(re);
		return re;
	}

	public void outStream(String str) {
		pw.println(str);
		pw.flush();
	}

	public void netClose() throws Exception {
		rpt.stop();
		pw.close();
		br.close();
		is.close();
		os.close();
		isf.close();
		osf.close();
		brp.close();
		clntSock.close();
		clntfileSock.close();
		clntprocessSock.close();
	}

	public String reMsg(String msg) {
		msg.replace(System.getProperty("line.separator"), "");
		return msg;
	}
}
