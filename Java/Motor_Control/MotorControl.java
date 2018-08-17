import java.util.concurrent.TimeUnit;

/**
 * This class ClientTCP manages the interaction between the motors of the robot and the TCP client which communicates with the TCP server on the robot. It can be used via a graphical interface to be more friendly.
 * 
 * @version 1.0
 * @author Judicael MARCHAND
 */

public class MotorControl {
	
	// Graphical interface
	private InterfaceManager interfaceManager;
	
	// Motors
	private MotorManager motorManager;
	private char command;
	
	// TCP socket
	private TCPManager tcpManager;
	
	/**
	 * Creates the object ClientTCP
	 */
	
	MotorControl()
	{
		interfaceManager = new InterfaceManager();
		motorManager = new MotorManager();
		tcpManager = new TCPManager();
		
		command = 0;
	}
	
	/**
	 * Launch the clientTCP program
	 */
	
	private void run()
	{		
		while(true)
		{
			try {
				TimeUnit.MILLISECONDS.sleep(10);	// Wait for 100 µs for the CPU to work fine
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if(interfaceManager.getStateKey() == true)	// If a key was pressed/released
			{
				motorManager.manageMotor(interfaceManager.getKeyCode(), interfaceManager.getAction());	// Manage the motors
				interfaceManager.resetAction();	// Reset the action of the key pressed/released
				
				command = motorManager.getCommand();	// Get the commands to send to the server
				
				tcpManager.setIPandPort(interfaceManager.getIPText(), interfaceManager.getPortText());	// Set the IP and the port written in the TextFields
				tcpManager.sendToServer(command);	// Send the commands to the server
			}
		}
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MotorControl tcp = new MotorControl();
		//getIP();
		tcp.run();
		//tcp.test();
	}
}
