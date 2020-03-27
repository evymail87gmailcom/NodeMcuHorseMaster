#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <MySQL_Connection.h>
#include<MySQL_Cursor.h>
#include <ESP8266WiFi.h>

class HorseMaster
{
private:
	String header;
	int horsePower, sound, distance, steps, gas;
	String timeNow, date, year, month, day, hour, minutes, test, safetyLevel;
	int getDistance();
	int getSteps();
	int getSound();
	int getGas();
	int getHorsePower();
public:

	HorseMaster() {};
	String getSafetyLevel();
	void getDataFromSlave();
	void sendDataToSql();
	void getDataFromSql();
	void sendDataToWeb();
	void startUps();
	~HorseMaster() {};

};
