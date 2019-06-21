# Distributed Sensor Network
## Consists of 2 servers and sensors(max 6).
Main server is on Raspberry Pi, additional one is PC.(If main server breaks down then another becomes main)
Each sensor is connected to individual ESP8266.

### Usage:
1. Run ```python3 app.py``` in project directory.
2. Connect devices to network using mobile app "SmartConfig"(Android: https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch&hl=en, IOS: https://apps.apple.com/us/app/iot-maker/id1222388182)
3. In app enter login and password to network you want to use.
4. Check server to see sensor measurements.
