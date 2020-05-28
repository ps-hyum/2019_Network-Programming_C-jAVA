package main;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;

public class FileSendThread extends Thread{
	private String fileName;
	private long fileSize;
	private ServerThread st;
	private OutputStream osf;
	
	public FileSendThread(ServerThread st) {
		this.st = st;
		this.osf = st.getOsf();
	}
	
	public void Init(String fileName, long fileSize) {
		this.fileName = fileName;
		this.fileSize = fileSize;
	}
	
	public void run() {
	File file = new File(fileName);
    if (!file.exists()) {
        System.out.println("File not Exist.");
        System.exit(0);
    }
     
    byte[] buffer = new byte[4096];
    BufferedOutputStream bos = new BufferedOutputStream(osf);
     
    try {
        FileInputStream fis = new FileInputStream(file);
        BufferedInputStream bis = new BufferedInputStream(fis);
        
        int length = fis.read(buffer);
        while(length != -1) {
        	osf.write(buffer, 0, length);
        	length = fis.read(buffer);
        }
        System.out.println("전송성공");
    } catch (IOException e) {
        e.printStackTrace();
    }
	}
}
