/**
 * @file sitbit.ino
 * @author RelaxedBamboo
 * @brief
 * User enters 'C' on the serial monitor to calibrate for correct posture.
 * If the user is slouching for >30s, an alert is made
 * If the user is sitting for >60s (IRL this will be like 45-50min), an alert is made
 * If the temp and humidity are too high (as per khush's research), an alert is made
 *
 */

#include "HTTPClient.h"
#include "WiFi.h"
#include "time.h"
#include <ArduinoJson.h>
#include <DHT.h>
#include <WebServer.h>
#include <math.h>

#define trigger 25
#define echo 26
#define BUZZER 19
#define DHT_dpin 4

#define SOUND_SPEED 0.0343
#define OPTI_TEMP 35
#define OPTI_HUMIDITY 70
#define OPTI_HI 85

int threshold_distance = 15;
String input = "";
int slouch_iter = 0;
int sit_iter = 0;

float temp = 0.0f;
float humidity = 0.0f;
float HI = 0.0f;

char *ssid = "The Mehta's [2.4GHz]";
char *pwd = "07112131";
WiFiServer server(80);
String cse_ip = "192.168.0.123";
String cse_port = "8080";
String Server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String cnt = "node1";
String ae = "Ultrasonic_Sensor";
String ae1 = "Temperature_Sensor";
String ae2 = "Humidity_Sensor";
String ae3 = "Heat_Index";
String ae4 = "Alerts";

WiFiClient client;

int writeChannelID = 1579632;
int writeChannelID_temp = 1592176;
int writeChannelID_hum = 1592179;
char writeAPIKey[] = "HNW00GZWT0EB6RVY";
char writeAPIKey_temp[] = "480VAL7GN6Z0R83G";
char writeAPIKey_hum[] = "ONVQSROYXRF7B70B";

void createCI(String &val, int type) {
    HTTPClient http;
    if (type == 0)
        http.begin(Server + ae + "/" + cnt + "/");
    else if (type == 1)
        http.begin(Server + ae1 + "/" + cnt + "/");
    else if (type == 2)
        http.begin(Server + ae2 + "/" + cnt + "/");
    else if (type == 3)
        http.begin(Server + ae3 + "/" + cnt + "/");
    else if (type == 4)
        http.begin(Server + ae4 + "/" + cnt + "/");

    http.addHeader("X-M2M-Origin", "admin:admin");
    http.addHeader("Content-Type", "application/json;ty=4");

    int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

    if (code != 201)
        Serial.println(code);
    else if (code == -1)
        Serial.println("UNABLE TO CONNECT TO THE SERVER");

    http.end();
}

void tone(byte pin, int freq) {
    ledcAttachPin(pin, 0);
    ledcWriteTone(0, freq);
    delay(1000);
    ledcDetachPin(pin);
}

float ping() {
    digitalWrite(trigger, HIGH);
    delay(10);
    digitalWrite(trigger, LOW);

    float duration = pulseIn(echo, HIGH);
    float distance = duration * SOUND_SPEED / 2;
    return distance;
}

DHT dht(DHT_dpin, DHT11);

void setup() {
    dht.begin();
    Serial.begin(115200);
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    pinMode(BUZZER, OUTPUT);
    ledcSetup(0, 5000, 8);

    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("HTTP server started");

    Serial.println("Sit perfectly straight.");
    delay(250);
    Serial.print("Calibrating");

    float reading = ping();

    for (int i = 1; i < 10; i++) {
        delay(500);
        reading = min(ping(), reading);
        Serial.print(".");
    }
    Serial.println();
    threshold_distance = reading;
    if (threshold_distance < 0 || threshold_distance > 100)
        threshold_distance = 15;

    String val_thresh = String(threshold_distance);

    Serial.println("Calibration complete.");
    Serial.print("Calibrated Value: ");
    Serial.println(threshold_distance);
}

void loop() {
    input = "";

    float distance = ping();
    Serial.print("Distance in cm: ");
    Serial.println(distance);
    String val = String(distance, 1);

    createCI(val, 0);
    humidity = dht.readHumidity();
    temp = dht.readTemperature();
    bool f = false;
    if (isnan(humidity) || isnan(temp) || temp < -40 || temp > 125) {
        Serial.println("Failed to get temperature and humidity!");
        f = true;
    } else {
        Serial.print("Humidity: ");
        Serial.println(humidity);
        Serial.print("Temperature: ");
        Serial.println(temp);

        String humidity_string = String(humidity, 0);
        createCI(humidity_string, 2);
        String temperature_string = String(temp, 0);
        createCI(temperature_string, 1);

        float tempF = (temp * 9) / 5 + 32;
        HI = -42.379 + (2.04901523) * tempF + (10.14333127) * humidity + (-0.22475541) * tempF * humidity + (-6.83783 * 0.001) * pow(tempF, 2) + (-5.481717 * 0.01) * pow(humidity, 2) + (1.22874 * 0.001) * (pow(tempF, 2) * humidity) + (8.5282 * 0.0001) * (tempF * pow(humidity, 2)) + (-1.99 * 0.000001) * (pow(tempF, 2) * pow(humidity, 2));
        Serial.print("Heat Index: ");
        Serial.println(HI);
        String val_HI = String(HI, 5);
        createCI(val_HI, 3);
    }

    if (distance > threshold_distance) {
        slouch_iter++;
    }
    if (distance < 50) {
        sit_iter++;
    } else {
        sit_iter = 0;
        slouch_iter = 0;
    }

    String alert = "";

    if (slouch_iter > 3) {
        tone(BUZZER, 1000);
        slouch_iter = 0;
        Serial.println("Slouching for too long.");
        alert = "0.";
        createCI(alert, 4);
    } else if (sit_iter > 3) {
        tone(BUZZER, 3000);
        sit_iter = 0;
        Serial.println("Sitting for too long.");
        alert = "1";
        createCI(alert, 4);
    } else if (HI > OPTI_HI && !f) {
        tone(BUZZER, 2000);
        Serial.println("Temperature and/or humidity too high.");
        alert = "2";
        createCI(alert, 4);
    } else {
        if (distance > 50)
            alert = "3"; // "Currently Not Seated.";
        else if (distance > threshold_distance)
            alert = "4"; // "Currently Slouching.";
        else
            alert = "5"; // "You're doing great! Maintain that excellent posture!";
        createCI(alert, 4);
    }

    delay(10000);
}
