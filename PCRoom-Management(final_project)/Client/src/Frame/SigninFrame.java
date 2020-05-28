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

import Main.Client;


public class SigninFrame extends JFrame implements ActionListener{
	
	Client cc;
	
	JPanel jp = new JPanel();
	JLabel jl = new JLabel();
	JLabel jl1 = new JLabel();
	JLabel jl2 = new JLabel();
	JLabel jl3 = new JLabel();
	JLabel jl4 = new JLabel();
	JTextField tf_name, tf_id, tf_pwd, tf_Pid;
	JButton btn = new JButton();
	private String sigininReq = "signinReq";
	String okRep = "okRep";
	String noRep = "noRep";
	
	String name, id, pwd, pid, req;
	
	public SigninFrame(Client cc) {
		this.cc = cc;
		tf_name = new JTextField(15);
		tf_id = new JTextField(15);
		tf_pwd = new JTextField(15);
		tf_Pid = new JTextField(15);
		
        this.setSize(new Dimension(220, 220));
		jp.setLayout(null);
		
		jl.setText("ȸ������");
		jl.setBounds(new Rectangle(110, 5, 250, 25));

		jl4.setText("�̸�");
		jl4.setBounds(new Rectangle(25, 35, 70, 20));

		tf_name.setBounds(new Rectangle(150, 35, 130, 20));
		
		jl1.setText("���̵�");
		jl1.setBounds(new Rectangle(25, 60, 70, 20));

		tf_id.setBounds(new Rectangle(150, 60, 130, 20));

		jl2.setText("��й�ȣ");
		jl2.setBounds(new Rectangle(25, 85, 65, 20));

		tf_pwd.setBounds(new Rectangle(150, 85, 130, 20));
		
		jl3.setText("�ֹι�ȣ(���ڸ�)");
		jl3.setBounds(new Rectangle(25, 110, 100, 20));
		
		tf_Pid.setBounds(new Rectangle(150, 110, 130, 20));
		
		btn.setText("ȸ������");
		btn.setBounds(new Rectangle(160, 135, 100, 20));
		
		jp.add(jl, null);
		jp.add(jl1, null);
		jp.add(jl2, null);
		jp.add(jl3, null);
		jp.add(jl4, null);
		jp.add(tf_name, null);
		jp.add(tf_id, null);
		jp.add(tf_pwd, null);
		jp.add(tf_Pid, null);
		jp.add(btn, null);
		btn.addActionListener(this);
        this.getContentPane().add(jp, null);
	    setVisible(true);
        this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);	
	}
	
	public void actionPerformed(ActionEvent e) {
		cc.outStream(sigininReq);
		name = tf_name.getText();
		id = tf_id.getText();
		pwd = tf_pwd.getText();
		pid = tf_Pid.getText();
		req = name + "``" + id + "``" + pwd + "``" + pid;
		cc.outStream(req);
	}
	
	public void okDial() {
		JOptionPane.showMessageDialog(this,"ȸ������ �Ǿ����ϴ�.\nȯ���մϴ�." + id + "��","ȸ�����ԿϷ�",JOptionPane.INFORMATION_MESSAGE);
		this.dispose();
	}
	
	public void noDial() {
		tf_id.setText("");
		JOptionPane.showMessageDialog(this,"�ߺ��Ǵ� ���̵� �Դϴ�." + id + "��","Error",JOptionPane.INFORMATION_MESSAGE);
		System.out.println(noRep);
	}
}
