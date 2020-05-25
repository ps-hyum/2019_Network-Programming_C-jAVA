package main;

public class PCs {
	private PC[] pcs;
	private int capacity = 15;
	
	public PCs() {
		pcs = new PC[capacity];
		initPCs();
	}
	
	public void initPCs() {
		for(int i=0; i<capacity; i++) {
			PC pc = new PC(i);
			pcs[i] = pc;
		}
	}
	
	public boolean availPC(int pcNum) {
		if(pcs[pcNum].getPower() == 0) {
			pcs[pcNum].powerON();
			return true;
		}
		else return false;
	}
	
	public void setUser(int pcNum, User user, ServerThread st) {
		System.out.println("pm안 setUser / " + pcNum);
		if(st == null)
			System.out.println("PCs안 쓰레드가 널임");
		pcs[pcNum].setUser(user);
		pcs[pcNum].setThread(st);
	}
	
	public PC getPC(int pcNum) {
		return pcs[pcNum];
	}
	
	public int getCapacity() {
		return this.capacity;
	}
}
