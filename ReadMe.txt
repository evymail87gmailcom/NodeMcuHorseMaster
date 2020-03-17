In this project the NodeMcu acts as a master device against the slave, an Arduino uno.
The Node will act as a webserver hosting the official site for the project.
It will also recieve data from the slave and send that to a MySql database witch in turn 
will send data to the javascript of the website.

1. Include neccessary libraries, Wire.h, ESP8266WiFi.h, <MySQL_Connection.h> and <MySQL_Cursor.h>
