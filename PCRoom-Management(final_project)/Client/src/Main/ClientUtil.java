package Main;

import java.awt.AWTException;
import java.awt.HeadlessException;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import javax.imageio.ImageIO;

public class ClientUtil extends Thread {
	private int num;
	private Client cc;
	private String path;
	private InputStream isf;
	private OutputStream osf;
	private long fileSize;
	
	public ClientUtil(Client cc, int num) {
		this.num = num;
		this.cc = cc;
		this.isf = cc.getIsf();
		this.osf = cc.getOsf();
		this.path = "screencapture"+num+".jpg";
	}
	
	public void capture() {
		File file = new File(path);
		try {
			// 전체 화면 Capture
			BufferedImage screencapture = new Robot()
					.createScreenCapture(new Rectangle(Toolkit
							.getDefaultToolkit().getScreenSize()));

			// JPEG 저장.
			ImageIO.write(screencapture, "jpg", file);
		} catch (HeadlessException e) {
			e.printStackTrace();
		} catch (AWTException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		this.fileSize = file.length();
	}
	
	public String getfileSize() {
		String str = Long.toString(fileSize);
		return str;
	}
	
	public void run() {
		File file = new File(path);
        if (!file.exists()) {
            System.out.println("File not Exist.");
            System.exit(0);
        }
         
        long fileSize = file.length();
        long totalReadBytes = 0;
        byte[] buffer = new byte[4096];
        int readBytes;
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
	
	public String getRunningProcesses(){
        String str = "start``";
        try{
            String line;
            Process p = Runtime.getRuntime().exec("tasklist.exe /FI \"STATUS eq RUNNING\"");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            line = input.readLine();
            line = input.readLine();
            line = input.readLine();
            while((line = input.readLine()) != null){
                if (!line.trim().equals("")){
                    String sub = line.substring(0, line.indexOf(" "));
                    str += sub + "``";
                }
            }
        }catch (Exception e){
            e.printStackTrace();
        }
        System.out.println(str);
        return str;
	}	
}
