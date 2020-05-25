package Main;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import Frame.*;

public class Client {
	String powerReq = "powerReq";
	String siginReq = "signinReq";
	String findidReq = "findidReq";
	String findpwdReq = "findpwdReq";
	String loginReq = "loginReq";
	String menuReq = "menuReq";
	String chatReq = "chatReq";
	String logoutReq = "logoutReq";

	String okRep = "okRep";
	String noRep = "noRep";
	String captureReq = "captureReq";
	String filedownReq = "filedownReq";
	String processReq = "processReq";
	String timeReq = "timeReq";
	InputStream is;
	OutputStream os;
	InputStream isf;
	OutputStream osf;
	OutputStream osp;
	PrintWriter pw;
	BufferedReader br;
	PrintWriter pwp;
	Socket sock;
	Socket filesock;
	Socket processsock;

	Timer timer;

	private int pcNum;
	private User user;

	PCFrame pf;
	LoginFrame lg;
	ClientFrame cf;
	SigninFrame sf;
	FindIDFrame fidf;
	FindPWDFrame fpwdf;
	ClientUtil cu;
	FileDownThread fdt;

	SimpleDateFormat format1 = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	SimpleDateFormat format2 = new SimpleDateFormat("mm:ss");

	public Client(String ip, int port, int fileport, int processport) {
		try {
			sock = new Socket(ip, port);
			filesock = new Socket(ip, fileport);
			processsock = new Socket(ip, processport);
			os = sock.getOutputStream();
			is = sock.getInputStream();
			osf = filesock.getOutputStream();
			isf = filesock.getInputStream();
			osp = processsock.getOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}

		pw = new PrintWriter(new OutputStreamWriter(os));
		br = new BufferedReader(new InputStreamReader(is));
		pwp = new PrintWriter(new OutputStreamWriter(osp));
	}

	public void ccStart() {
		pf = new PCFrame(this);
		try {
			while (true) {
				String msg = null;
				msg = this.inStream();
				System.out.println("get msg : " + msg);
				// pc선택, 회원가입, id찾기, pw찾기, 시간충전, 로그인, 채팅, 음식주문
				String re = null;
				if (msg.equals(powerReq)) {
					re = inStream();
					if (re.equals(okRep)) {
						pcNum = pf.getNum();
						lg = new LoginFrame(this, pcNum);
						pf.dispose();
					} else if (re.equals(noRep)) {
						// 다ㅇㅣ얼 띄우기
						System.out.println(noRep);
					} else {
						System.out.println("something wrong");
					}
				} else if (msg.equals(loginReq)) {
					re = inStream();
					if (re.equals(okRep)) {
						user = lg.getUser();
						cf = new ClientFrame(this, user.getId() , pcNum);
						this.startProcessThread();
						lg.dispose();
						cu = new ClientUtil(this, pcNum);
						fdt = new FileDownThread(this);
					} else if (re.equals(noRep)) {
						// 다이얼 띄우기
						System.out.println(noRep);
					}
				} else if (msg.equals(siginReq)) {
					sf = lg.getsfFrame();
					// 이름, 아이디, 비밀먼호, 주민번호
					re = inStream();
					if (re.equals(okRep)) {
						sf.okDial();
					} else if (re.equals(noRep)) {
						sf.noDial();
					}
				} else if (msg.equals(findidReq)) {
					fidf = lg.getfidfFrame();
					// 이름, 아이디, 비밀먼호, 주민번호
					re = inStream();
					if (re.equals(okRep)) {
						String reid = inStream();
						fidf.okDial(reid);
					} else if (re.equals(noRep)) {
						fidf.noDial();
					}
				} else if (msg.equals(findpwdReq)) {
					fpwdf = lg.getfpwdfFrame();
					// 이름, 아이디, 비밀먼호, 주민번호
					re = inStream();
					if (re.equals(okRep)) {
						String reid = inStream();
						fpwdf.okDial(reid);
					} else if (re.equals(noRep)) {
						fpwdf.noDial();
					}
				} else if (msg.equals(menuReq)) {
					re = inStream();
					cf.getMenuFrame().setVisible(false);
				} else if (msg.equals(chatReq)) {
					re = inStream();
					cf.getChat(re);
				} else if (msg.equals(logoutReq)) {
					outStream(msg);
					outStream(msg);
					break;
				} else if (msg.equals(processReq)) {
					outStream(msg);
					outStream(cu.getRunningProcesses());
				} else if (msg.equals(captureReq)) {
					outStream(msg);
					cu.capture();
					outStream(cu.getfileSize());
					cu.start();
				} else if (msg.equals(filedownReq)) {
					re = inStream();
					String[] arr = re.split("``");
					String fileName = arr[0];
					long fileSize = Long.parseLong(arr[1]);
					fdt.Init(fileName, fileSize);
					fdt.start();
				}
				else {
					System.out.println("something wrong");
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			cf = null; lg = null; pf = null;
			sf = null; fidf= null;
			fpwdf = null; cu = null;
			try {
				netClose();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	public User getUser() {
		return this.user;
	}
	
	public int getpcNum() {
		return this.pcNum;
	}

	public Socket getFileSock() {
		return this.filesock;
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
	public void outpStream(String str) {
		pwp.println(str);
		pwp.flush();
	}

	public void netClose() throws Exception {
		if (pw != null)	pw.close();
		if (pwp != null) pwp.close();
		if (br != null)	br.close();
		if (is != null)	is.close();
		if (os != null)	os.close();
		if (osp != null) osp.close();
		if (isf != null) isf.close();
		if (osf != null) osf.close();
	}

	public String reMsg(String msg) {
		msg.replace(System.getProperty("line.separator"), "");
		return msg;
	}

	public void startProcessThread() {
		timer = new Timer();
		long period = 1 * 1000;
		timer.schedule(new ProcessTask(this), 100, period);
	}

	class ProcessTask extends TimerTask {
		private Client cc;

		public ProcessTask(Client cc) {
			this.cc = cc;
		}

		public void run() {
			try {
			Date dd = new Date();
			long snd = dd.getTime() - user.getStart().getTime();
			String tt = format2.format(snd);
			user.setUseTime(tt);
			cc.outpStream(tt);
			cf.updateTM(tt);
			outpStream(cu.getRunningProcesses());
			}
			catch(NullPointerException e) {
				System.out.println("클라이언트가 종료되었습니다.");
				System.exit(0);
			}
		}
	}

}