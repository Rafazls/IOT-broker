import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WriteOptions
import datetime
import logging

# .env example constants: 
# Constants for InfluxDB
INFLUXDB_URL = ''
INFLUXDB_TOKEN = 'my-secret-token'
INFLUXDB_ORG = 'globalsolution'
INFLUXDB_BUCKET = 'iot'

# Constants for MQTT
MQTT_BROKER = '192.168.15.6' #Use 'localhost' or IP address of your MQTT broker
MQTT_PORT = 1883
MQTT_TOPICS = ["/energy", "/vazao", "/price"]

def setup_logging():
    """Configure logging settings."""
    logging.basicConfig(level=logging.INFO)

def create_influxdb_client():
    """Initialize and return the InfluxDB client."""
    try:
        client = InfluxDBClient(url=INFLUXDB_URL, token=INFLUXDB_TOKEN, org=INFLUXDB_ORG)
        logging.info("Connected to InfluxDB")
        return client.write_api(write_options=WriteOptions(batch_size=1))
    except Exception as e:
        logging.error(f"Error connecting to InfluxDB: {e}")
        raise

def persists(msg):
    """Persist data to InfluxDB."""
    try:
        current_time = datetime.datetime.utcnow().isoformat()
        write_points(msg.payload, current_time)
    except Exception as e:
        logging.error(f"Error writing to InfluxDB: {e}")

def write_points(payload, current_time):
    """Write points to InfluxDB."""
    value = int(payload)
    points = [
        Point("energy").field("value", value).time(current_time),
        Point("vazao").field("value", value).time(current_time),
        Point("price").field("value", value).time(current_time)
    ]
    for point in points:
        write_api.write(bucket=INFLUXDB_BUCKET, record=point)

def on_connect(client, userdata, flags, rc):
    """Callback function for MQTT connect."""
    if rc == 0:
        logging.info("Connected to MQTT broker")
        for topic in MQTT_TOPICS:
            client.subscribe(topic)
    else:
        logging.error(f"Failed to connect to MQTT broker, return code {rc}")

def on_message(client, userdata, msg):
    """Callback function for MQTT message reception."""
    logging.info(f"Received message: {msg.payload} on topic {msg.topic}")
    persists(msg)

def create_mqtt_client():
    """Initialize and return the MQTT client."""
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    return client

def connect_mqtt_client(client):
    """Connect to the MQTT broker."""
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        logging.info("Connecting to MQTT broker")
    except Exception as e:
        logging.error(f"Error connecting to MQTT broker: {e}")
        raise

if __name__ == "__main__":
    setup_logging()
    write_api = create_influxdb_client()
    mqtt_client = create_mqtt_client()
    connect_mqtt_client(mqtt_client)
    mqtt_client.loop_forever()
