#include"HorseMasterFunctions.h"
#include <Blynk.h>
#include<BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial

int pinValue[2];
//Blynk password
const char auth[] = "4hPSchreMGmafdTADpF1IcNNndo5Byv6";
const char* ssid = "#Telia-5BB008";
const char* netPassword = "A()Zc14yN1#uZw8)";
BlynkTimer timer;
HorseMaster myMaster;
unsigned long timedEvent1 = 40000;
unsigned long currentTime = 0;
unsigned long timedEvent2 = 1000;
unsigned long currentTime2 = 0;

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
	
	Blynk.run();
	timer.run();
	
	
	if (timedEvent1 + currentTime >= millis()) {
		myMaster.getDataFromSlave();
		myMaster.sendDataToSql();
		myMaster.getDataFromSql();
		currentTime = millis();
	}

	myMaster.sendDataToWeb();
}

void pushMessage()
{
	Blynk.notify(String("Cloudias tempo is: ") + myMaster.getHorsePower());
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