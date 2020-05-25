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

import Main.Client;

public class PCFrame extends JFrame implements ActionListener {
	
	String powerReq = "powerReq";
	String okRep = "okRep";
	String noRep = "noRep";
	
	String numStr;
	
	Client cc;
	
	JLabel jl = new JLabel();
	JPanel jp = new JPanel();
	JTextField tf = new JTextField(15);
	JButton btn_power = new JButton("확인");
	
	public PCFrame(Client cc) {
		super("PC");
		this.cc = cc;

		this.setSize(new Dimension(150, 150));
		jp.setLayout(null);

		jl.setText("PC번호");
		jl.setBounds(new Rectangle(45, 5, 65, 25));

		tf.setBounds(new Rectangle(45, 40, 65, 25));

		btn_power.setBounds((new Rectangle(45, 70, 65, 25)));
		btn_power.addActionListener(this);

		jp.add(jl, null);
		jp.add(tf, null);
		jp.add(btn_power, null);
		this.getContentPane().add(jp, null);
		setVisible(true);
		this.setDefaultCloseOperation(EXIT_ON_CLOSE);
	}
	
	public int getNum() {
		return Integer.parseInt(numStr);
	}

	public void actionPerformed(ActionEvent e) {
		numStr = tf.getText();
		int a = Integer.parseInt(numStr);
		if((a < 0) || (a>15)) {
			tf.setText("");
			return;
		}
		cc.outStream(powerReq);
		cc.outStream(numStr);
	}
}