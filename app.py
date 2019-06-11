from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
import eventlet
from oauth2client.service_account import ServiceAccountCredentials

UNIQUE_ID = 686

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
        socketio.emit(str(message.topic).replace("/esp8266/", ""), {'data': message.payload.decode("utf-8")})
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
    socketio.run(app, host="0.0.0.0", port=8080)

