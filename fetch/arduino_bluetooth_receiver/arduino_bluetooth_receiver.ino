#include <Arduino.h>
#include <ArduinoBLE.h>

BLEService gpsService("b83bbcfe-d51f-44c2-b127-99cba4b8d647");
BLECharacteristic gpsCharacteristic("77e405f4-0f06-40c3-9cfb-c17fbaacc313", BLERead | BLENotify, 32);

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to initialize

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("BLE failed to initialize!");
    while (1);
  }

  // Start scanning for BLE devices
  BLE.scanForUuid(gpsService.uuid());
}

void loop() {
  BLEDevice peripheral = BLE.available(); // Check if the peripheral is available

  if (peripheral) {
    Serial.print("Found device: ");
    Serial.println(peripheral.address());

    if (peripheral.connect()) { // Attempt to connect
      Serial.println("Connected to Peripheral");

      // Discover services and characteristics
      peripheral.discoverAttributes();
      BLECharacteristic gpsChar = peripheral.characteristic(gpsCharacteristic.uuid());

      if (gpsChar) {
        while (peripheral.connected()) {
          Serial.print("Received GPS Data: ");
          Serial.println(gpsChar);
          // delay(500); // Wait before collecting data again
        }
      } else {
        Serial.println("GPS Characteristic not found!");
      }
    } else {
      Serial.println("Failed to connect!");
    }
  }
}