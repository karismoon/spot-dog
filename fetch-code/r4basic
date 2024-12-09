#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "OLIN-DEVICES";
const char* password = "Engineering4Every1!";

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Sync every 60 seconds

// HC-SR04 pins
const int trigPin = 4;
const int echoPin = 5;

unsigned long lastNTPTime = 0;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP
  timeClient.begin();

  // Set up HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  timeClient.update();
  unsigned long currentNTPTime = timeClient.getEpochTime();

  if (currentNTPTime != lastNTPTime) {
    lastNTPTime = currentNTPTime;

    // Send pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    Serial.println("Pulse sent!");

    // Listen for echo
    long duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
    if (duration > 0) {
      float distance = duration * 0.034 / 2; // Calculate distance in cm
      Serial.print("Received echo! Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("No echo received.");
    }
  }
}
