package Main;

public class Main {
	
    public static String serverIP = "127.0.0.1";
    public static int serverPort = 8888;
    public static int serverfilePort = 7777;
    public static int serverproessPort = 6666;

	public static void main(String[] args) {
		Client cc = new Client(serverIP, serverPort, serverfilePort, serverproessPort);
		cc.ccStart();
		System.out.println("end the main client");
	}
}
