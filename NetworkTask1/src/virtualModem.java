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
	// TODO Add exception catches to Reads
	private static final String ECHO_REQUEST_CODE = "E2563";
	private static final String IMAGE_REQUEST_CODE = "M0825";
	private static final String IMAGE_REQUEST_CODE_E = "G7479";
	private static final String GPS_REQUEST_CODE = "P2513";
	private static final String ACK_RESULT_CODE = "Q7023";
	private static final String NACK_RESULT_CODE = "R7137";

	private long time;
	private String sendMessage = ""; // sending message
	String rcvMessage = ""; // received message
	private String buffer = ""; // buffer for prototype recognizing
	private int bufferSize = 0;
	private String width[];
	private String length[];
	private int parameters = 0;
	private int errorPackets = 0;
	private int noErrorPackets = 0;

	public virtualModem() {
		width = new String[100];
		length = new String[100];
	}

	public static void main(String[] param) throws IOException {
		(new virtualModem()).rx();
	}

	public void request(String code, Modem modem) {
		byte[] b; // characters to send
		sendMessage = code + "\r";
		b = sendMessage.getBytes();
		modem.write(b);
	}

	public boolean rcvImage(String imageName, Modem modem) throws IOException {

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
		System.out.println("Starting image download!");
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

	public String recvEchoPacket(Modem modem) {
		char k; // received character
		long elapsedTime;

		buffer = "";
		while (!foundSequence("PSTART")) {
			k = (char) modem.read();
			buffer += k;
			// TODO add timeout
		}
		// if timed out return a bad string
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
		elapsedTime = System.currentTimeMillis() - time;
		System.out.println("Packet time: " + elapsedTime + "ms");
		System.out.println(rcvMessage);
		return rcvMessage;
	}

	public boolean readGreeting(Modem modem) {
		char k = ' '; // received character
		boolean foundCR;
		int countLF;

		int i = 0; // temp used for counting
		// Read one byte loop
		foundCR = false;
		countLF = 0;
		for (;;) {
			try {
				// Read a char from the modem
				k = (char) modem.read();
				if (k == -1)
					break;
				if (k == '\r')
					foundCR = true; //
				else if (k == '\n' && foundCR)
					countLF++;
				else {
					foundCR = false;
					countLF = 0;
				}
				System.out.print((char) k);
				rcvMessage += k;

				if (countLF == 3) { // found the end of the reading stream
					foundCR = false;
					countLF = 0;
					System.out.println("Message Ended!");
					break; // break reading loop
				}

			} catch (Exception x) {
				break;
			}
		}
		if (k == (int) -1) {
			return false; // failed to read
		} // reading loop

		// TODO empty string "rcvMessage"
		return true;
	}

	public boolean readIntro(Modem modem) {
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

	public boolean readGPS(Modem modem) {
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
				System.out.println("\nStarted GPS reception\n");
				rcvMessage = "";
			}
			if (rcvMessage.endsWith("STOP ITHAKI GPS TRACKING\r\n")) {
				System.out.println("\nStopped GPS reception\n");
				System.out.println("Found " + parameters + " coordinates");
				rcvMessage = "";
				break;
			}
			if (rcvMessage.endsWith("\r\n")) { // found GPS line
				if (counter == MAX_SECONDS) { // MAX_SECONDS time passed
					getGPSparameters();
					System.out.println("Got one signal\n");
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
		int i; // used for counting
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

	void getGPSimage(Modem modem) throws IOException {
		sendMessage = GPS_REQUEST_CODE;
		for (int i = 0; i < parameters; i++) {
			sendMessage += "T=";
			sendMessage += decToDegrees(length[i]);
			sendMessage += decToDegrees(width[i]);
		}
		request(sendMessage, modem);
		rcvImage("image_GPS.jpeg", modem);
	}

	boolean errorCorrection(Modem modem) {
		char k;
		boolean foundError = true; // in order to get in the loop
		rcvMessage = "";
		request(ACK_RESULT_CODE, modem);
		System.out.println("ACK sent");

		while (foundError) {
			for (;;) {
				k = (char) modem.read();
				if (k == -1) {
					System.out.println("Read error.");
					return false;
				}
				rcvMessage += k;
				if (rcvMessage.endsWith("PSTOP")) {
					break; // stop reading chars
				}
			} // end reading chars
			foundError = errorDetect();
			if (foundError) {
				errorPackets++;
				System.out.println("Received ERRORS packet");
				System.out.println("NACK sent");
				request(NACK_RESULT_CODE, modem);
			}
			rcvMessage ="";
		}
		noErrorPackets++;
		System.out.println("Received NO_ERRORS packet");
		rcvMessage = "";
		return true;
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

	// TODO add try catch on modem.read() everywhere
	public void rx() throws IOException {

		Modem modem;

		modem = new Modem();
		modem.setSpeed(80000);
		modem.setTimeout(2000);
		modem.open("ithaki");
			// TODO zero buffer

		readIntro(modem);
			// ECHO REQUEST
		for (int i = 0; i < 10; i++) {
			request(ECHO_REQUEST_CODE, modem); // send an echo request to the
												// modem
			time = System.currentTimeMillis(); // holds time after sending Echo
			// request
			rcvMessage = recvEchoPacket(modem);
		}
			//JPEG REQUEST (no errors)
		request(IMAGE_REQUEST_CODE,modem);
		rcvImage("image_no_errors.jpeg",modem);
		request(IMAGE_REQUEST_CODE_E, modem);
		rcvImage("image_with_errors.jpeg", modem);
		request(GPS_REQUEST_CODE+"R=1000199", modem);
		request("R=1000102",modem);
		readGPS(modem);
		getGPSimage(modem);
		for (int i = 0; i < 1000; i++) {
			errorCorrection(modem);
		}
		System.out.println("Errors: "+errorPackets);
		System.out.println("Corrects: "+(noErrorPackets-errorPackets));
		
		//TODO run only 4 functions in main
		modem.close();

	}
}
