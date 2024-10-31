#include <Arduino.h>
#include <ArduinoBLE.h>

BLEClient bleClient;
BLEService gpsService("service-UUID"); // Replace with service UUID !!
BLECharacteristic gpsCharacteristic("characteristic-UUID", BLERead | BLENotify); // Replace with characteristic UUID !!

void setup() {
  Serial.begin(115200);
  while (!Serial);

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
    // Connect to the peripheral device
    if (bleClient.connect(peripheral)) {
      Serial.println("Connected to ESP32!");

      // Discover services and characteristics
      bleClient.discoverAttributes();

      // Read the GPS data
      if (bleClient.characteristic(gpsCharacteristic.uuid())) {
        String gpsData = bleClient.readString(gpsCharacteristic);
        Serial.println("GPS Data: " + gpsData); // Print received GPS data
      }

      bleClient.disconnect();
      Serial.println("Disconnected.");
    } else {
      Serial.println("Failed to connect!");
    }
  }

  delay(1000); // Wait for 1 second before next scan
}