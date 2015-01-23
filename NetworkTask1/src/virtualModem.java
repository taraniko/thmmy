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

import ithakimodem.*;

public class virtualModem {
	// TODO Add exception catches to Reads
	private static final String ECHO_REQUEST_CODE = "E7882";
	private static final String IMAGE_REQUEST_CODE = "M2428";
	private static final String IMAGE_REQUEST_CODE_E = "G6558";
	private static final String GPS_REQUEST_CODE = "P9448";
	private static final String ACK_RESULT_CODE = "8180";
	private static final String NACK_RESULT_CODE = "0775";

	private long time;
	private String sendMessage = ""; // sending message
	String rcvMessage = ""; // received message
	private String buffer = ""; // buffer for prototype recognizing
	private int bufferSize = 0;

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
		for (;;) {
			k = (char) modem.read();
			if (k == -1) {
				System.out.println("Read GPS failed. -1");
				return false;
			}
			System.out.print(k);
			if(k == -1) break; //must be deleted
		}

		return true;
	}

	public void rx() throws IOException {

		Modem modem;

		modem = new Modem();
		modem.setSpeed(80000);
		modem.setTimeout(2000);
		modem.open("ithaki");
		// TODO zero buffer

		readIntro(modem);
		// ECHO REQUEST
		 for(int i=0; i<100; i++){
		 request(ECHO_REQUEST_CODE,modem); // send an echo request to the modem
		 time = System.currentTimeMillis(); //holds time after sending Echo
		// request
		 rcvMessage = recvEchoPacket(modem);
		 }
		// JPEG REQUEST (no errors)
		 request(IMAGE_REQUEST_CODE,modem);
		 rcvImage("image_no_errors.jpeg",modem);
		 request(IMAGE_REQUEST_CODE_E, modem);
		 rcvImage("image_with_errors.jpeg", modem);
		request(GPS_REQUEST_CODE, modem);
		readGPS(modem);
		modem.close();

	}
}
