# Intelligent-Machine-Alarm-System
This system was developed by USCC Lab. It is deployed within a company's industrial simulation intelligent diagnosis platform. The system consists of both software and hardware components. The following description focuses only on the software part, and detailed documentation is private.le.

## Main.ino

This file is burned onto the Arduino Mega inside the chassis. It is responsible for receiving data from various sensors and functional modules, controlling the corresponding actions of each functional module, and transmitting the current status data of each functional module to the NodeMCU (ESP8266).

## MAS_DashBoard_nodejs_add.ino

This file is burned onto the NodeMCU (ESP8266) and is responsible for the following functions:

- Receiving sensor data from Arduino Mega
- Wifi code: Connecting to Wifi
- HTML code: Web page structure
- Broadcasting the IP address, allowing users to observe the current status of each functional module in real-time through the web
- Post to node.js: Posting data to node.js and storing it in a MySQL database through node.js

## Dash

Dash utilizes Plotly for visualization, enabling users to quickly query the historical status of the equipment.

Note: The above descriptions are provided only for the software part of the system. Detailed documentation is not publicly available.
