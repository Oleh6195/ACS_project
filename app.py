from __future__ import print_function

import random
import socket
import time

import firebase_admin
import gspread
import paho.mqtt.client as mqtt
from firebase_admin import credentials
from flask import Flask, render_template
from flask_socketio import SocketIO
from oauth2client.service_account import ServiceAccountCredentials

UNIQUE_ID = 686
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
cred = credentials.Certificate(
    'sensorsnetwork-c38e4-firebase-adminsdk-cnn2m-f0b4040288.json')

firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://sensorsnetwork-c38e4.firebaseio.com/'
})


def write_id():
    scope = ['https://spreadsheets.google.com/feeds',
             'https://www.googleapis.com/auth/drive']
    creds = ServiceAccountCredentials.from_json_keyfile_name('secret.json',
                                                             scope)
    client = gspread.authorize(creds)
    sheet = client.open('AIOT').sheet1
    hostname = socket.gethostname()
    IPAddr = socket.gethostbyname(hostname)
    row = [str(UNIQUE_ID), str(IPAddr)]
    sheet.insert_row(row)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("/esp8266/gas_val")
    client.subscribe("/esp8266/gas_tag")
    client.subscribe("/esp8266/percentage")


def on_message(client, userdata, message):
    print("Received message '" + str(
        message.payload.decode("utf-8")) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))
    if message.topic == "/esp8266/gas_val":
        print("temperature update")
        socketio.emit('gas_val', {'data': message.payload.decode("utf-8")})
    if message.topic == "/esp8266/gas_tag":
        print("temperature tag update")
        socketio.emit('gas_tag', {'data': message.payload.decode("utf-8")})
    if message.topic == "/esp8266/percentage":
        print("gas update")
        socketio.emit('percentage', {'data': message.payload.decode("utf-8")})
    time.sleep(3)
    socketio.emit('tag_1', {'data': "Hummidity"})
    socketio.emit('val_1', {'data': str(random.randint(1, 100))})
    time.sleep(3)
    socketio.emit('tag_2', {'data': "Pressure"})
    socketio.emit('val_2', {'data': str(random.randint(1, 100))})
    time.sleep(3)
    socketio.emit('tag_3', {'data': "Distance"})
    socketio.emit('val_3', {'data': str(random.randint(1, 100))})
    time.sleep(3)
    socketio.emit('tag_4', {'data': "Speed"})
    socketio.emit('val_4', {'data': str(random.randint(1, 100))})


mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("localhost", 1883, 40)
mqttc.loop_start()
write_id()


@app.route("/")
def main():
    return render_template('main.html', async_mode=socketio.async_mode)


@socketio.on('my event')
def handle_my_custom_event(json):
    print('received json data here: ' + str(json))


if __name__ == "__main__":
    socketio.run(app, host='0.0.0.0', port=8181, debug=True)
