#include"HorseMasterFunctions.h"
#include <Blynk.h>
#include<BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial


int pinValue[2];
//Blynk-key, wifi and  password
const char auth[] = "Addyourblynkauth";
const char* ssid = "Addyourssid";
const char* netPassword = "Addyourpassword";
BlynkTimer timer;
HorseMaster myMaster;
unsigned long timedEvent = 20000;
unsigned long currentTime = 0;

void setup() {
	//Function for startSequenses here
	Serial.begin(115200);
	myMaster.startUps();
	//Code to connect to Blynk
	Blynk.begin(auth, ssid, netPassword);
	Blynk.notify("Device started");
	timer.setInterval(60000L, pushMessage);
}

void loop() {
	//Checks for blynk-events
	Blynk.run();
	timer.run();
	
	//Transports data between Arduino Uno and MySql every 20 seconds to let each process get enough time to execute.
	if (timedEvent + currentTime >= millis()) {
		myMaster.getDataFromSlave();
		myMaster.sendDataToSql();
		myMaster.getDataFromSql();
		currentTime = millis();
	}
	//Keeps the webserver going
	myMaster.sendDataToWeb();
}

//Sends a message to the Blynk-app
void pushMessage()
{
	Blynk.notify(String("Cloudia is feeling: ") + myMaster.getSafetyLevel());
}


//The Value of V0 controls the blinkTest-Function on the slave-device
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
