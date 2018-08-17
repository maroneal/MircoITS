import java.awt.event.KeyEvent;

/**
 * This class MotorManages manages the configuration of the motors on the robot.
 * 
 * @version 1.0
 * @author Judicael MARCHAND
 */

public class MotorManager {

	// Constants
	private final char COMMAND_FORWARD_PRESSED = (char) 0xA0;
	private final char COMMAND_RIGHT_PRESSED = (char) 0xA1;
	private final char COMMAND_BACKWARD_PRESSED = (char) 0xA2;
	private final char COMMAND_LEFT_PRESSED = (char) 0xA3;
	private final char COMMAND_SPACE_PRESSED = (char) 0xA4;
	private final char COMMAND_FORWARD_RELEASED = (char) 0xA5;
	private final char COMMAND_RIGHT_RELEASED = (char) 0xA6;
	private final char COMMAND_BACKWARD_RELEASED = (char) 0xA7;
	private final char COMMAND_LEFT_RELEASED = (char) 0xA8;
	private final char COMMAND_SPACE_RELEASED = (char) 0xA9;
	
	// Motor variables
	private char command;
	
	/**
	 * Creates the object MotorManager and initializes all variables
	 */
	
	MotorManager()
	{		
		command = 0;
	}
	
	
	/**
	 * Returns the commands of the motors with input, the array to complete
	 * @param com Array of commands to fulfill 
	 */
	
	public char getCommand()
	{
		return command;
	}
	
	/**
	 * Manages the motor as function of the key pressed/released
	 * @see InterfaceManager
	 * @param c Character pressed on the keyboard
	 * @param touch State of the touch (pressed/released)
	 */
	
	public void manageMotor(int c, String touch)
	{
		if(touch == "PRESSED") manageMotorPressed(c);
		else if (touch == "RELEASED") manageMotorReleased(c);
	}
	
	/**
	 * Manages the motor if the key was pressed
	 * @param c Character pressed on the keyboard
	 */
	
	private void manageMotorPressed(int c)
	{
		if (c == KeyEvent.VK_UP || c == KeyEvent.VK_Z || c == KeyEvent.VK_W)	// Forward
		{
			command = COMMAND_FORWARD_PRESSED;
			System.out.println("FORWARD PRESSED");
		}
		else if (c == KeyEvent.VK_RIGHT || c == KeyEvent.VK_D)	// Turn right
		{
			command = COMMAND_RIGHT_PRESSED;
			System.out.println("RIGHT PRESSED");
		}
		else if (c == KeyEvent.VK_DOWN || c == KeyEvent.VK_S)	// Backward
		{
			command = COMMAND_BACKWARD_PRESSED;
			System.out.println("BACKWARD PRESSED");
		}
		else if (c == KeyEvent.VK_LEFT || c == KeyEvent.VK_Q || c == KeyEvent.VK_A)	// Turn left
		{
			command = COMMAND_LEFT_PRESSED;
			System.out.println("LEFT PRESSED");
		}
		else if (c == KeyEvent.VK_SPACE)	// Stop
		{
			command = COMMAND_SPACE_PRESSED;
			System.out.println("SPACE PRESSED");
		}
	}
	
	
	/**
	 * Manages the motor if the key was released
	 * @param c Character pressed on the keyboard
	 */
	
	private void manageMotorReleased(int c)
	{
		// All motors take their previous speed
		if (c == KeyEvent.VK_UP || c == KeyEvent.VK_Z || c == KeyEvent.VK_W)	// Forward
		{
			command = COMMAND_FORWARD_RELEASED;
			System.out.println("FORWARD RELEASED");
		}
		else if (c == KeyEvent.VK_RIGHT || c == KeyEvent.VK_D)	// Turn right
		{
			command = COMMAND_RIGHT_RELEASED;
			System.out.println("RIGHT RELEASED");
		}
		else if (c == KeyEvent.VK_DOWN || c == KeyEvent.VK_S)	// Backward
		{
			command = COMMAND_BACKWARD_RELEASED;
			System.out.println("BACKWARD RELEASED");
		}
		else if (c == KeyEvent.VK_LEFT || c == KeyEvent.VK_Q || c == KeyEvent.VK_A)	// Turn left
		{
			command = COMMAND_LEFT_RELEASED;
			System.out.println("LEFT RELEASED");
		}
		else if (c == KeyEvent.VK_SPACE)	// Stop
		{
			command = COMMAND_SPACE_RELEASED;
			System.out.println("SPACE RELEASED");
		}
	}
	
}
