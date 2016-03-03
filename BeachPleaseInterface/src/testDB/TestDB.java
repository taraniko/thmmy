package testDB;

import java.sql.*;

public class TestDB {

	public static void main(String[] args) {

		// Query1.java: Query an mSQL database using JDBC.

		/**
		 * A JDBC SELECT (JDBC query) example program.
		 */

		try {
			String userName = "nikos";
			String password = "taras";

			String url ="jdbc:sqlserver://37.6.143.67:1433;databaseName=stelios";
			//String url = "jdbc:sqlserver://NIKOLAS-PC\\SQLEXPRESS;databaseName=stelios;integratedSecurity=true";
			// String url
			// ="jdbc:sqlserver://NIKOLAS-PC\\SQLEXPRESS;databaseName=stelios";
			Class.forName("com.microsoft.sqlserver.jdbc.SQLServerDriver");
			// Connection conn = DriverManager.getConnection(url);
			Connection conn = DriverManager.getConnection(url, userName, password);

			Statement stmt = conn.createStatement();
			ResultSet rs;

			rs = stmt.executeQuery("SELECT * FROM Accommodation");
			while (rs.next()) {
				String lastName = rs.getString("Name");
				System.out.println(lastName);
			}
			conn.close();
		} catch (Exception e) {
			System.err.println("Got an exception! ");
			System.err.println(e.getMessage());
		}

	}

}
