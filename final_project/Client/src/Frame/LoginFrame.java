package Frame;

import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import Main.Client;
import Main.User;

public class LoginFrame extends JFrame implements ActionListener {

String findidReq = "findidReq";
String findpwdReq = "findpwdReq";
String signinReq = "signinReq";
	String loginReq = "loginReq";
	String okRep = "okRep";
	String noRep = "noRep";

	JTextField tf_id, tf_pwd;
	JButton btn_login, btn_signin, btn_fID, btn_fPWD, btn_fill;
	String id;
	
	Client cc;
	JPanel jp1 = new JPanel();
	JPanel jp2 = new JPanel();
	JLabel jl1 = new JLabel();
	JLabel jl2 = new JLabel();
	JLabel jl3 = new JLabel();
	JLabel jl4 = new JLabel();
	
	SigninFrame sf;
	FindIDFrame fidf;
	FindPWDFrame fpwdf;
	
	private int pcNum;
	
	public LoginFrame(Client cc, int pcNum) {
		this.pcNum = pcNum;
		this.cc = cc;
		this.setSize(new Dimension(220, 220));
        jp1.setLayout(null);
		tf_id = new JTextField(15);
		tf_pwd = new JTextField(15);
		btn_signin = new JButton();
		btn_login = new JButton();
		btn_fID = new JButton();
		btn_fPWD = new JButton();
		btn_fill = new JButton();
		jl1.setText("로그인 페이지");
        jl1.setBounds(new Rectangle(10, 5, 250, 25));
	    jl1.setHorizontalAlignment(SwingConstants.CENTER);
	    
	    jl4.setText("이름");
        jl4.setBounds(new Rectangle(20, 30, 70, 20));
        jl4.setHorizontalAlignment(SwingConstants.TRAILING);

        tf_id.setBounds(new Rectangle(100, 30, 130, 20));
	    
	    jl2.setText("아이디");
        jl2.setBounds(new Rectangle(20, 60, 70, 20));
        jl2.setHorizontalAlignment(SwingConstants.TRAILING);
        
        tf_id.setBounds(new Rectangle(100, 60, 130, 20));
        
        jl3.setText("비밀번호");
        jl3.setBounds(new Rectangle(25, 100, 65, 20));
        jl3.setHorizontalAlignment(SwingConstants.TRAILING);

        tf_pwd.setBounds(new Rectangle(100, 100, 130, 20));
                
        btn_signin.setText("회원가입");
        btn_signin.setBounds(new Rectangle(10, 130, 120, 20));

        btn_login.setText("로그인");
        btn_login.setBounds(new Rectangle(130, 130, 120, 20));
        
        btn_fID.setText("아이디 찾기");
        btn_fID.setBounds(new Rectangle(10, 160, 120, 20));
        
        btn_fPWD.setText("비밀번호 찾기");
        btn_fPWD.setBounds(new Rectangle(130, 160, 120, 20));
        
	    jp1.add(jl1, null);
	    jp1.add(jl2, null);
	    jp1.add(tf_id, null);
	    jp1.add(jl3, null);
	    jp1.add(tf_pwd, null);
	    jp1.add(btn_login, null);
	    jp1.add(btn_signin, null);
	    jp1.add(btn_fID, null);
	    jp1.add(btn_fPWD, null);
		btn_login.addActionListener(this);
		btn_signin.addActionListener(this);
		btn_fID.addActionListener(this);
		btn_fPWD.addActionListener(this);
        this.getContentPane().add(jp1, null);
        this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        this.setVisible(true);
	}
	
	public String getID() {
		return this.id;
	}
	
	public User getUser() {
		User user = new User(id);
		return user;
	}
	
	public SigninFrame getsfFrame() {
		return this.sf;
	}
	
	public FindIDFrame getfidfFrame() {
		return this.fidf;
	}
	public FindPWDFrame getfpwdfFrame() {
		return this.fpwdf;
	}
	
	public void actionPerformed(ActionEvent e) {
		if(e.getSource() == btn_signin) {
			sf = new SigninFrame(cc);
		}
		else if(e.getSource() == btn_login) {
			cc.outStream(loginReq);
			id = tf_id.getText();
			String pwd = tf_pwd.getText();
			String Req = id + "``" + pwd;
			cc.outStream(Req);
		}
		else if(e.getSource() == btn_fID) {
			fidf = new FindIDFrame(cc);
		}
		else if(e.getSource() == btn_fPWD) {
			fpwdf = new FindPWDFrame(cc);
		}
	}

}
