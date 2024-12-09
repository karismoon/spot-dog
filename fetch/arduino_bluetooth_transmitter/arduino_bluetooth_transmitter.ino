#include <ArduinoBLE.h>

BLEService gpsService(" b83bbcfe-d51f-44c2-b127-99cba4b8d647");
BLEStringCharacteristic gpsCharacteristic("77e405f4-0f06-40c3-9cfb-c17fbaacc313", BLERead | BLENotify, 32);

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to initialize
  Serial.println("Serial monitor initialized");

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  // Set up the service and characteristic
  BLE.setLocalName("GPS_Peripheral");
  BLE.setAdvertisedService(gpsService);
  gpsService.addCharacteristic(gpsCharacteristic);
  BLE.addService(gpsService);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE Peripheral Ready to Advertise");
}

void loop() {
  // Check for connections
  BLEDevice central = BLE.central();

  if (central) { // If connected to a central device
    Serial.println("Connected to Central");

    while (central.connected()) {
      String gpsData = getGPSData();
      gpsCharacteristic.writeValue(gpsData);

      Serial.print("Sent GPS Data: ");
      Serial.println(gpsData);
      delay(5000); // Send every 5 seconds
    }

    Serial.println("Disconnected from Central");
  }
}

String getGPSData() {
  // Replace this with actual code to read from the GPS
  String gpsData = "Latitude: 37.7749, Longitude: -122.4194"; // Example GPS data
  return gpsData;
}