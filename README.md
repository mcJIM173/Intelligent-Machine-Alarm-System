# Intelligent-Machine-Alarm-System
This project was built for a company's Industrial simulation intelligent diagnosis platform.

This system is a machine alarm simulation system developed in collaboration between me and the USCC Lab. It is deployed within a company's industrial simulation diagnostic platform. The system consists of both software and hardware components. The following description focuses only on the software part, and detailed documentation is not publicly available.

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
