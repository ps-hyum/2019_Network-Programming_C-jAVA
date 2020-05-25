package Frame;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;

import main.PC;
import main.PCs;

public class SeatFrame extends JPanel{
	
	private int numSeat;
	private PCs pm;
	private ServerFrame sf;
	private JLabel usage, usetime, timemoney, menumoney;
	private JButton btn = new JButton();
	
	public SeatFrame(int numSeat, ServerFrame sf, PCs pm) {
		this.numSeat = numSeat;
		this.sf = sf;
		this.pm = pm;
		
		usetime = new JLabel();
		timemoney = new JLabel();
		menumoney = new JLabel();
		String snum = Integer.toString(numSeat);
		setBorder(new TitledBorder(new LineBorder(Color.black,1), "pc " + snum));
		setLayout(new GridLayout(0,1));
		
		add(new JLabel("사용여부"));
		usage = new JLabel("미사용");
		//usage.setBounds();
		add(usage, null);

		btn.setText("pcnum");
		
		this.setVisible(true);
		this.setFocusable(true);
		this.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
            	sf.selectPC(numSeat);
            	sf.changeInfo(pm.getPC(numSeat));
            }
       });
	}
	
	public void setID(String str) {
		usage.setText(str);
	}
	
	public void defaultID() {
		usage.setText("미사용");
	}
}
