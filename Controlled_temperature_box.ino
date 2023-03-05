#include "DHT.h"

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define r1 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define lin 16

#define DHTPIN1 2
#define DHTPIN2 4
#define DHTTYPE DHT11
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

uint32_t tdis;
uint32_t tris;

bool acc = false;
bool failure = false;
#define tdes 50
#define delta 3
#define MAX_TEMP 80
#define MIN_TEMP 0

float T = 50;
float H = 50;

void setup() {


  Wire.begin();
  dht1.begin();
  dht2.begin();


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // display2.begin(SSD1306_SWITCHCAPVCC, 0x68);
  // display2.clearDisplay();
  // display2.setTextSize(2);
  // display2.setTextColor(WHITE);
  // display2.setCursor(0, 0);
  // display2.print(F("Test dispaly 2"));



  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print(F("Initializing"));

  display.display();

  pinMode(r1, OUTPUT);


  digitalWrite(r1, LOW);

  delay(3000);
}

void loop() {

  // luce di sopra
  if (millis() - tris > 100) {
    tris = millis();

    float t1 = dht1.readTemperature();
    float t2 = dht2.readTemperature();
    float h1 = dht1.readHumidity();
    float h2 = dht2.readHumidity();

    float t_mean;

    if (t1 < MIN_TEMP || t1 > MAX_TEMP) {
      if (t2 < MIN_TEMP || t2 > MAX_TEMP) {
        failure = true;
      } else {
        t_mean = t2;
      }
    } else if (t2 < MIN_TEMP || t2 > MAX_TEMP) {
      t_mean = t1;
    } else {
      t_mean = (t1 + t2) / 2;
    }
    T = T * 0.8 + t_mean * 0.2;

    H = H * 0.8 + ((h1 + h2) / 2) * 0.2;


    if (!failure) {

      if (t1 > tdes + 5 * delta || t2 > tdes + 5 * delta) {
        acc = false;
        digitalWrite(r1, LOW);
      } else {

        if (acc == false && T < (tdes - delta)) {
          acc = true;
          digitalWrite(r1, HIGH);
        }

        if (acc == true && T > (tdes + delta)) {
          acc = false;
          digitalWrite(r1, LOW);
        }
      }
    } else {
      digitalWrite(r1, LOW);
    }
  }


  if (millis() - tdis > 200) {

    display.setCursor(0, 0);
    display.clearDisplay();


    if (!failure) {

      display.println("Serra");
      display.print("Temp:");
      display.println(T);
      display.print("Umid:");
      display.println(H);
      display.display();
      tdis = millis();
    } else {
      display.println("Failure");
      display.display();
    }
  }
}
