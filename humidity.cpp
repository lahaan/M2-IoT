#include <Arduino.h>
#include <ittiot.h>
#include <Ticker.h>
#include <DHT.h>

#define WIFI_NAME     "C6"
#define WIFI_PASSWORD "Vanaema70"

#define MQTT_SERVER   "193.40.245.72"
#define MQTT_PORT     "1883"
#define MQTT_USER     "M2"
#define MQTT_PASS     "test"

#define MODULE_TOPIC  "ESP40/temphumid"

#define DHTPIN  D3
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Ticker timeTicker;
bool sendDataFlag = false;

void sendData() {
  sendDataFlag = true;
}

void iot_received(String topic, String msg) {
}

void iot_connected() {
  Serial.println("MQTT connected callback");
  // iot.subscribe(MODULE_TOPIC "/set");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting DHT → MQTT");

  iot.setConfig("wname", WIFI_NAME);
  iot.setConfig("wpass", WIFI_PASSWORD);

  iot.setConfig("msrv",  MQTT_SERVER);
  iot.setConfig("mport", MQTT_PORT);
  iot.setConfig("muser", MQTT_USER);
  iot.setConfig("mpass", MQTT_PASS);

  iot.printConfig();  // dump json
  iot.setup();

  dht.begin();
  timeTicker.attach(1.0, sendData);  // every 1 s
}

void loop() {
  iot.handle();

  if (sendDataFlag) {
    sendDataFlag = false;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.printf("Temperature: %.2f C,   Humidity: %.2f %%\n", t, h);

    char buf[16];
    dtostrf(t, 5, 2, buf);
    iot.publishMsg("temp", buf);

    dtostrf(h, 5, 2, buf);
    iot.publishMsg("hum", buf);
  }
}
/*[env:d1_mini]
platform = espressif8266
board = d1_mini
framework = arduino
upload_speed = 921600
upload_port = COM3
monitor_speed = 115200
lib_deps = ITTIoT, DHT sensor library, Adafruit Unified Sensor*/