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
	public static void main(String[] param) {
		(new virtualModem()).rx();
	}

	public void rx() {
		int k;
		Modem modem;
		modem = new Modem();
		modem.setSpeed(1000);
		modem.setTimeout(2000);
		modem.open("ithaki");
		for (;;) {
			try {
				k = modem.read();
				if (k == -1)
					break;
				System.out.print((char) k);
			} catch (Exception x) {
				break;
			}
		}
		modem.close();
	}
}