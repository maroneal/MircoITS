import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * This class TCPManager manages all the communication between the TCP client and the TCP server on the robot.
 * 
 * @version 1.0
 * @author Judicael MARCHAND
 */
public class TCPManager {
	
	// Socket
	Socket socket;
	DataOutputStream buffer;
	
	// Other
	String ip;
	int port;
	int nbCommandsMax;
	
	/**
	 * Creates the object TCPManager
	 * @param nb Number max of commands to send in one array
	 */
	
	TCPManager(int nb)
	{
		ip = "";
		port = 0;
		nbCommandsMax = nb;
	}
	
	TCPManager()
	{
		ip = "";
		port = 0;
		nbCommandsMax = 0;
	}
	
	/**
	 * Connects to the TCP server
	 * @param ip IP of the server
	 * @param port Port of the server
	 */
	
	private void connectToServer(String ip, int port)
	{
		try {
			socket = new Socket(ip, port);
			buffer = new DataOutputStream(socket.getOutputStream());
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/**
	 * Sets the IP and the port of the configuration
	 * @param ip IP of the server
	 * @param port Port of the server
	 */
	
	public void setIPandPort(String ip, String port)
	{
		this.ip = ip;
		this.port = Integer.parseInt(port);
	}
	
	/**
	 * Sends the command to the server
	 * @param command Command to send
	 */
	
	public void sendToServer(char command)
	{
		String toSend = "";
		
		try {
			connectToServer(ip, port);
			//buffer.writeBytes(charToString(command));
			toSend = "192.168.1.3;1;2;3;4;5;6;7;8;" + charToString(command) + ";";
			buffer.write(toSend.getBytes("ISO-8859-1"), 0, toSend.length());	// Tell the bytes to be encoded in ISO-8859-1 to avoid the '0xC2' character on linux
			disconnectFromServer();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/**
	 * Send the array of commands to the server
	 * @param com Array of commands to send
	 */
	
	public void sendToServer(char com[])
	{
		int i;
		String toSend = "192.168.1.3;"
				+ ""
				+ "for;0;0;0;0;0;";
		
		for(i=0; i<nbCommandsMax ; i++)
		{
			try {
				connectToServer(ip, port);
				//buffer.writeBytes(charToString(com[i]));	// Write the command throw the socket
				toSend = "192.168.1.3;0;0;0;0;0;0;0;0;" + charToString(com[i]) + ";";
				buffer.write(toSend.getBytes(), 0, toSend.length());
				disconnectFromServer();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
	}
	
	/**
	 * Send a string to the server
	 * @param s String to send
	 */
	
	/*public void sendToServer(String s)
	{
		try {
			connectToServer(ip, port);
			buffer.writeBytes(s);	// Write the command throw the socket
			disconnectFromServer();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}*/
	
	public void sendToServer(String s)
	{
		connectToServer(ip, port);
		
		try {
			buffer.write(s.getBytes(), 0, s.length());	// Write the command throw the socket
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		disconnectFromServer();
	}
	
	/**
	 * Disconnect from the server
	 */
	
	private void disconnectFromServer()
	{
		// Close the buffer and the socket
		try {
			buffer.close();
			socket.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/**
	 * Transform a char to a String
	 * @param c	Character to transform
	 * @return The String made by the character entered
	 */
	
	private String charToString(char c)
	{
		String s;
		
		s = "";
		s += c;
		
		return s;
	}

}
