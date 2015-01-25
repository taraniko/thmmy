/*
 *
 * Diktya Ypologistwn I
 *
 * Experimental Virtual Lab
 *
 * Java virtual modem communications seed code
 *
 */
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.sun.xml.internal.ws.api.message.Packet;

import ithakimodem.*;

public class virtualModem {
	
	Modem modem;
	// TODO Add exception catches to Reads
	private static final String ECHO_REQUEST_CODE = "E7274";
	private static final String IMAGE_REQUEST_CODE = "M9670";
	private static final String IMAGE_REQUEST_CODE_E = "G0604";
	private static final String GPS_REQUEST_CODE = "P8653";
	private static final String ACK_RESULT_CODE = "Q5394";
	private static final String NACK_RESULT_CODE = "R1135";
	
	private String sendMessage = ""; // sending message
	String rcvMessage = ""; // received message
	private String buffer = ""; // buffer for prototype recognizing
	private int bufferSize = 0;
	private String width[];
	private String length[];
	private int parameters = 0;
	private int errorPackets = 0;
	private int noErrorPackets = 0;

	virtualModem() {
		//Init Modem
		modem = new Modem();
		modem.setSpeed(80000);
		modem.setTimeout(2000);
		modem.open("ithaki");
		
		width = new String[100];
		length = new String[100];
	}
	

	public static void main(String[] param) throws IOException {
		(new virtualModem()).rx();
	}

	public void request(String code) {
		byte[] b; // characters to send
		sendMessage = code + "\r";
		b = sendMessage.getBytes();
		modem.write(b);
	}

	public boolean rcvImage(String imageName) throws IOException {

		int k; // received char
		int previous; // previous char
		FileOutputStream image;

		try {
			image = new FileOutputStream(imageName);
		} catch (FileNotFoundException e) {
			System.out.println("Impossible to open file.");
			return false;
		}

		previous = 0;
		System.out.println("Starting image (" + imageName + ") download!");
		for (;;) {
			try {
				k = modem.read();
				if (k == -1) {
					System.out.println("Read -1 from modem on image reception");
					image.close();
					return false;
				}
			} catch (Exception x) {
				System.out
						.println("Error connecting to modem on image reception");
				image.close();
				return false;
			}
			try {
				image.write(k);
			} catch (IOException e) {
				System.out.println("Impossible to write data to file");
				image.close();
				return false;
			}
			if (previous == 0xFF && k == 0xD9) {
				System.out.println("Image complete!");
				break;
			}
			previous = k;
		}
		image.close();
		return true;
	}

	public boolean foundSequence(String seq) {
		// drop out first buffer char if buffer is full
		if (buffer.length() > seq.length()) {
			buffer = buffer.substring(1);
		}
		if (buffer.equals(seq)) {
			return true;
		} else {
			return false;
		}
	}

	public void recvEchoPacket(int timeout) {
		char k; // received character
		long time;

		buffer = "";
		long temp = System.currentTimeMillis();
		while (!foundSequence("PSTART")) {
			k = (char) modem.read();
			if(k==-1){
				System.out.println("Failed receiving packet!");
				return;
			}
			time = System.currentTimeMillis() - temp;
			if(time>=timeout){
				System.out.println("Reading Timeout. Failed receiving packets!");
			}
			buffer += k;
		}
		// else (found PSTART)
		buffer = "";
		rcvMessage = "PSTART";
		while (!foundSequence("PSTOP")) {
			k = (char) modem.read();
			buffer += k;
			rcvMessage += k;
			// TODO add timeout
		}
		// if timed out return a bad string
		// else (FOUND PSTOP)
		return;
	}


	public boolean readIntro() {
		char k = ' '; // received character
		rcvMessage = "";
		while (!rcvMessage.endsWith("\r\n\n\n")) {
			try {
				// Read a char from the modem
				k = (char) modem.read();
				if (k == -1)
					break;
				rcvMessage += k;
			} catch (Exception x) {
				break;
			}
		}
		if (k == (int) -1) {
			return false; // failed to read
		} // reading loop
		System.out.println("Intro finished!");
		return true;
	}

	public boolean readGPS() {
		char k; // received character
		int MAX_SECONDS = 10;
		int counter = MAX_SECONDS;
		rcvMessage = "";
		for (;;) {
			k = (char) modem.read();
			if (k == -1) {
				System.out.println("Read GPS failed. -1");
				return false;
			}
			// Code if character received
			rcvMessage += k;
			if (rcvMessage.endsWith("START ITHAKI GPS TRACKING\r\n")) {
				System.out.println("Started GPS reception");
				rcvMessage = "";
			}
			if (rcvMessage.endsWith("STOP ITHAKI GPS TRACKING\r\n")) {
				System.out.println("Stopped GPS reception");
				System.out.println("Found " + parameters + " coordinates");
				rcvMessage = "";
				break;
			}
			if (rcvMessage.endsWith("\r\n")) { // found GPS line
				if (counter == MAX_SECONDS) { // MAX_SECONDS time passed
					getGPSparameters();
					//Got one GPS parameter
					counter = 0;
				} else {
					counter++;
				}
				rcvMessage = "";
			}
		}
		return true;
	}

	void getGPSparameters() {
		width[parameters] = rcvMessage.substring(18, 27);
		length[parameters] = rcvMessage.substring(31, 40); // I ommit the first
															// zero
		parameters++; // new GPS coordinates received;
	}

	String decToDegrees(String coord) {
		String degrees = "";
		int dec;
		float converted;
		degrees += coord.substring(0, 4);
		dec = Integer.parseInt(coord.substring(5));
		converted = dec * 6 / 1000;
		converted = Math.round(converted);

		if (converted < 10) {
			degrees += 0;
		}
		degrees += (int) converted;
		return degrees;
	}

	void getGPSimage(String imageName) throws IOException {
		sendMessage = GPS_REQUEST_CODE;
		for (int i = 0; i < parameters; i++) {
			sendMessage += "T=";
			sendMessage += decToDegrees(length[i]);
			sendMessage += decToDegrees(width[i]);
		}
		request(sendMessage);
		rcvImage(imageName);
	}

	public int errorCorrection() {
		int resends=0;
		char k;
		boolean foundError = true; // in order to get in the loop
		rcvMessage = "";

		while (foundError) {
			for (;;) {
				k = (char) modem.read();
				if (k == -1) {
					System.out.println("Read error.");
					return -1;
				}
				rcvMessage += k;
				if (rcvMessage.endsWith("PSTOP")) {
					break; // stop reading chars
				}
			} // end reading chars
			foundError = errorDetect();
			if (foundError) {
				errorPackets++;
				request(NACK_RESULT_CODE);
				resends++;
			}
			rcvMessage = "";
		}
		noErrorPackets++;
		//received No Errors Packet
		rcvMessage = "";
		return resends;
	}

	public boolean errorDetect() {
		int startPacket, endPacket;
		byte packetBytes[] = new byte[20];
		int FCS;
		int xorResult;
		String packet = "";

		// find packet and FCS
		startPacket = rcvMessage.indexOf('<') + 1;
		endPacket = rcvMessage.indexOf('>');
		packet = rcvMessage.substring(startPacket, endPacket);
		FCS = Integer.parseInt(rcvMessage.substring(endPacket + 2,
				endPacket + 5));

		// Implement xor
		packetBytes = packet.getBytes();
		xorResult = packetBytes[0];
		for (int i = 1; i < 16; i++) {
			xorResult ^= packetBytes[i];
		}
		if (xorResult == FCS) {
			return false; // no error found
		} else
			return true; // found error
	}

	
	public void saveCodesToFile(String codesFileName) throws IOException{
		
		FileOutputStream codesFile;
		try {
			//codesFile = new FileOutputStream(codesFileName);
			codesFile = new FileOutputStream(codesFileName); 
		} catch (FileNotFoundException e) {
			System.out.println("Impossible to open file.");
			return;
		}
		sendMessage = "Echo request code: " + ECHO_REQUEST_CODE + "\r\n";
		codesFile.write(sendMessage.getBytes());
		sendMessage = "Image request code: " + IMAGE_REQUEST_CODE + "\r\n";
		codesFile.write(sendMessage.getBytes());
		sendMessage = "Image request code(errors): " + IMAGE_REQUEST_CODE_E + "\r\n";
		codesFile.write(sendMessage.getBytes());
		sendMessage = "GPS request code: " + GPS_REQUEST_CODE + "\r\n";
		codesFile.write(sendMessage.getBytes());
		sendMessage = "ACK result code: " + ACK_RESULT_CODE + "\r\n";
		codesFile.write(sendMessage.getBytes());
		sendMessage = "NACK result code: " + NACK_RESULT_CODE + "\r\n";
		codesFile.write(sendMessage.getBytes());
		codesFile.close();
		System.out.println("Wrote codes in a file ("+codesFileName+")");
	}
	
	public void simplePacketRequest(int sessionTime, String filename) throws IOException {
		long packetNumber = 0;
		long seconds;
		long temp;
		readIntro();
		FileOutputStream file;
		try {
			//codesFile = new FileOutputStream(codesFileName);
			file = new FileOutputStream(filename); 
		} catch (FileNotFoundException e) {
			System.out.println("Impossible to open file.");
			return;
		}
		sendMessage = "Packet,Time(ms)\r\n";
		file.write(sendMessage.getBytes());
		long time;
		temp = System.currentTimeMillis();
		seconds = 0;
		System.out.println("Started receiving packets for "+ sessionTime + " seconds.");
		// ECHO REQUEST
		while (seconds < sessionTime) {
			packetNumber++;
			request(ECHO_REQUEST_CODE); // send an echo request to the
												// modem
			time = System.currentTimeMillis(); // holds time after sending Echo
			// request
			recvEchoPacket(3000);
			//measure time difference
			time = System.currentTimeMillis() - time;
			sendMessage = packetNumber +"," + time + "\r\n";
			file.write(sendMessage.getBytes());
			seconds = (System.currentTimeMillis() - temp) / 1000;
		}
		System.out.println("Finished receiving packets.");
		file.close();
	}
	
	public void receiveImages(String imageNoErrors, String imageErrors) throws IOException{
		// JPEG REQUEST (no errors)
		request(IMAGE_REQUEST_CODE);
		rcvImage(imageNoErrors);
		request(IMAGE_REQUEST_CODE_E);
		rcvImage(imageErrors);
	}
	
	public void receiveGPS(String imageName) throws IOException{
		request(GPS_REQUEST_CODE + "R=1000140");
		readGPS();
		getGPSimage(imageName);
	}
	
	public void ARQpacketRequest(int sessionTime, String filename) throws IOException{
		
		long packetNumber = 0;
		long seconds = 0;
		long time;
		int resends;
		
		FileOutputStream file = new FileOutputStream(filename); 
		
		sendMessage = "Packet,Time(ms),Resends\r\n";
		file.write(sendMessage.getBytes());
		System.out.println("Started receiving ARQ packets.");
		long temp = System.currentTimeMillis();
		while (seconds < sessionTime) {
			packetNumber++;
			request(ACK_RESULT_CODE);
			time = System.currentTimeMillis();
			resends = errorCorrection();
			time = System.currentTimeMillis() - time;
			sendMessage = packetNumber +"," + time +","+resends+ "\r\n";
			file.write(sendMessage.getBytes());
			seconds = (System.currentTimeMillis() - temp) / 1000;	
		}
		System.out.println("Finished receiving packets.");
		file.close();
	}

	public void rx() throws IOException {

		saveCodesToFile("codes.txt");
		simplePacketRequest(5*60,"simple_packets.txt");
		receiveImages("image_no_errors.jpeg","image_with_errors.jpeg");
		receiveGPS("image_GPS.jpeg");
		ARQpacketRequest(5*60,"ARQ_packets.txt");

		modem.close();

	}
}
