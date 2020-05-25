package main;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import Frame.ServerFrame;


public class Server {
	
    private ServerSocket serverSocket = null;
    private ServerSocket serverfileSocket = null;
    private ServerSocket serverprocessSocket = null;
    private Socket clntSock = null;
    private Socket clntFileSock = null;
    private Socket clntProcessSock = null;
    private PCs pm;
    private UserMap um;
    private LoginUserMap lum;
    
    public Server(int port, int fileport, int processport){
		pm = new PCs();
		um = new UserMap();
		lum = new LoginUserMap();
		um.updateUser("000", "000", "000", 000);
		um.updateUser("111", "111", "111", 111);
		um.updateUser("222", "222", "222", 222);
		um.updateUser("333", "333", "333", 333);
		um.updateUser("444", "444", "444", 444);
		um.updateUser("555", "555", "555", 555);
		um.updateUser("666", "666", "666", 666);
		um.updateUser("777", "777", "777", 777);
		um.updateUser("888", "888", "888", 888);
		um.updateUser("999", "999", "999", 999);
		um.updateUser("123", "123", "123", 123);
		um.updateUser("1234", "1234", "1234", 1234);
		ServerFrame sf = new ServerFrame(pm);
		try {
			serverSocket = new ServerSocket(port);
			serverfileSocket = new ServerSocket(fileport);
			serverprocessSocket = new ServerSocket(processport);
			while (true) {
				clntSock = serverSocket.accept();
				clntFileSock = serverfileSocket.accept();
				clntProcessSock = serverprocessSocket.accept();
				new ServerThread(clntSock, pm, um, lum, sf, clntFileSock, clntProcessSock).start();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
    
    public PCs getPCS() {
    	return pm;
    }

	public static void main(String[] args) {
		new Server(8888, 7777, 6666);
	}
}
