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
	private static final String ECHO_REQUEST_CODE = "4169";
	private static final String IMAGE_REQUEST_CODE = "3193";
	private static final String IMAGE_REQUEST_CODE_E = "3109";
	private static final String GPS_REQUEST_CODE = "6522";
	private static final String ACK_RESULT_CODE = "8180";
	private static final String NACK_RESULT_CODE = "0775";
	private long time;
	private String sendMessage = ""; // sending message
	String rcvMessage = ""; // received message
	private String buffer = ""; // buffer for prototype recognizing
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

	public boolean foundSequence(String seq) {
		//drop out first buffer char if buffer is full
		if (buffer.length() > seq.length()){
			buffer = buffer.substring(1);
		}
		if (buffer.equals(seq)){
			return true;
		}
		else{
			return false;
		}
	}

	public String recvEchoPacket(Modem modem) {
		char k; // received character
		long elapsedTime;
		
		buffer = "";
		while(!foundSequence("PSTART")){ 
			k = (char) modem.read();
			buffer += k;
			//TODO add timeout
		}
		//if timed out return a bad string
		//else (found PSTART)
		buffer = "";
		rcvMessage = "PSTART";
		while(!foundSequence("PSTOP")){
			k = (char) modem.read();
			buffer += k;
			rcvMessage +=k;
			//TODO add timeout
		}
		//if timed out return a bad string
		//else (FOUND PSTOP)
		elapsedTime = System.currentTimeMillis() - time;
		System.out.println("Packet time: "+ elapsedTime + "ms");
		System.out.println(rcvMessage);
		return rcvMessage;
	}
	
	public boolean readGreeting(Modem modem){
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
						return false; //failed to read
					} // reading loop

					// TODO empty string "rcvMessage"
					return true;
	}

	public void rx() {



		Modem modem;
		
		modem = new Modem();
		modem.setSpeed(1000);
		modem.setTimeout(2000);
		modem.open("ithaki");
		//TODO zero buffer
		
		readGreeting(modem);
		for(int i=0; i<100; i++){
			echoRequest(modem); // send an echo request to the modem
			time = System.currentTimeMillis();
			rcvMessage = recvEchoPacket(modem);
		}
		System.out.println("\n");
		modem.close();

	}
}
