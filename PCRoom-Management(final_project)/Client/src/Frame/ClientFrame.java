package Frame;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.text.SimpleDateFormat;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

import Main.Client;

public class ClientFrame extends JFrame implements ActionListener {

	String chatReq = "chatReq";
	String logoutReq = "logoutReq";
	String captureReq = "captureReq";
	String filedownReq = "filedownReq";
	String processReq = "processReq";
	String timeReq = "timeReq";
	
    GridBagLayout gBag;
	
	Client cc;
	
	JPanel jp = new JPanel();
	JPanel lp = new JPanel();
	JPanel llp = new JPanel();
	JPanel rp = new JPanel();
	JButton btn_menu = new JButton("메뉴");
	JButton btn_logout = new JButton("사용종료");
	JButton btn_transfer = new JButton("전송");
	JLabel jl_id = new JLabel("아이디");
	JLabel jl_Iid = new JLabel();
	JLabel jl_time = new JLabel("사용시간");
	JLabel jl_Itime = new JLabel();
	JLabel jl_money = new JLabel("사용요금");
	JLabel jl_Imoney = new JLabel();

	JTextArea txtA = new JTextArea(5,5);
	JTextField txtF = new JTextField(20);

	SimpleDateFormat format1 = new SimpleDateFormat ( "yyyy-MM-dd HH:mm:ss");
	SimpleDateFormat format2 = new SimpleDateFormat ( "mm:ss");
	
	JPanel p1 = new JPanel();
	
	MenuFrame mf;
	String id;
	
	public ClientFrame(Client cc, String id, int pcNum) {
		super("PC번호[" + pcNum + "] / 클라이언트 : " + id);
		this.cc = cc;
		this.id = id;

        gBag = new GridBagLayout(); //GridBagLayout 생성
        setLayout(gBag);
		
        lp.setLayout(new BorderLayout());
        llp.setLayout(new GridLayout(0,2));
        txtA.setEditable(false);
    	JScrollPane scrollPane = new JScrollPane(txtA);
    	scrollPane.getVerticalScrollBar().setValue(scrollPane.getVerticalScrollBar().getMaximum());
		llp.add(txtF);
		llp.add(btn_transfer);
		
		lp.add(scrollPane, BorderLayout.CENTER);		
		lp.add(llp, BorderLayout.SOUTH);
		
		rp.setLayout(new GridLayout(0,2));
		rp.add(jl_id);
		jl_Iid.setText(id);
		rp.add(jl_Iid);
		rp.add(jl_time);
		rp.add(jl_Itime);
		rp.add(jl_money);
		rp.add(jl_Imoney);
		rp.add(btn_menu);
		rp.add(btn_logout);
		
		btn_menu.addActionListener(this);
		btn_transfer.addActionListener(this);
		btn_logout.addActionListener(this);
		
		this.gbinsert(lp, 0, 0, 200, 400);
		this.gbinsert(rp, 200, 0, 200, 400);
		this.pack();
        setVisible(true);
        this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == btn_menu) {
			if(mf == null)
				mf = new MenuFrame(this.cc);
			else
				mf.setVisible(true);
		} else if (e.getSource() == btn_transfer) {
			if (txtF.getText().equals("")) {
				return;
			}
			txtA.append("[ME] : " + txtF.getText() + "\n");
			cc.outStream(chatReq);
			cc.outStream(txtF.getText());
			txtF.setText("");
		} else if (e.getSource() == btn_logout) {
			this.setVisible(false);
			cc.outStream(logoutReq);
			cc.outStream(logoutReq);
		}
	}
	
	public void getChat(String str) {
		txtA.append("[SERVER] : " + str + "\n");
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
	public MenuFrame getMenuFrame() {
		return this.mf;
	}
	
	public void updateTM(String time) {
		String[] arr = time.split(":");
		int minute = Integer.parseInt(arr[0]);
		int sec = Integer.parseInt(arr[1]);
		jl_Itime.setText(time);
		int a = cc.getUser().getMenuMoney() + minute*60 + sec*10;
		jl_Imoney.setText(Integer.toString(a));
	}
}
