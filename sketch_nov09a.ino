/**
 * @file proj.ino
 * @author RelaxedBamboo
 * @brief
 * User enters 'C' on the serial monitor to calibrate for correct posture.
 * If the user is slouching for >30s, an alert is made
 * If the user is sitting for >60s (IRL this will be like 45-50min), an alert is made
 * If the temp and humidity are too high (as per khush's research), an alert is made
 *
 * @version 1
 * @date 2021-Nov
 *
 */

// pin definitions
#define trigger 25
#define echo 26
#define BUZZER 19
#define DHT_dpin 4

// other defs
#define DHTTYPE DHT11
#define SOUSPD 0.0343
#define OPTI_TEMP 35     // hi khush
#define OPTI_HUMIDITY 70 // hi khush

// libraries and stuff
#include <DHT.h>
// #include "BluetoothSerial.h"
#include <WiFi.h>
#include <ThingSpeak.h>
#include <WebServer.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// global variables
int straight_thresh = 10;
String input = "";
int slouch_iter = 0;
int sit_iter = 0;
int temp = 0;
int hum = 0;
// BluetoothSerial SerialBT;

char *ssid = "Sunflower";
char *pwd = "mumbai2020**";
WiFiServer server(80);
String cse_ip = "127.0.0.1"; // YOUR IP from ipconfig/ifconfig
String cse_port = "8080";
String Server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "Ultrasonic_Sensor";
String cnt = "node1";
String ae1 = "Temperature_Sensor";
String ae2 = "Humidity_Sensor";
String ae3 = "Heat_Index";

WiFiClient client;

int writeChannelID = 1579632;
int writeChannelID_temp = 1592176;
int writeChannelID_hum = 1592179;
char writeAPIKey[] = "HNW00GZWT0EB6RVY";
char writeAPIKey_temp[] = "480VAL7GN6Z0R83G";
char writeAPIKey_hum[] = "ONVQSROYXRF7B70B";

void createCI(String &val, int type)
{
  HTTPClient http;
  if (type == 0)
    http.begin(Server + ae + "/" + cnt + "/");
  else if (type == 1)
    http.begin(Server + ae1 + "/" + cnt + "/");
  else if(type == 2)
    http.begin(Server + ae2 + "/" + cnt + "/");
  else
    http.begin(Server + ae3 + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

void tone(byte pin, int freq)
{
  ledcAttachPin(pin, 0);
  ledcWriteTone(0, freq);
  delay(1000);
  ledcDetachPin(pin);
}

float ping()
{
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);

  float duration = pulseIn(echo, HIGH);
  float distance = duration * SOUSPD / 2;
  return distance;
}

DHT dht(DHT_dpin, DHTTYPE);

void setup()
{
  dht.begin();
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(BUZZER, OUTPUT);
  ledcSetup(0, 5000, 8);
  // SerialBT.begin("ESP32Ultra");

  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // ThingSpeak.begin(client);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Sit perfectly straight.");
  delay(250);
  Serial.print("Calibrating");
  float reading = ping();
  for (int i = 1; i < 10; i++)
  {
    delay(500);
    reading = min(ping(), reading);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Calibration complete.");
  Serial.print("Distance(cm) upto which posture is considered 'correct' is : ");
  Serial.println(reading);
  straight_thresh = reading;
}

void loop()
{
  // read the serial input
  // if (SerialBT.available())
  // {
  //   input = SerialBT.readString();
  //   input.trim();
  //   while (SerialBT.available() > 0)
  //   {
  //     char t = SerialBT.read();
  //   }
  // }

  // if the user enters 'C' on the serial monitor, calibrate the sensor
  // if (input.equals("C") || input.equals("c"))
  // {
  // }
  input = "";
  // correct weird readings
  if (straight_thresh < 0 || straight_thresh > 100)
    straight_thresh = 10;

  // fire sensors and get readings
  float distance = ping();
  Serial.print("Distance in cm: ");
  Serial.println(distance);
  String val = String(distance, 1);
  // int x = ThingSpeak.writeField(writeChannelID, 1, distance, writeAPIKey);
  Serial.println(x);
  createCI(val, 0);
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  bool f = false;
  if (isnan(hum) || isnan(temp) || temp < -40 || temp > 125)
  {
    Serial.println("Failed to get temperature and humidity");
    f = true;
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.println(hum);
    Serial.print("Temperature: ");
    Serial.println(temp);
    String val_humid = String(hum, 1);
    createCI(val_humid, 2);
    String val_temp = String(temp, 2);
    createCI(val_temp, 1);
    double HI = -42.379 + (-2.04901523)*temp + (-10.14333127)*hum + (-0.22475541)*temp*hum + (-6.83783 x 0.001)*pow(temp,2) + (-5.481717 * 0.01)*pow(hum, 2) + (-1.22874 * 0.001)(pow(temp,2)*hum) + (8.5282 * 0.0001)(temp*pow(hum,2)) + (-1.99 * 0.000001)*(pow(temp,2)*pow(hum,2));
    String val_HI = String(HI, 5);
    createCI(HI, 3);
    // x = ThingSpeak.writeField(writeChannelID_temp, 1, distance, writeAPIKey_temp);
    // Serial.println("humidity-status: ");
    // Serial.println(x);
    // x = ThingSpeak.writeField(writeChannelID_hum, 1, distance, writeAPIKey_hum);
    // Serial.println("temp-status: ");
    // Serial.println(x);
  }

  // update tracking variables
  if (distance > straight_thresh)
  {
    slouch_iter++;
  }
  if (distance < 50)
  {
    sit_iter++;
  }
  else
  {
    sit_iter = 0;
    slouch_iter = 0;
  }

  // alerts and reset tracking variables
  if (slouch_iter > 3)
  {
    tone(BUZZER, 1000);
    Serial.println("Slouching for too long.");
    slouch_iter = 0;
  }
  if (sit_iter > 3)
  {
    tone(BUZZER, 3000);
    Serial.println("Sitting for too long.");
    sit_iter = 0;
  }
  if ((temp > OPTI_TEMP || hum > OPTI_HUMIDITY) && !f)
  {
    tone(BUZZER, 2000);
    SerialBT.println("Temperature and/or humidity too high.");
  }

  // 10 second delay
  delay(10000);
}
