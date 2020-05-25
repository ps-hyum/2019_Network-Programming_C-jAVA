package Main;

import java.util.Date;

public class User {
	private String name;
	private String id;
	private String password;
	private int Pid; //ÁÖ¹Î ¾Õ¹øÈ£
	private int Rmoney; //³²Àº µ·
	private Date startTime;
	private String useTime;
	private int pcNum;
	private int useMoney;
	private int menuMoney;
	private int totMoney;
	
	public User(String id) {
		this.id = id;
		this.startUser();
	}
	
	public User(String name, String id, String password, int Pid)
	{
		this.name = name;
		this.id = id;
		this.password = password;
		this.Pid = Pid;
		this.Rmoney = 0;
	}
	
	public void seatUser(int pcNum) {
		this.pcNum = pcNum;
		useMoney = 0;
		menuMoney = 0;
		totMoney = 0;
		
	}
	
	public String setUseTime(String el) {
		this.useTime = el;
		return useTime;
	}
	
	public int getPCnum() {
		return this.pcNum;
	}

	public void startUser() {
		this.startTime = new Date();
	}
	
	public Date getStart() {
		return this.startTime;
	}
	
	public void setPW(String pw) {
		password= pw;
	}
	
	public void setRmoney(int Rmoney) {
		this.Rmoney = Rmoney;
	}
	
	public int getRmoney() {
		return Rmoney;
	}
	
	public String getId() {
		return this.id;
	}
	
	public String getPassword() {
		return this.password;
	}
	
	public int getPid() {
		return this.Pid;
	}
	
	public String getName() {
		return this.name;
	}
	public int getMenuMoney() {
		return menuMoney;
	}

	public void setMenuMoney(int sum) {
		menuMoney += sum;
	}

	public String getTotMoney() {
		return Integer.toString(totMoney);
	}

	public void setTotMoney() {
		this.totMoney = menuMoney + useMoney;
	}
}
