#include "HorseMasterFunctions.h"
WiFiServer server(80);
WiFiClient client;
MySQL_Connection conn((Client*)&client);
WiFiClient mySqlClient;
MySQL_Connection mySqlConnection((Client*)&mySqlClient);

void HorseMaster::getDataFromSlave() {
	int getValue[5];
	int counter = 0;
	Wire.requestFrom(8, 6); //request and read data of size 13 from slave(8)
		
	while(Wire.available()) {
		
		getValue[counter] = Wire.read();
		Serial.print("Read from wire: ");
		Serial.print(getValue[counter]);
		Serial.print(" ");
		counter++;
	}
	Serial.println();
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
	MySQL_Cursor* cur_mem = new MySQL_Cursor(&mySqlConnection);  // Instans av en MySQL Cursor (där data transporteras)

	// INSERT sensordata
	String query = "INSERT INTO `findmyhorse`.`horsedata` (`DateTime`,`distance`,`steps`,`sound`,`horsepower`,`gas`) VALUES (now()," + String(getDistance()) + "," + String(getSteps()) + "," + String(getSound()) + "," + String(getHorsePower()) + "," + String(getGas()) + ")";
	// Mall: INSERT INTO `temphum2`.`temphum` (`DateTime`, `Temp`, `Hum`) VALUES (now(), 22.80, 21.30)

	Serial.println(query);
	const char* q = query.c_str();                          // Konverterar en sträng till en char-array
	cur_mem->execute(q);                              // Utför vår query, och stoppar resultatet i cur_mem, en INSERT ger ingen data tillbaka...

	delete cur_mem;
	
	//End of sendDataToSql

	
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

			dateTime = atoi(row->values[1]);
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

		}

	} while (row != NULL);


	delete cur_mem;
	//End of getDataFromSql
}
void HorseMaster::sendDataToWeb() {

	client = server.available();   // Listen for incoming clients

	if (client) {

		// If a new client connects,
		Serial.println("New Client.");          // print a message out in the serial port
		String currentLine = "";                // make a String to hold incoming data from the client

		while (client.connected()) { // loop while the client's connected

			if (client.available()) {             // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);                    // print it out the serial monitor
				header += c;
				if (c == '\n') {                    // if the byte is a newline character
				  // if the current line is blank, you got two newline characters in a row.
				  // that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						//Break this into function webResponse
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
						client.println("<link rel=\"stylesheet\" href=\"https://evymail87gmailcom.github.io/FindMyHorseWeb/main.css\"></head>");

						// Link to external javascript
						client.print("<body><script src=\"https://evymail87gmailcom.github.io/FindMyHorseWeb/scripts/index-script.js\">");
						client.println("</script>");
						client.println("<script>");
						client.print("getDataFromNode(");
						client.print(dateTime);
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
				//deletes the pointer 

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
//IPAddress serverAdress(172, 16, 116, 88);//IP that MySql is connected to. Not the Arduino Server.
//MyHome 
	const char* ssid = "#Telia-5BB008";
	const char* netPassword = "A()Zc14yN1#uZw8)";
	IPAddress serverAdress(192, 168, 1, 101);//IP that MySql is connected to. Not the Arduino Server.
	//Mickes place
	//const char* ssid = "Bitoki";
	//const char* netPassword = "aabbccddee";
	//IPAddress serverAdress(192,168,0,146);//IP that MySql is connected to. Not the Arduino Server.
	//MySql user and password
	char user[] = "kurs1234";
	char password[] = "kurs1234";
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
	//Join i2c bus with SDA=D1 and SCL=D2 of the Node
	Wire.begin(D1, D2);

}
void HorseMaster::userInfo() {

}