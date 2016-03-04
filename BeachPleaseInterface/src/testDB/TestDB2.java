package testDB;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Container;
import java.awt.EventQueue;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

import javax.swing.*;

import java.awt.GridLayout;



class TestDB2 {

	static private JPanel cards;
	static private JFrame frame;
	static private String priviledge;
	static private ConnectDB db;
	TestDB2 window;
	JPanel cardLogin;
    JPanel cardMainAdmin;
    JPanel cardMainUser;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					db = new ConnectDB();
					new TestDB2();
					
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	public TestDB2()
	{
		window = this;
		window.createAndShowGUI();
	}
	private void createAndShowGUI(){
		
		//Create and set up the window.
		frame = new JFrame();
		frame = new JFrame();
		frame.setBounds(100, 100, 320, 240);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
				
		//Create and set up the content pane.
		
		window.addCardsToPane(frame.getContentPane());
				
		//Display the window.
		//frame.pack();
		frame.setVisible(true);
		
	}

	
	
	
	public void addCardsToPane(Container pane) {
        
        //Create the "cards".
        cardLogin = new JPanel();
        cardMainAdmin = new JPanel();
        cardMainUser = new JPanel();
        
        window.fillLoginCard(cardLogin);
        
         
        //Create the panel that contains the "cards".
        cards = new JPanel(new CardLayout());
        cards.add(cardLogin,"login");
        cards.add(cardMainAdmin,"mainAdmin");
        cards.add(cardMainUser,"mainUser");
        cardMainUser.setLayout(new BoxLayout(cardMainUser, BoxLayout.X_AXIS));
        
        
        
        
        
        
        pane.add(cards, BorderLayout.CENTER);
    }
	
	public void fillLoginCard(JPanel cardLogin)
	{
	     JTextField usernameTxt = new JTextField(25);   
	     JTextField passwordTxt = new JPasswordField(25);
	     JLabel usernameLbl = new JLabel("Username: ");
	     JLabel passwordLbl = new JLabel("Password: ");
	     JButton loginButton = new JButton("Login");
	     loginButton.addActionListener(new loginButtonListener(usernameTxt,passwordTxt));
	     cardLogin.add(usernameLbl);
	     cardLogin.add(usernameTxt);
	     cardLogin.add(passwordLbl);
	     cardLogin.add(passwordTxt);
	     cardLogin.add(loginButton);
	}
	
	public class loginButtonListener implements ActionListener{
		JTextField user;
		JTextField pass;
		public loginButtonListener(JTextField user, JTextField pass)
		{
			this.user = user;
			this.pass = pass;
		}
        public void actionPerformed(ActionEvent ev){
            //if username and password is good hide child window
            String username = user.getText();
            String password = pass.getText();
            
            if ((username.equals("admin")) && (password.equals("admin")))
            {
            	priviledge = "admin";
            	window.fillMainAdminCard(cardMainAdmin);
            	CardLayout cardLayout = (CardLayout) cards.getLayout();
            	cardLayout.show(cards, "mainAdmin");
            	
            	
            }
            if ((username.equals("user")) && (password.equals("user")))
            {
            	priviledge = "user";
            	window.fillMainUserCard(cardMainUser);
            	CardLayout cardLayout = (CardLayout) cards.getLayout();
            	cardLayout.show(cards, "mainUser");
            }
            if ((!priviledge.equals("admin")) && (!priviledge.equals("user")))
            {
            	JOptionPane.showMessageDialog(null, "Wrong username/password!", "Error", JOptionPane.ERROR_MESSAGE);
            }
            
             
        }
	}
	
	public void fillMainAdminCard(JPanel cardMain)
	{
		frame.setBounds(100, 100, 640, 480);
		JTextArea textArea = new JTextArea(25,25);
	    cardMain.add(textArea);
	    textArea.setText("Admin Card");
	}
	
	public void fillMainUserCard(JPanel cardMain)
	{
		frame.setBounds(100, 100, 640, 640);
	    
	    JLabel lblDistrict = new JLabel("Περιφέρεια: ");
	    final JComboBox<String> listDistricts = new JComboBox<String>(db.getDistricts());
	    
	    JLabel lblPopularity = new JLabel("Popularity: ");
	    String[] pop = {"All","1","2","3","4","5"};
	    final JComboBox<String> listPopularity = new JComboBox<String>(pop);
	    
	    JLabel lblShower = new JLabel("Ντουζιέρες: ");
	    final JCheckBox chkShower = new JCheckBox();
	    
	    JLabel lblLifeguard = new JLabel("Ναυαγοσώστης: ");
	    final JCheckBox chkLifeguard = new JCheckBox();
	    
	    
	    final JTextArea txtResult = new JTextArea();
	    
	    JButton btnSearch = new JButton("Αναζήτηση");
	    btnSearch.addActionListener(new ActionListener() {
	    	 
            public void actionPerformed(ActionEvent e)
            {
                //Execute when button is pressed
            	
                String res =  db.searchBeaches(listDistricts.getSelectedItem().toString(),listPopularity.getSelectedItem().toString(),
                		chkShower.isSelected(),chkLifeguard.isSelected());
                txtResult.setText(res);
            }
        });     
	    
	    
	    
	    
	    
	    
	    cardMain.setLayout(new GridBagLayout());
	    GridBagConstraints c = new GridBagConstraints();
	    
	    c.gridx = 0;
	    c.gridy = 0;
	    cardMain.add(lblDistrict,c);
	    c.gridx = 2;
	    cardMain.add(listDistricts,c);
	    
	    c.gridx = 0;
	    c.gridy = 1;
	    cardMain.add(lblPopularity,c);
	    c.gridx = 2;
	    cardMain.add(listPopularity,c);
	    
	    c.gridx = 0;
	    c.gridy = 2;
	    cardMain.add(lblShower,c);
	    c.gridx = 2;
	    cardMain.add(chkShower,c);
	    
	    c.gridx = 0;
	    c.gridy = 3;
	    cardMain.add(lblLifeguard,c);
	    c.gridx = 2;
	    cardMain.add(chkLifeguard,c);
	    
	    c.gridx = 2;
	    c.gridy = 4;
	    cardMain.add(btnSearch,c);
	    
	    c.gridy = 5;
	    c.gridx = 1;
	    c.anchor = GridBagConstraints.CENTER;
	    cardMain.add(txtResult,c);
	    
	    
	    
	    
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
		
	public String[] getDistricts()
	{
		ArrayList<String> districts = new ArrayList<String>();
		districts.add("All");
		try {
			ResultSet rs;
			String q = new String("SELECT DISTINCT District FROM Beach");
			rs = stmt.executeQuery(q);
			while (rs.next()){
				String district = rs.getString("District");
				districts.add(district);
			}
		}
		catch(SQLException e)
			{
			System.err.println("Got an exception! ");
			System.err.println(e.getMessage());
			}
		
		String[] dArray = districts.toArray(new String[districts.size()]);
		return dArray;
	}
	
	public String searchBeaches(String district, String popularity, boolean shower,
			boolean lifeguard)
	{
		String q = new String("SELECT * FROM Beach");
		boolean andFlag = false;
		
		if (shower || lifeguard || !district.equals("All") || !popularity.equals("All") )
			q += " WHERE";
		
		if (!district.equals("All")){ 
			q += " District='" + district+ "'";
			andFlag = true;
		}
		
		if (!popularity.equals("All")){
			if (andFlag) q+= " AND";
			q += " Popularity=" + popularity;
			andFlag = true;
		}

		if (shower){
			if (andFlag) q+= " AND";
			q += " Shower='True'";
			andFlag = true;
		}
		
		if (lifeguard){
			if (andFlag) q+= " AND";
			q += " Lifeguard='True'";
			andFlag = true;
		}
		
		System.out.println(q);
		
		String res = new String("");
		ResultSet rs;
		try {
			rs = stmt.executeQuery(q);
			/*ResultSetMetaData rsmd = rs.getMetaData();
		    int columnsNumber = rsmd.getColumnCount();
		    while (rs.next()) {
		        for (int i = 1; i <= columnsNumber; i++) {
		            if (i > 1) res += (",  ");
		            String columnValue = rs.getString(i);
		            res += columnValue + " " + rsmd.getColumnName(i);
		        }
		        res += "\n\r";
		    }
		    */
			boolean flag = false;
			res += "Αποτέλεσμα αναζήτησης:\r\n\r\n";	
			while (rs.next()) {
				flag = true;
				String beach = rs.getString("Name");
				res += beach + "\r\n";
			}
			if (!flag)
				res += "Δεν υπάρχουν παραλίες με αυτά τα κριτήρια!";
				
		} catch (SQLException e) {
			System.err.println("Got an exception! ");
			System.err.println(e.getMessage());
		}
		
		return res;
		
	
		
	}
	
	
}
