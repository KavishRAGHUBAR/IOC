# Sorbonne University (M1 SESI)
# Binome : Kavish RAGHUBAR et Weiyi GONG
# Date : 21 Mai 2023
# Enseignant : Franck Wajsburt

import paho.mqtt.client as mqtt

# Callback function to handle incoming MQTT messages and write the luminosity value to a text file
def on_message(client, userdata, message):
    if message.topic == "topic_lum":
        luminosity = message.payload.decode()
        with open('luminosity.txt', 'w') as file:
            file.write(luminosity)

# Create an MQTT client instance
client = mqtt.Client()

# Set the username and password for the MQTT broker
broker_username = "gong"
broker_password = "weiyigong"
client.username_pw_set(broker_username, broker_password)

# Set the callback function
client.on_message = on_message

# Connect to the MQTT broker and subscribe to the desired topic
broker_address = "172.20.10.8"
broker_port = 1883
topic = "topic_lum"

client.connect(broker_address, broker_port)
client.subscribe(topic)

# Run the MQTT client in a loop to receive messages
client.loop_forever()
