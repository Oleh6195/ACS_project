import socket
import ssl
from urllib.parse import urlencode
from urllib.request import urlopen, Request

import eventlet
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
from wireless import Wireless

app = Flask(__name__)
app.config['MQTT_BROKER_URL'] = "0.0.0.0"
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_KEEPALIVE'] = 20
eventlet.monkey_patch()
mqtt = Mqtt(app)
socketio = SocketIO(app)
SUBSCIPRIONS = ["/esp8266/gas_val", "/esp8266/gas_tag", "/esp8266/val_2",
                "/esp8266/tag_2", "/esp8266/val_3", "/esp8266/tag_3",
                "/esp8266/val_4", "/esp8266/tag_4", "/esp8266/val_5",
                "/esp8266/tag_5", "/esp8266/val_6", "/esp8266/tag_6"]


def write_id():
    wireless = Wireless()
    print(wireless.current())
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    url = 'https://olehtyzhai.pythonanywhere.com/post'  # Set destination URL here
    post_fields = {'id': wireless.current(),
                   'ip': ip}  # Set POST fields here

    request = Request(url, urlencode(post_fields).encode())
    gcontext = ssl.SSLContext()
    json = urlopen(request, context=gcontext).read().decode()
    print(json)


@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print('on_connect client : {} userdata :{} flags :{} rc:{}'.format(client,
                                                                       userdata,
                                                                       flags,
                                                                       rc))
    for subscription in SUBSCIPRIONS:
        mqtt.subscribe(subscription)


@mqtt.on_subscribe()
def handle_subscribe(client, userdata, mid, granted_qos):
    print(
        'on_subscribe client : {} userdata :{} mid :{} granted_qos:{}'.format(
            client, userdata, mid, granted_qos))


@mqtt.on_message()
def handle_message(client, userdata, message):
    print("Received message '" + str(
        message.payload.decode("utf-8")) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))
    if message.topic in SUBSCIPRIONS:
        print(message.topic + " was updated")
        socketio.emit(str(message.topic).replace("/esp8266/", ""),
                      {'data': message.payload.decode("utf-8")})
        socketio.sleep(0)


@mqtt.on_disconnect()
def handle_disconnect(client, userdata, rc):
    print('on_disconnect client : {} userdata :{} rc :{}'.format(client,
                                                                 userdata, rc))


@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)


@socketio.on('my event')
def handle_my_custom_event(json):
    print('received json data here: ' + str(json))


@app.route("/")
def main():
    return render_template('main.html', async_mode=socketio.async_mode)


if __name__ == '__main__':
    write_id();
    socketio.run(app, host="0.0.0.0", port=8080)
