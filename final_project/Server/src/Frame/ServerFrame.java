package Frame;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.text.SimpleDateFormat;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import main.PC;
import main.PCs;
import main.ServerThread;
import main.User;


public class ServerFrame extends JFrame implements ActionListener {

	String chatReq = "chatReq";
	String logoutReq = "logoutReq";
	String captureReq = "captureReq";
	String filedownReq = "filedownReq";
	String processReq = "processReq";
	
    GridBagLayout gBag;
	
	JPanel lm = new JPanel();
	JPanel rm = new JPanel();
	SeatFrame[] slm = new SeatFrame[15];
	SimpleDateFormat format1 = new SimpleDateFormat ( "yyyy-MM-dd HH:mm:ss");
	SimpleDateFormat format2 = new SimpleDateFormat ( "mm:ss");

	//2pc번호, 14아이디, 4사용시작시간, 6사용시간, 8사용요금, 10메뉴요금, 12전체요금
	private JLabel pcinfo01 = new JLabel("PC번호");
	private JLabel pcinfo02 = new JLabel("X");
	private JLabel pcinfo13 = new JLabel("아이디");
	private JLabel pcinfo14 = new JLabel("X");
	private JLabel pcinfo03 = new JLabel("사용시작시간");
	private JLabel pcinfo04 = new JLabel("X");
	private JLabel pcinfo05 = new JLabel("사용시간");
	private JLabel pcinfo06 = new JLabel("X");
	private JLabel pcinfo07 = new JLabel("사용요금");
	private JLabel pcinfo08 = new JLabel("X");
	private JLabel pcinfo09 = new JLabel("메뉴요금");
	private JLabel pcinfo10 = new JLabel("X");
	private JLabel pcinfo11 = new JLabel("전체요금");
	private JLabel pcinfo12 = new JLabel("X");
	
	private int nowPC;
	private PCs pm;
	private String processStr;
	
	//1:사용중지, , 3:프로세스확인, 4:채팅, 5:화면캡쳐, 6:파일전송
	private JButton btn01 = new JButton("사용중지");
	private JButton btn03 = new JButton("프로세스");
	private JButton btn04 = new JButton("채팅");
	private JButton btn05 = new JButton("화면캡쳐");
	private JButton btn06 = new JButton("파일전송");
	private JTextField txtF = new JTextField(20);
	
	ServerChatFrame[] scf = new ServerChatFrame[15];

	public ServerFrame(PCs pm) {
        gBag = new GridBagLayout();
        this.pm = pm;
       
        setLayout(gBag);
		lm.setLayout(new GridLayout(0, 5));
		for (int i = 0; i < pm.getCapacity(); i++) {
			slm[i] = new SeatFrame(i, this, pm);
			lm.add(slm[i]);
		}

		setTitle("Server");
		rm.setLayout(new GridLayout(0,2));
        pcinfo01.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo01.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo01);
        pcinfo02.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo02.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo02);
		pcinfo13.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo13.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo13);
        pcinfo14.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo14.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo14);
        pcinfo03.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo03.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo03);
        pcinfo04.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo04.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo04);
        pcinfo05.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo05.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo05);
        pcinfo06.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo06.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo06);
        pcinfo07.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo07.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo07);
        pcinfo08.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo08.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo08);
        pcinfo09.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo09.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo09);
        pcinfo10.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo10.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo10);
        pcinfo11.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo11.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo11);
        pcinfo12.setHorizontalAlignment(SwingConstants.CENTER);
        pcinfo12.setHorizontalTextPosition(SwingConstants.CENTER);
		rm.add(pcinfo12);
		rm.add(btn01);
		rm.add(btn03);
		rm.add(btn04);
		rm.add(btn05);
		rm.add(txtF);
		rm.add(btn06);
		
		this.gbinsert(lm, 0, 0, 2, 1);
		this.gbinsert(rm, 50, 0, 1, 1);

		btn01.addActionListener(this);
		btn03.addActionListener(this);
		btn04.addActionListener(this);
		btn05.addActionListener(this);
		btn06.addActionListener(this);

		this.pack();
		setVisible(true);
		this.setDefaultCloseOperation(EXIT_ON_CLOSE);
	}
	
	public SeatFrame getSeatFrame(int i) {
		return slm[i];
	}

	public static void main(String[] args) {
		//new ServerFrame();
	}

	//1:사용중지, 2:파일전송, 3:프로세스, 4:채팅, 5:화면캡쳐, 6:파일전송
	public void actionPerformed(ActionEvent e) {
		if(pm.getPC(nowPC).getUser() == null) {
			return;
		}
		if(e.getSource() == btn01) {
			pm.getPC(nowPC).getThread().outStream(logoutReq);
			System.out.println("press logoutReq\n");
		}
		else if(e.getSource() == btn03) {
			new ProcessFrame(processStr);
		}
		else if(e.getSource() == btn04) {
			if(scf[nowPC] == null)
				scf[nowPC] = new ServerChatFrame(pm.getPC(nowPC), pm.getPC(nowPC).getNum());
			else
				scf[nowPC].setVisible(true);
		}
		else if(e.getSource() == btn05) {
			PC imsi = pm.getPC(nowPC);
			System.out.println("PC번호는 " + imsi.getNum());
			ServerThread ii = imsi.getThread();
			if(ii == null)
				System.out.println("쓰레드가 null임");
			ii.outStream(captureReq);
			//pm.getPC(nowPC).getThread().outStream(captureReq);			
		}
		else if(e.getSource() == btn06) {
			String fileName = txtF.getText();
			if(fileName == "")
				return;
			File file = new File(fileName);
			System.out.println("파일 염");
	        if (!file.exists()) {
	            System.out.println("File not Exist.");
	            txtF.setText("");
	            JOptionPane.showMessageDialog(this,"존재하지 않는 파일명입니다. ","파일",JOptionPane.INFORMATION_MESSAGE);
	            return;
	        }	         
	        long fileSize = file.length();
	        String snd = fileName + "``" + Long.toString(fileSize);
			for(int i=0; i<15; i++) {
				System.out.println("for문 안 "+i);
				if(pm.getPC(i).getUser() != null) {
					pm.getPC(i).getThread().outStream(filedownReq);
					pm.getPC(i).getThread().outStream(snd);
					pm.getPC(i).getThread().getFST().Init(fileName, fileSize);
					pm.getPC(i).getThread().getFST().start();
				}
			}
			txtF.setText("");
		}
		
	}
	
	public void clientChatReq(int num) {
		scf[num] = new ServerChatFrame(pm.getPC(num), pm.getPC(num).getNum());
	}
	
	public ServerChatFrame getChatFrame(int num) {
		return this.scf[num];
	}
	
	public void setProcesses(String str) {
		this.processStr = str;
	}
	
	public void selectPC(int num) {
		nowPC = num;
	}
	
	public void changeInfo(PC pc) {
		System.out.println("매개변수 pc의 번호는 : " + pc.getNum());
		System.out.println("현재 쓰레드가 갖고있는 번호는 : " + nowPC);
		String npc = Integer.toString(nowPC);
		pcinfo02.setText(npc);
		User now = pc.getUser();
		if(now == null) {
			pcinfo14.setText("X");
			pcinfo04.setText("X");
			pcinfo06.setText("X");
			pcinfo08.setText("X");
			pcinfo10.setText("X");
			pcinfo12.setText("X");
		}
		else {
			//2pc번호, 14아이디, 4사용시작시간, 6사용시간, 8사용요금, 10메뉴요금, 12전체요금
			pcinfo14.setText(now.getId());
			String time1 = format1.format(now.getStart());
			pcinfo04.setText(time1);
			pcinfo06.setText(now.getUseTime());
			pcinfo08.setText(now.getUseMoney());
			pcinfo10.setText(now.getMenuMoney());
			pcinfo12.setText(now.getTotMoney());			
		}
	}
	
	public void gbinsert(Component c, int x, int y, int w, int h){
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill= GridBagConstraints.BOTH;
        gbc.gridx = x;
        gbc.gridy = y;
        gbc.gridwidth = w;
        gbc.gridheight = h;
        gBag.setConstraints(c,gbc);
        this.add(c);      
    }
	
	
	public void menuDial(int num, String str) {
		String arr[] = str.split("``");
		String mm = " : ";
		for(int i=1; i<arr.length; i++) {
			mm += arr[i] + ", ";
		}
		JOptionPane.showMessageDialog(this,"PC[" + num + "]의 주문\n" + mm,"주문",JOptionPane.INFORMATION_MESSAGE);
	}
}
