import java.awt.GridLayout;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

/**
 * This class InterfaceManager manages the graphical interface of the application.
 * 
 * @version 1.0
 * @author Judicael MARCHAND
 */
@SuppressWarnings("serial")
public class InterfaceManager extends JFrame implements KeyListener {

	private final String ipList[] = {"192.168.1.1", "192.168.1.2"};
	
	// Graphical interface
	private JPanel panel;
	//private JTextField textIP;
	private JTextField textPort;
	private JTextField textCommand;
	private JFrame frame;
	private JPanel panelUp;
	private JPanel panelDown;
	private JLabel labelIP;
	private JLabel labelPort;
	private JLabel labelTextZone;
	private JComboBox<String> boxIP;
	
	// Interaction variables
	private boolean state;
	private char key;
	private int keyCode;
	private String action;
	
	/**
	 * Creates the object InterfaceManager, initializes the window and resets actions of the keyboard 
	 */
	
	InterfaceManager()
	{
		initializeComponents();
		resetAction();
	}
	
	/**
	 * Returns the state of the key (if it was pressed/released or no)
	 * @return Boolean to tell if the key was pressed/released (true if pressed/released, false if not)
	 */
	
	public boolean getStateKey()
	{
		return state;
	}
	
	/**
	 * Returns the key pressed/released
	 * @return Char : character of the key
	 */
	
	public char getKey()
	{
		return key;
	}
	
	/**
	 * Returns the code of the key pressed/released
	 * @return Int : Code of the key
	 */
	
	public int getKeyCode()
	{
		return keyCode;
	}
	
	/**
	 * Returns the action of the key
	 * @return String : "PRESSED" or "RELEASED"
	 */
	
	public String getAction()
	{
		return action;
	}
	
	/**
	 * Resets the intern variables when the event of key pressed/released is read
	 */
	
	public void resetAction()
	{
		state = false;
		key = ' ';
		action = "";
	}
	
	/**
	 * Returns the text of the IP TextField
	 * @return String : text of the IP TextField
	 */
	
	public String getIPText()
	{
		//return textIP.getText();
		return (String) boxIP.getSelectedItem();
	}
	
	/**
	 * Returns the text of the port TextField
	 * @return String : text of the port TextField
	 */
	
	public String getPortText()
	{
		return textPort.getText();
	}
	
	/**
	 * Gets the event of a key pressed and sets the intern variables
	 */
	
	@Override
	public void keyPressed(KeyEvent arg0) {
		// TODO Auto-generated method stub
		key = arg0.getKeyChar();
		keyCode = arg0.getKeyCode();
		action = "PRESSED";
		state = true;		
	}
	
	/**
	 * Gets the event of a key released and sets the intern variables
	 */

	@Override
	public void keyReleased(KeyEvent arg0) {
		// TODO Auto-generated method stub
		key = arg0.getKeyChar();
		action = "RELEASED";
		state = true;
	}
	
	/**
	 * Not used
	 */

	@Override
	public void keyTyped(KeyEvent arg0) {
		// TODO Auto-generated method stub
	}
	
	
	/**
	 * Initializes the window
	 */
	
	private void initializeComponents() {
		// Initialize frame
		frame = new JFrame("Control robots");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(600, 400);
		
		// Initialize panels
		panel = new JPanel();
		panelUp = new JPanel();
		panelDown = new JPanel();
		
		panel.setLayout(new GridLayout(2,1));	// 2 lines - 1 column
		panelUp.setLayout(new GridLayout(2,3));	// 2 lines - 3 columns
		panelDown.setLayout(new GridLayout(2,1));	// 2 lines - 1 column
		
		// Initialize text fields
		//textIP = new JTextField();
		boxIP = new JComboBox<>(ipList);
		textPort = new JTextField();
		textCommand = new JTextField();
		
		//textIP.setText("192.168.1.1");
		textPort.setText("4200");
		
		// Initialize labels
		labelIP = new JLabel("IP Address");
		labelIP.setHorizontalAlignment(JLabel.CENTER);
		labelPort = new JLabel("Port");
		labelPort.setHorizontalAlignment(JLabel.CENTER);
		labelTextZone = new JLabel("Command Zone");
		labelTextZone.setHorizontalAlignment(JLabel.CENTER);
		
		// Add components in the panels
		panelUp.add(labelIP);
		panelUp.add(labelPort);
		//panelUp.add(textIP);
		panelUp.add(boxIP);
		panelUp.add(textPort);
		panelDown.add(labelTextZone);
		panelDown.add(textCommand);
		
		panel.add(panelUp);
		panel.add(panelDown);
		
		// Add panel in the frame
		frame.add(panel);
		
		// Make the frame visible
		frame.setVisible(true);
		
		// Add key listener
		textCommand.addKeyListener(this);
	}
	
	/**
	 * Show a message to the user through a dialog box
	 * @param s String to show to the user
	 */
	
	public void showAlertMessage(String s)
	{
		JOptionPane.showMessageDialog(frame, s);
	}
	
}
