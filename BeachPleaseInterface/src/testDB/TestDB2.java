package testDB;

import java.awt.EventQueue;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import javax.swing.JFrame;

class TestDB2 {

	private JFrame frame;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					new ConnectDB();
					TestDB2 window = new TestDB2();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public TestDB2() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setBounds(100, 100, 450, 300);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

}

class ConnectDB
{
	public static Connection conn;
	public static Statement stmt;
	
	public ConnectDB()
	{
		try {
			String userName = "nikos";
			String password = "taras";

			String url ="jdbc:sqlserver://37.6.143.67:1433;databaseName=stelios";
		
			Class.forName("com.microsoft.sqlserver.jdbc.SQLServerDriver");

			conn = DriverManager.getConnection(url, userName, password);

			stmt = conn.createStatement();
			testQuery();
			
			//conn.close();
		} catch (Exception e) {
			System.err.println("Got an exception! ");
			System.err.println(e.getMessage());
		}
	}
	
	public void testQuery()
	{	
		try {
			ResultSet rs;
			rs = stmt.executeQuery("SELECT * FROM Accommodation");
			while (rs.next()) {
				String lastName = rs.getString("Name");
				System.out.println(lastName);
			}
		} catch (SQLException e) {
			System.err.println("Got an exception! ");
			System.err.println(e.getMessage());
		}
		
		
	}
}
