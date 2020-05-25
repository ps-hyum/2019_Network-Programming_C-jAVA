package main;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.Date;

public class RecvProcessThread extends Thread {
	
	private ServerThread st;
	private BufferedReader brp;
	
	public RecvProcessThread(ServerThread st, BufferedReader brp) {
		this.st = st;
		this.brp = brp;
	}
	
	public void run() {
		while(true) {
			try {
			String recvTime;
			recvTime = this.inpStream();
			st.timeReq(recvTime);
			String recvProcess;
			recvProcess = this.inpStream();
			st.processReq(recvProcess);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	public String inpStream() throws Exception {
		String re = brp.readLine();
		re = this.reMsg(re);
		return re;
	}

	public String reMsg(String msg) {
		msg.replace(System.getProperty("line.separator"), "");
		return msg;
	}
}
