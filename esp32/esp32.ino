#include <Arduino.h>
#include <ArduinoBLE.h>

BLEService gpsService("b83bbcfe-d51f-44c2-b127-99cba4b8d647"); // UUID for the service
BLECharacteristic gpsCharacteristic("77e405f4-0f06-40c3-9cfb-c17fbaacc313", BLERead | BLENotify); // UUID for the characteristic

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("BLE failed to initialize!");
    while (1);
  }

  // Add service and characteristic
  BLE.addService(gpsService);
  BLE.addCharacteristic(gpsCharacteristic);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE Device is Ready to Pair");
}

void loop() {
  // Listen for BLE connections
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to ");
    Serial.println(central.address());

    while (central.connected()) {
      String gpsData = getGPSData(); // Function to get GPS data
      gpsCharacteristic.setValue(gpsData); // Update the characteristic with GPS data
      gpsCharacteristic.notify(); // Notify the client of the new value

      delay(1000); // Send data every 1 second
    }

    Serial.print("Disconnected from ");
    Serial.println(central.address());
  }
}

String getGPSData() {
  // Replace this with actual code to read from the GPS
  String gpsData = "Latitude: 37.7749, Longitude: -122.4194"; // Example GPS data
  return gpsData;
}