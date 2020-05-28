package Frame;

import java.awt.GridLayout;
import java.awt.Rectangle;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

public class ProcessFrame extends JFrame{
	
	private String[] processArr;
	JPanel jp = new JPanel();
	JScrollPane scr;
	
	public ProcessFrame(String str) {
		super("프로세스 정보");
		this.setSize(250,200);;
		this.processArr = str.split("``");
		int len = processArr.length;
		jp.setLayout(new GridLayout(0,1));
		scr = new JScrollPane(jp);
		for(int i=1; i<len; i++) {
			jp.add(new JLabel(processArr[i]));
		}
		add(scr);
		this.setVisible(true);
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
	}
}
