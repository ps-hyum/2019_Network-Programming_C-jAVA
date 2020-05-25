package Frame;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

import Main.Client;

public class MenuFrame extends JFrame{
	
	JPanel jp = new JPanel();
	JCheckBox [] menu = new JCheckBox [5]; 
	String [] names = {"���(100��)", "�߰���(150��)", "����Ƣ��(120��)", "�����(50��)", "������(150��)"}; 
	String [] name = {"���", "�߰���", "����Ƣ��", "�����", "������"}; 
	int[] ii = {0,0,0,0,0};
	JLabel sumLabel;
	JButton btn = new JButton("�ֹ�");
	int sum = 0;
	Client cc;
	String menuStr;

	String menuReq = "menuReq";

	public MenuFrame(Client cc) {
		setTitle("�޴�");
		this.cc = cc;
		setDefaultCloseOperation(HIDE_ON_CLOSE);
		setLayout(new BorderLayout());
		jp.setLayout(new GridLayout(0,1));
		MyItemListener listener = new MyItemListener();
		MyActionListener ac = new MyActionListener();
		for(int i=0; i<menu.length; i++) {
			menu[i] = new JCheckBox(names[i]); 
			menu[i].setBorderPainted(true);
			menu[i].addItemListener(listener);
			jp.add(menu[i]);
		}
		sumLabel = new JLabel("���� 0 �� �Դϴ�.");
		btn.addActionListener(ac);;
		add(jp,BorderLayout.CENTER);
		add(sumLabel, BorderLayout.NORTH);
		add(btn, BorderLayout.SOUTH);
		//setSize(150,200);
		this.pack();
		setVisible(true);
	}
	
	class MyActionListener implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			if(sum==0)
				return;
			cc.outStream(menuReq);
			cc.getUser().setMenuMoney(sum);
			cc.getUser().setTotMoney();
			menuStr = Integer.toString(sum) + "``";
			for(int i=0; i<5; i++) {
				if(ii[i] == 1)
					menuStr += name[i] + "``";
				menu[i].setSelected(false);
			}
			System.out.println(sum);
			cc.outStream(menuStr);
			sum = 0;
		}
	}
	
	//��� 100, �߰��� 150, ��Ƣ 120, ����� 50, ������ 150
	class MyItemListener implements ItemListener {		
		public void itemStateChanged(ItemEvent e) {
			if(e.getStateChange() == ItemEvent.SELECTED) { 
				if(e.getItem() == menu[0]) {
					sum += 100;
					ii[0] = 1;
				}
				else if(e.getItem() == menu[1]) {
					sum += 150;
					ii[1] = 1;
				}
				else if(e.getItem() == menu[2]) {
					sum += 120;
					ii[2] = 1;
				}
				else if(e.getItem() == menu[3]) {
					sum += 50;
					ii[3] = 1;
				}
				else {
					sum += 150;
					ii[4] = 1;
				}
			}
			else {
				if(e.getItem() == menu[0]) {
					sum -= 100;
					ii[0] = 0;
				}
				else if(e.getItem() == menu[1]) {
 					sum -= 150;
					ii[1] = 0;
				}
				else if(e.getItem() == menu[2]) {
 					sum -= 120;
					ii[2] = 0;
				}
				else if(e.getItem() == menu[3]) {
 					sum -= 50;
					ii[3] = 0;
				}
				else if(e.getItem() == menu[4]) {
 					sum -= 150;
					ii[4] = 0;
				}
				
			}
			sumLabel.setText("���� " + sum + "�� �Դϴ�."); 
		}
	}
}
