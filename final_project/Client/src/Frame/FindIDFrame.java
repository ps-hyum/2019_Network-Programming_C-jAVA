package Frame;

import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import Main.Client;

public class FindIDFrame extends JFrame implements ActionListener{

	String findidReq = "findidReq";
	String okRep = "okRep";
	String noRep = "noRep";

	JTextField tf_name, tf_pid;
	JButton btn;
	
	Client cc;
	JPanel jp1 = new JPanel();
	JPanel jp2 = new JPanel();
	JLabel jl1 = new JLabel();
	JLabel jl2 = new JLabel();
	JLabel jl3 = new JLabel();
	
	public FindIDFrame(Client cc) {
		this.cc = cc;
		this.setSize(new Dimension(220, 220));
        jp1.setLayout(null);
		tf_name = new JTextField(15);
		tf_pid = new JTextField(15);
		btn = new JButton();
		jl1.setText("아이디 찾기");
        jl1.setBounds(new Rectangle(10, 5, 250, 25));
	    jl1.setHorizontalAlignment(SwingConstants.CENTER);
	    
	    jl2.setText("이름");
        jl2.setBounds(new Rectangle(20, 35, 70, 20));
        jl2.setHorizontalAlignment(SwingConstants.TRAILING);
        
        tf_name.setBounds(new Rectangle(100, 35, 130, 20));
        
        tf_pid.setBounds(new Rectangle(100, 60, 130, 20));
        
        jl3.setText("주민번호(앞자리)");
        jl3.setBounds(new Rectangle(25, 60, 65, 20));
        jl3.setHorizontalAlignment(SwingConstants.TRAILING);
                
        btn.setText("찾기");
        btn.setBounds(new Rectangle(10, 100, 120, 20));
        
	    jp1.add(jl1, null);
	    jp1.add(jl2, null);
	    jp1.add(tf_name, null);
	    jp1.add(jl3, null);
	    jp1.add(tf_pid, null);
	    jp1.add(btn, null);
		btn.addActionListener(this);
        this.getContentPane().add(jp1, null);
	    setVisible(true);
        this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
	}
	
	public void actionPerformed(ActionEvent e) {
		cc.outStream(findidReq);
		String name, pid, req;
		name = tf_name.getText();
		pid = tf_pid.getText();
		req = name + "``" + pid;
		cc.outStream(req);
	}
	
	public void okDial(String reid) {
		JOptionPane.showMessageDialog(this,"찾는 아이디 : " + reid,"아이디",JOptionPane.INFORMATION_MESSAGE);
		this.dispose();
	}
	
	public void noDial() {
		tf_name.setText("");
		tf_pid.setText("");
		JOptionPane.showMessageDialog(this,"이름 또는 주민번호가 잘못되었습니다.","Error",JOptionPane.INFORMATION_MESSAGE);
		
	}
}