import paho.mqtt.client as mqtt
import statsd
import time


#send data to statsd

def send_data_to_statsd(mqtt_data):
    metrics=['air_temperature','wort_temperature','hot','cold','bubbles']
    statsd.Connection.set_defaults(host='statsd.domain.tld', port=8125)
    
    c=0
    gauge = statsd.Gauge('Homebrew.batch_dev')
    for data in mqtt_data.split(','):
        gauge.send(metrics[c], float(data))
        c+=1
    
    

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("garagarduino")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    
    filename = 'data.csv'
    data=open(filename, 'a')
    data.write("%s,%s" % (time.strftime("%Y-%m-%d %H:%M:%S"),msg.payload))
    data.write("\n")
    data.close()
    

    send_data_to_statsd(msg.payload)




client = mqtt.Client()
client.username_pw_set("garagarduino", "changeme")

client.on_connect = on_connect
client.on_message = on_message

client.connect("brew.domain.tld", 1883, 60)




# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()