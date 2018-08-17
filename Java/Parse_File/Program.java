import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * @version 1.0
 * @author Judicael MARCHAND
 * This program is used to parse the file made by the main MicroITS program. It reads all the line and parses it to be a CSV file which can be open by a spreadsheet.
 * The input file has to be called "log_oneshot.txt" and the output file will be called "log_format.csv".
 *
 */
public class Program {
	
	/**
	 * The delemiter character used to separate the values in the CSV file
	 */
	private static char CHARACTER_DELEMITER = ',';
	
	/**
	 * @param string The line read
	 * @return The hour in the file
	 */
	public static String getHour(String string)
	{
		String temp = null;
		int index;
		
		index = string.indexOf('-');
		temp = string.substring(0, index-1);	// From the beginning until the space before '-'
		
		return temp;
	}
	
	/**
	 * @param string The line read
	 * @return The date in the file
	 */
	public static String getDate(String string)
	{
		String temp = null;
		int index;
		
		index = string.indexOf('-');
		temp = string.substring(index+2);	// From the space after '-'
		
		return temp;
	}
	
	/*
	 * @param string The line read
	 * @return The quality of the signal in the file
	 */
	public static String getQuality(String string)
	{
		String temp = null;
		int indexEnd, indexBegin;
		
		indexBegin = string.indexOf('=');
		indexEnd = indexBegin +3;
		temp = string.substring(indexBegin+1, indexEnd);
		
		return temp;
	}
	
	/**
	 * @param string The line read
	 * @return The signal level of the signal in the file
	 */
	public static String getSignalLevel(String string)
	{
		String temp = null;
		int indexBegin, indexEnd;
		
		indexBegin = string.indexOf('=', 9);
		indexEnd = string.indexOf(' ', 22);
		temp = string.substring(indexBegin+1, indexEnd);
		
		return temp;
	}
	
	/**
	 * @param string The line read
	 * @return The number of packets lost, received and errors in the file
	 */
	public static String getPackets(String string)
	{
		String temp = null;
		int indexBegin, indexEnd;
		
		// Number of packets
		indexBegin = string.indexOf(':');
		indexEnd = string.indexOf(' ', 3);
		System.out.println(Integer.toString(indexBegin) + ", "  + Integer.toString(indexEnd));
		temp = string.substring(indexBegin+1, indexEnd) + CHARACTER_DELEMITER;
		
		// Number of error packets
		indexBegin = string.indexOf(':', 15);
		indexEnd = string.indexOf(' ', 21);
		System.out.println(Integer.toString(indexBegin) + ", "  + Integer.toString(indexEnd));
		temp += string.substring(indexBegin+1, indexEnd) + CHARACTER_DELEMITER;
		
		// Number of dropped packets
		indexBegin = string.indexOf(':', 24);
		indexEnd = string.indexOf(' ', 30);
		System.out.println(Integer.toString(indexBegin) + ", "  + Integer.toString(indexEnd));
		temp += string.substring(indexBegin+1, indexEnd);
		
		return temp;
	}
	
	/**
	 * @param string The line read
	 * @return The position of the robot in the file
	 */
	public static String getPosition(String string)
	{
		String temp = null;
		int indexBegin, indexEnd;
		
		// X position
		indexBegin = 0;
		indexEnd = string.indexOf(',');
		temp = string.substring(indexBegin, indexEnd) + CHARACTER_DELEMITER;
		
		// Y position
		indexBegin = string.indexOf(',');
		temp += string.substring(indexBegin+1);
		
		return temp;
	}
	
	/**
	 * @param string The line read
	 * @return The time of a round trip of a message between the two robots
	 */
	public static String getTimeRoundTrip(String string)
	{
		return string;
	}
	
	/**
	 * @param file The file in which the string has to be written
	 * @param s  The string to write in the file
	 */
	public static void writeFormatFile(File file, String s)
	{
		try 
		{
			BufferedWriter writer = new BufferedWriter(new FileWriter(file, true));
			writer.append(s + "\n");
			writer.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	/**
	 * @param file The file to erase
	 */
	public static void eraseFile(File file)
	{
		try 
		{
			BufferedWriter writer = new BufferedWriter(new FileWriter(file));
			writer.write("");
			writer.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	/**
	 * @param file The file to init with the header
	 */
	public static void initFile(File file)
	{
		try 
		{
			BufferedWriter writer = new BufferedWriter(new FileWriter(file));
			writer.write("Date,Hour,Quality of signal (/70),Signal level (dbm),Rx Packets,Rx Errors,Rx Dropped,Tx Packets,Tx Errors,Tx Dropped,X Position,Y Position, X Position (other robot), Y Position (other robot), Time of a round trip (s)\n");
			//writer.write("Date,Hour,X Position,Y Position\n");
			writer.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		// File
		File source = new File("./log_oneshot.txt");
		File fileFormat = new File("./log_format.csv");
		eraseFile(fileFormat);
		initFile(fileFormat);
		// Lists
		List<String> lines = new ArrayList<String>();
		List<List<String>>data = new ArrayList<List<String>>();
		String line;
		// Format
		String toWrite;
		int indexLine;
				
		try
		{
			BufferedReader reader = new BufferedReader(new FileReader(source));	// Open the buffer to read the file
			while((line = reader.readLine()) != null)	// While there is text to read in the file
			{
				if(!line.equals(";"))
				{
					lines.add(line.trim());	// Add the line read in the variable
				}
				else
				{
					data.add(lines);	// Add the list in the data list
					//lines.clear();	// DON'T USE THIS LINE BECAUSE IT ERASES THE LINE IN THE DATA VARIABLE
					lines = new ArrayList<String>();	// Reset the lines
				}
			}
			reader.close();	// Close the buffer
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		
		for(List<String> list : data)
		{
			// Initialization variable
			indexLine = 0;
			toWrite = "";
			if(list.size() < 2) // Incomplete list or with errors
			{
				System.out.println("ERROR WITH A LIST");
			}
			else
			{
				// Get hour and date
				line = list.get(indexLine++);
				toWrite += getHour(line) + CHARACTER_DELEMITER;
				toWrite += getDate(line) + CHARACTER_DELEMITER;
				// Cell Address
				line = list.get(indexLine++);
				// Quality and signal level
				line = list.get(indexLine++);
				toWrite += getQuality(line) + CHARACTER_DELEMITER;
				toWrite += getSignalLevel(line) + CHARACTER_DELEMITER;
				// ESSID
				line = list.get(indexLine++);
				// Rx and Tx packets
				line = list.get(indexLine++);
				toWrite += getPackets(line) + CHARACTER_DELEMITER;
				line = list.get(indexLine++);
				toWrite += getPackets(line) + CHARACTER_DELEMITER;
				// Position actual robot
				line = list.get(indexLine++);
				toWrite += getPosition(line) + CHARACTER_DELEMITER;
				// Position other robot
				line = list.get(indexLine++);
				toWrite += getPosition(line) + CHARACTER_DELEMITER;
				// Time for the information to do a round trip
				line = list.get(indexLine++);
				toWrite += getTimeRoundTrip(line) + CHARACTER_DELEMITER;
				
				writeFormatFile(fileFormat, toWrite);
				
				System.out.println(toWrite);
			}
		}
	}

}
