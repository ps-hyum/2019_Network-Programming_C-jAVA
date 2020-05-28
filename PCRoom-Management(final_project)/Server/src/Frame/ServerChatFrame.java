package Frame;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

import main.PC;
import main.ServerThread;


public class ServerChatFrame extends JFrame implements ActionListener {

	String chatReq = "chatReq";
	
	JTextArea txtA = new JTextArea();
	JTextField txtF = new JTextField(20);
	JButton btn_transfer = new JButton("전송");
	String myID;
	
	JPanel p1 = new JPanel();
	
	PC pc;
	int pcNum;
	String id;
	ServerThread st;
	
	public ServerChatFrame(PC pc, int num) {
		super("PC번호" + num + " 채팅");
		this.pc = pc;
		this.pcNum = num;
		this.id = pc.getUser().getId();
		this.st = pc.getThread();

    	JScrollPane scrollPane = new JScrollPane(txtA);
    	scrollPane.getVerticalScrollBar().setValue(scrollPane.getVerticalScrollBar().getMaximum());
		add("Center", scrollPane);
		txtA.setEditable(false);

		p1.add(txtF);
		p1.add(btn_transfer);
		add("South", p1);
		
		btn_transfer.addActionListener(this);
		setBounds(300,300,350,300);
		this.setDefaultCloseOperation(HIDE_ON_CLOSE);
        setVisible(true);
	}
	
	public void actionPerformed(ActionEvent e) {
		System.out.println("my pcNum : " + pcNum);
        if(txtF.getText().equals("")){
               return;
        }
        txtA.append("[Server] : "+ txtF.getText()+"\n");
        st.outStream(chatReq);
        st.outStream(txtF.getText());
        txtF.setText("");
	}
	
	public void getChat(String str) {	
        txtA.append("[PC[" + pcNum + "]] : "+ str +"\n");
	}
}
