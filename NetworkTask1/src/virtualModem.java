/*
 *
 * Diktya Ypologistwn I
 *
 * Experimental Virtual Lab
 *
 * Java virtual modem communications seed code
 *
 */
import ithakimodem.*;

public class virtualModem {
	private static final String ECHO_REQUEST_CODE = "0498";
	private static final String IMAGE_REQUEST_CODE = "3193";
	private static final String IMAGE_REQUEST_CODE_E = "3109";
	private static final String GPS_REQUEST_CODE = "6522";
	private static final String ACK_RESULT_CODE = "8180";
	private static final String NACK_RESULT_CODE = "0775";

	private String sendMessage = null; // sending message
	private String buffer = null; // buffer for prototype recognizing
	private int bufferSize = 0;

	public static void main(String[] param) {
		(new virtualModem()).rx();
	}

	public void echoRequest(Modem tempModem) {
		byte[] b; // character to send
		sendMessage = "ATE" + ECHO_REQUEST_CODE + "\r";
		b = sendMessage.getBytes();
		for (int i = 0; i < sendMessage.length(); i++) {
			tempModem.write(b);
		}
	}

	public boolean findSequence(String seq, Modem modem) {
		char k; // received character
		for (int i = 0; i < seq.length(); i++) { // escape loop
			try {
				// Read a char from the modem
				k = (char) modem.read();
				buffer += k;
			} catch (Exception x) {
				return false;
			}
		}
		// We filled the buffer
		// now loop adding to the buffer and checking
		for (;;) {
			if (buffer == seq)
				break; // found sequence
			else {
				try {
					// Read a char from the modem
					k = (char) modem.read();
					buffer = buffer.substring(1) + k;
				} catch (Exception x) {
					return false;
				}
			}
		}
		return true;
	}

	public String recvEchoPacket(Modem modem) {
		char k; // received character
		char previous; // previous received character
		buffer = null;
		findSequence("PSTART", modem);
		//TODO read until findSequence.
		return "A";
	}

	public void rx() {

		char k = ' '; // received character
		String rcvMessage = null; // received message

		boolean foundCR;
		int countLF;

		int i = 0; // temp used for counting

		Modem modem;
		modem = new Modem();
		modem.setSpeed(1000);
		modem.setTimeout(2000);
		modem.open("ithaki");

		while (true) {// program loop

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
				break;
			} // reading loop

			// TODO empty string "rcvMessage"

			echoRequest(modem); // send an echo request to the modem
			rcvMessage = recvEchoPacket(modem);

			System.out.println("\n");
		} // general loop
		modem.close();

	}
}
