package Main;

import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class FileDownThread extends Thread{
	private long fileSize;
	private FileOutputStream fos = null;
	private int pcNum;
	private InputStream isf;
	private Client cc;
	String fileName;
	
	public FileDownThread(Client cc) {
		this.cc = cc;
		this.isf = cc.getIsf();
	}
	
	public void Init(String fileName, long fileSize) {
		this.fileName = fileName;
		this.fileSize = fileSize;
	}
	
	@Override
	public void run() {		
		int totalRecv = 0;
		int readBytes;
		String[] arr = fileName.split("\\.");
		fileName = arr[0];
		fileName += cc.getpcNum() +"." + arr[1];
		try {
			fos = new FileOutputStream(fileName);
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
