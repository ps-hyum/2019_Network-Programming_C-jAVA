package main;

import java.util.Date;

public class User {
	private String name;
	private String id;
	private String password;
	private int Pid; //ÁÖ¹Î ¾Õ¹øÈ£
	private Date startTime;
	private String useTime;
	private int useMoney;
	private int menuMoney;
	private int totMoney;
	
	public User(String name, String id, String password, int Pid)
	{
		this.name = name;
		this.id = id;
		this.password = password;
		this.Pid = Pid;
		this.useMoney = 0;
		this.menuMoney = 0;
		this.totMoney = 0;
	}

	public void setUseTime(String el) {
		this.useTime = el;
		String[] arr = useTime.split(":");
		int minute = Integer.parseInt(arr[0]);
		int sec = Integer.parseInt(arr[1]);
		useMoney = (minute*60 + sec*10);
		totMoney = useMoney + menuMoney;
	}
	
	public String getUseTime() {
		return useTime;
	}

	public void startUser() {
		this.startTime = new Date();
	}
	
	public void endUser() {
		startTime = null;
		useTime = null;
		this.useMoney = 0;
		this.menuMoney = 0;
		this.totMoney = 0;
	}
	
	public Date getStart() {
		return this.startTime;
	}
	
	public void setPW(String pw) {
		password= pw;
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

	public String getUseMoney() {
		return Integer.toString(useMoney);
	}

	public void setUseMoney(int useMoney) {
		this.useMoney = useMoney;
	}

	public String getMenuMoney() {
		return Integer.toString(menuMoney);
	}

	public void setMenuMoney(int menuMoney) {
		this.menuMoney += menuMoney;
	}

	public String getTotMoney() {
		return Integer.toString(totMoney);
	}

	public void setTotMoney() {
		this.totMoney = menuMoney + useMoney;
	}
}
