# Distributed Sensor Network
## Consists of 2 servers and sensors(max 6).
Main server is on Raspberry Pi, additional one is localhost.(If main server breaks down then another becomes main)
Each sensor is connected to individual ESP8266.

### Usage:
1. Run ```python3 app.py``` in project directory.
2. Connect devices to network using mobile app "SmartConfig"(Android: https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch&hl=en, IOS: )
3. Check servers to see sensor measurements.
