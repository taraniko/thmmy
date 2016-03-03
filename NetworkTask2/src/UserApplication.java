import java.io.IOException;
import java.net.*;

import javax.sound.sampled.*;


public class UserApplication {

	private static final int SERVER_PORT = 8000;
	private static final int CLIENT_PORT = 9000;
	private static final String ECHO_REQUEST_CODE = "";
	
	
	
	public static void main(String[] args) throws IOException
	{
		(new UserApplication()).start();
	}
	
	public void start() throws IOException
	{
		initSocket();
	}
	
	public void initSocket() throws IOException
	{
		DatagramSocket socket = new DatagramSocket();
		int serverPort = SERVER_PORT;
		byte[] hostIP = { (byte)155,(byte)207,18,(byte)208 };
		InetAddress hostAddress = InetAddress.getByAddress(hostIP);
		
		
		
		
	}
	
	public static void helper() throws IOException {
		// TODO Auto-generated method stub
		DatagramSocket s = new DatagramSocket();
		String packetInfo = "Hello there !";
		byte[] txbuffer = packetInfo.getBytes();
		int serverPort = 2008;
		byte[] hostIP = { (byte)155,(byte)207,18,(byte)208 };
		InetAddress hostAddress = InetAddress.getByAddress(hostIP);
		DatagramPacket p = new DatagramPacket(txbuffer,txbuffer.length,
		hostAddress,serverPort);
		s.send(p);
		int clientPort = 2004;
		DatagramSocket r = new DatagramSocket(clientPort);
		r.setSoTimeout(800);
		byte[] rxbuffer = new byte[2048];
		DatagramPacket q = new DatagramPacket(rxbuffer,rxbuffer.length);
		String message;
		for (;;) {
			try {
				r.receive(q);
				message = new String(rxbuffer,0,q.getLength());
			} catch (Exception x) {
				System.out.println(x);
			}
		}
	}

}
