#include "HorseMasterFunctions.h"
WiFiServer server(80);
WiFiClient client;
MySQL_Connection conn((Client*)&client);
WiFiClient mySqlClient;
MySQL_Connection mySqlConnection((Client*)&mySqlClient);

void HorseMaster::getDataFromSlave() {
	int getValue[] = { 0,0,0,0,0 };
	int counter = 0;

	//request and read data of size 6 from slave(8)
	Wire.requestFrom(8, 6); 
		
	//As long as there are bytes to read, the loop will run and read the the result
	while(Wire.available()) {
		Serial.print("Read from wire: ");
		//Puts the result in an array
		getValue[counter] = Wire.read();

		Serial.print(getValue[counter]);
		Serial.print(" ");
		counter++;
		delay(500);
	}
	Serial.println();

	//In this case we know the order in wich the information is transmitted. So we know that on place zero i the array we will have the value of distance and so on.
	for (int i = 0; i <= 5; i++)
	{	
			switch (i)
			{
			case 0: {
				distance = getValue[i];
				Serial.print("Distance: ");
				Serial.println(distance);
				break;
			}
			case 1: {
				steps = getValue[i];
				Serial.print("Steps: ");
				Serial.println(steps);
				break;
			}
			case 2: {
				sound = getValue[i];
				Serial.print("Sound: ");
				Serial.println(sound);
				break;
			}
			case 3: {
				horsePower = getValue[i];
				Serial.print("HorsePower: ");
				Serial.println(horsePower);
				break;
			}
			case 4: {
				gas = getValue[i];
				Serial.print("Gas: ");
				Serial.println(gas);
				break;
			}
			default:
				break;
			}
			
		}
	
};
int HorseMaster::getDistance() {
	return distance;
}
int HorseMaster::getSteps() {
	return steps;
}
int HorseMaster::getSound() {
	return sound;
}
int HorseMaster::getHorsePower() {
	return horsePower;
}
int HorseMaster::getGas() {
	return gas;
}
void HorseMaster::sendDataToSql() {

	Serial.println(F("Recording data."));
	//MySql instans where data is transported
	MySQL_Cursor* cur_mem = new MySQL_Cursor(&mySqlConnection);  

	// INSERT sensordata
	String query = "INSERT INTO `findmyhorse`.`horsedata` (`DateTime`,`distance`,`steps`,`sound`,`horsepower`,`gas`) VALUES (now()," + String(getDistance()) + "," + String(getSteps()) + "," + String(getSound()) + "," + String(getHorsePower()) + "," + String(getGas()) + ")";
	
	Serial.println(query);
	// Converts a string to a char-array
	const char* q = query.c_str();         
	//Executes the query 
	cur_mem->execute(q);                            

	delete cur_mem;
	
}
void HorseMaster::getDataFromSql() {
	//Function to getDataFromSql
	MySQL_Cursor* cur_mem = new MySQL_Cursor(&mySqlConnection);  // Instans av en MySQL Cursor (där data transporteras)
	row_values* row = NULL;
	cur_mem->execute("SELECT * FROM findmyhorse.horsedata WHERE Id = (SELECT LAST_INSERT_ID());");
	column_names* columns = cur_mem->get_columns();
	do {
		row = cur_mem->get_next_row();
		if (row != NULL) {
			Serial.print(F("Data from sql: "));
			//Divides the incoming string into smaller parts 
			timeNow = (row->values[1]);
			year = timeNow.substring(0, 4);
			month = timeNow.substring(5, 7);
			day = timeNow.substring(8, 10);
			hour = timeNow.substring(11, 13);
			minutes = timeNow.substring(14, 16);
			
			//To check in the Serialwindow if the format is as we want it to
			Serial.print(year.toInt());
			Serial.print("-");
			Serial.print(month.toInt());
			Serial.print("-");
			Serial.print(day.toInt());
			Serial.print(" ");
			Serial.print(hour.toInt());
			Serial.print(":");
			Serial.println(minutes.toInt());
			//Checks if what we get from SQL corresponds with the conversion made in the previous step.
			Serial.print("DateTime: ");
			Serial.println(row->values[1]);
			
			distance = atoi(row->values[2]);
			Serial.print("Distance: ");
			Serial.println(row->values[2]);

			steps = atoi(row->values[3]);
			Serial.print("Steps: ");
			Serial.println(row->values[3]);

			sound = atoi(row->values[4]);
			Serial.print("Sound: ");
			Serial.println(row->values[4]);

			horsePower = atoi(row->values[5]);
			Serial.print("HorsePower: ");
			Serial.println(row->values[5]);

			gas = atoi(row->values[6]);
			Serial.print("Gas: ");
			Serial.println(row->values[6]);
			
			//To get another dataset determined by physical sensor-data
			if (distance <= 20) {
				if (gas >= 300) {
					safetyLevel = "Danger";
				}
				else safetyLevel = "Stuck";
			};
			if (distance <= 40 && distance> 20) {
				if (gas >= 300) {
					safetyLevel = "Danger";
				}
				else safetyLevel = "OK";
			}
			if (distance > 40) {
				if (gas >= 300) {
					safetyLevel = "Danger";
				}
				else safetyLevel = "Freedom";
			}

		}

	} while (row != NULL);


	delete cur_mem;
	
}
String HorseMaster::getSafetyLevel() {
	return safetyLevel;
}
void HorseMaster::sendDataToWeb() {
	
	// Listen for incoming clients
	client = server.available();  

	if (client) {

		// If a new client connects,print a message out in the serial port
		Serial.println("New Client.");          
		// make a String to hold incoming data from the client
		String currentLine = "";                
		// loop while the client's connected
		while (client.connected()) { 
			
			// if there's bytes to read from the client, read a byte, then print it out the serial monitor.		            
			if (client.available()) {      
				char c = client.read();
				Serial.write(c);                    
				header += c;
				if (c == '\n') {                    
				  // if the current line is blank, you got two newline characters in a row.
				 
				  // that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();

						// Display the HTML web page
						client.println("<!DOCTYPE html><html lang=\"en\">");
						client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<title>FindMyHorseRobotCloudia</title>");

						//Link to external css
						client.println("<link rel=\"stylesheet\" href=\"https://marialoue.github.io/horse-robot-web/main.css\"></head>");

						// Link to external javascript
						client.print("<body><script src=\"https://marialoue.github.io/horse-robot-web/scripts/index-script.js\">");
						client.println("</script>");
						//Script to insert date and time convertet from mySql and display it on the webpage
						client.println("<script>");
						client.print("getDateTime(");
						client.print(year.toInt());
						client.print(",");
						client.print(month.toInt());
						client.print(",");
						client.print(day.toInt());
						client.print(",");
						client.print(hour.toInt());
						client.print(",");
						client.print(minutes.toInt());
						client.println(");");
						//To insert data from mySql and display it on the webpage
						client.print("getDataFromNode(");
						client.print(5);
						client.print(",");
						client.print(distance);
						client.print(",");
						client.print(steps);
						client.print(",");
						client.print(sound);
						client.print(",");
						client.print(horsePower);
						client.print(",");
						client.print(gas);
						client.println(");");
						client.print("</script></body></html>");

						// The HTTP response ends with another blank line
						client.println();
						// Break out of the while loop
						break;
					}//End of webResponse
					else { // if you got a newline, then clear currentLine
						currentLine = "";
					}
				}
				else if (c != '\r') {  // if you got anything else but a carriage return character,
					currentLine += c;      // add it to the end of the currentLine
				}
		
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		Serial.println("Client disconnected.");
		Serial.println("");

	}
}
void HorseMaster::startUps() {
// Replace with your network credentials
//STI 
//const char* ssid = "STI Student";
//const char* netPassword = "STI1924stu";
//IPAddress serverAdress(172,16,116,90);//IP that MySql is connected to. Not the Arduino Server.

//Home 
	const char* ssid = "wifi";
	const char* netPassword = "password";
	IPAddress serverAdress(insertyouriphere);//IP that MySql is connected to. Not the Arduino Server.

	//MySql user and password
	char user[] = "insertuser";
	char password[] = "insertpassword";
	
	// Connect to Wi-Fi network with SSID and password
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, netPassword);
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	// Print local IP address and start web server for the Node
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();

	//Connecting to Mysqlserver
	Serial.println("Connecting to database");
	//Waiting for connection to MySql
	while (mySqlConnection.connect(serverAdress, 3306, user, password) != true)
	{
		delay(200);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("Connected to SQL Server!");

	//Serial for debug on Master device
	Serial1.begin(9600);

	//Joins i2c bus with SDA=D1 and SCL=D2 of the Node
	Wire.begin(D1, D2);

}

