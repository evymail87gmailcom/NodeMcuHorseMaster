
// Load Wi-Fi library


#include <Blynk.h>
#include <MySQL_Connection.h>
#include<MySQL_Cursor.h>
#include <ESP8266WiFi.h>
#include<BlynkSimpleEsp8266.h>
#include <Wire.h>
#define BLYNK_PRINT Serial

int pinValue[2];
int pinValue2;
char auth[] = "4hPSchreMGmafdTADpF1IcNNndo5Byv6";
BlynkTimer timer;
// Replace with your network credentials
//const char* ssid = "STI Student";
//const char* netPassword = "STI1924stu";

const char* ssid = "#Telia-5BB008";
const char* netPassword = "A()Zc14yN1#uZw8)";


// Set web server port number to 80
WiFiServer server(80);
WiFiClient client;
MySQL_Connection conn((Client*)&client);
WiFiClient mySqlClient;
MySQL_Connection mySqlConnection((Client*)&mySqlClient);
//IPAddress serverAdress(172, 16, 116, 88);//IP that MySql is connected to. Not the Arduino Server.
IPAddress serverAdress(192,168,1,101);//IP that MySql is connected to. Not the Arduino Server.
//MySql user and password
char user[] = "kurs1234";
char password[] = "kurs1234";

// Variable to store the HTTP request
String header;
static int tempo = 0;
int horsePower,sound,distance,steps,gas;

int getHorsePower,getSteps,getDistance,getGas,getSound,getDateTime;
void setup() {


	//function for startSequenses here
	Serial.begin(115200);

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

	//Code to connect to Blynk
	Blynk.begin(auth, ssid, netPassword);
	Blynk.notify("Device started");
	timer.setInterval(60000L, pushMessage);
	//End of startSequenses

	
}

void loop() {

	Blynk.run();

	timer.run();
	
	/*
	//Function sendDataToSlave
	//Starts the transmission, checks commands from the Blynk app and executes the functions in each command and then ends transmission
	for (int i = 0; i < 2; i++)
	{

		Wire.beginTransmission(8); //device adress 8
		

		Wire.write(pinValue[i]);
		Serial.print("pinvalue: " + i);

		Wire.endTransmission(); //Stop transmitting
		//End of sendDataToSlave
		//Start for getDataFromSlave
		//Retrieves data from Uno, needs to be put in a function
	}
	Wire.beginTransmission(8); //device adress 8
	Blynk.run();
	
	timer.run();
	//Wire.write(pinValue), pinValue2);
	Wire.endTransmission(); //Stop transmitting
	
	*/
	//End of sendDataToSlave
	//Start for getDataFromSlave
	//Retrieves data from Uno, needs to be put in a function
	Wire.requestFrom(8, 13); //request and read data of size 13 from slave(8)
	if (Wire.available()) {
		distance = Wire.read();
		steps = Wire.read();
		sound = Wire.read();
		horsePower = Wire.read();
		gas = Wire.read();
		Serial.print("Distance: ");
		Serial.println(distance);
		Serial.print("Steps: ");
		Serial.println(steps);
		Serial.print("Sound: ");
		Serial.println(sound);
		Serial.print("HorsePower: ");
		Serial.println(horsePower);
		Serial.print("Gas: ");
		Serial.println(gas);
	}
	Serial.println();
	delay(1000);
	//End of getDataFromSlave

	
	//Break into function sendDataToSql 

	Serial.println(F("Recording data."));
	MySQL_Cursor* cur_mem = new MySQL_Cursor(&mySqlConnection);  // Instans av en MySQL Cursor (där data transporteras)

	// INSERT sensordata
	String query = "INSERT INTO `findmyhorse`.`horsedata` (`DateTime`,`distance`,`steps`,`sound`,`horsepower`,`gas`) VALUES (now()," + String(distance)+","+String(steps)+","+String(sound)+","+String(horsePower)+","+String(gas) + ")";
	// Mall: INSERT INTO `temphum2`.`temphum` (`DateTime`, `Temp`, `Hum`) VALUES (now(), 22.80, 21.30)

	Serial.println(query);
	const char* q = query.c_str();                          // Konverterar en sträng till en char-array
	cur_mem->execute(q);                              // Utför vår query, och stoppar resultatet i cur_mem, en INSERT ger ingen data tillbaka...

	//delete cur_mem;
	delay(10000); //10 sec
	//End of sendDataToSql
	
	//Function to getDataFromSql
	row_values* row = NULL;
	 cur_mem->execute ("SELECT * FROM findmyhorse.horsedata WHERE Id = (SELECT LAST_INSERT_ID());");
	 column_names* columns = cur_mem->get_columns();
	 do {
		 row = cur_mem->get_next_row();
		 if (row != NULL) {
			 Serial.print(F("Data from sql: "));

			 getDateTime = atoi(row->values[1]);
			 Serial.print("DateTime: ");
			 Serial.println(row->values[1]);

			 getDistance = atoi(row->values[2]);
			 Serial.print("Distance: ");
			 Serial.println(row->values[2]);
			 
			 getSteps = atoi(row->values[3]);
			 Serial.print("Steps: ");
			 Serial.println(row->values[3]);

			 getSound = atoi(row->values[4]);
			 Serial.print("Sound: ");
			 Serial.println(row->values[4]);

			 getHorsePower = atoi(row->values[5]);
			 Serial.print("HorsePower: ");
			 Serial.println(row->values[5]);

			 getGas = atoi(row->values[6]);
			 Serial.print("Gas: ");
			 Serial.println(row->values[6]);
			
		 }

	 } while (row != NULL);
	
	
	delete cur_mem;
	//End of getDataFromSql

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
						client.print(getDateTime);
						client.print(",");
						client.print(getDistance);
						client.print(",");
						client.print(getSteps);
						client.print(",");
						client.print(getSound);
						client.print(",");
						client.print(getHorsePower);
						client.print(",");
						client.print(getGas);
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

void pushMessage()
{
	Blynk.notify(String("Cloudias tempo is: ") + getHorsePower);
}

int pinvalue1;
//The Value of V1 should control the blinkTest-Function on the slave-device
BLYNK_WRITE(V0) {
	pinValue[0] = param.asInt();
	
	Wire.beginTransmission(8);
	Wire.write("V0");
	Wire.write(pinValue[0]);//sends an int
	Wire.endTransmission();
	Serial.print("VO: ");
	Serial.println(pinValue[0]);
}
//The Value of V1 should then be sent and control the walk-Function on the slave-device
BLYNK_WRITE(V1) {
	pinValue[1] = param.asInt();
	Wire.beginTransmission(8);
	Wire.write("V1");
	Wire.write(pinValue[1]);//sends an int
	Wire.endTransmission();
	Serial.print("V1: ");
	Serial.println(pinValue[1]);
}