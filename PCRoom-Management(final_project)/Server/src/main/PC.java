package main;

public class PC {
	private int pcNum; //1~20������
	private int power; //0�̸� off, 1�̸� on
	private User user;
	private ServerThread st;
	private String processes;
	
	public PC(int pcNum) {
		this.pcNum = pcNum;
		this.power = 0;
		this.user = null;
		//this.startTime = 0;
	}
	
	public void setProcess(String str) {
		this.processes = str;
	}
	
	public String getProcess() {
		return this.processes;
	}
	
	public void setThread(ServerThread st) {
		this.st = st;
		if(st == null)
			System.out.println("PC�ȿ��� st�� nuu��");
		if(this.st == null)
			System.out.println("PC�ȿ��� this.st�� null��");
	}
	
	public int getNum() {
		return this.pcNum;
	}
	
	public ServerThread getThread() {
		return this.st;
	}
	
	public void powerON() {
		this.power = 1;
	}
	
	public void powerOFF() {
		this.power = 0;
	}
	
	public void setUser(User user) {
		this.user = user;
		this.user.startUser();
	}
	
	public void logoutUser() {
		this.powerOFF();
		this.user.endUser();
		this.user = null;
	}
	
	public int getPower() {
		return this.power;
	}
	public User getUser() {
		return this.user;
	}
}
