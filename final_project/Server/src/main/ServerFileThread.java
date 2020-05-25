package main;

import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class ServerFileThread extends Thread{
	private long fileSize;
	private FileOutputStream fos = null;
	private DataInputStream dis;
	private int pcNum;
	private InputStream isf;
	
	public ServerFileThread(long fileSize, int pcNum, InputStream isf) {
		this.fileSize = fileSize;
		this.pcNum = pcNum;
		this.isf = isf;
		this.dis = new DataInputStream(isf);
	}
	
	@Override
	public void run() {		
		int totalRecv = 0;
		int readBytes;
		try {
			fos = new FileOutputStream("screencapture" + pcNum + ".jpg");
			byte[] buffer = new byte[4096];
			while(totalRecv < fileSize) {
				System.out.print("#");
				readBytes = isf.read(buffer);
				fos.write(buffer, 0 ,readBytes);
				totalRecv += readBytes;
			}
			System.out.println("\nrecv end");
		}
		catch (IOException e) {
            e.printStackTrace();
        } finally {
        	if(fos != null) {
        		try {
        			fos.close();
        		}
        		catch(IOException e) {
        			e.printStackTrace();
        		}
        	}
        }
		System.out.println("END THREAD!");
	}

}
