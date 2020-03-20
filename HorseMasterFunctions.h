#pragma once


#include <Arduino.h>
#include <Wire.h>
#include <MySQL_Connection.h>
#include<MySQL_Cursor.h>
#include <ESP8266WiFi.h>

class HorseMaster
{
protected:
	String header;
	int horsePower, sound, distance, steps, gas, dateTime;
	int getDistance();
	int getSteps();
	int getSound();
	
	int getGas();
public:

	HorseMaster() {};
	void getDataFromSlave();
	void sendDataToSql();
	void getDataFromSql();
	void sendDataToWeb();
	int getHorsePower();
	void startUps();
	void userInfo();
	~HorseMaster() {};




	


};
