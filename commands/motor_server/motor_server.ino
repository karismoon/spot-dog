#include "WiFiS3.h"
#include <Adafruit_MotorShield.h>
#include <Servo.h>

#define relay 2

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);

Servo myservofrontright;
Servo myservofrontleft;
Servo myservobackleft;
Servo myservobackright;
Servo myservoneck;

int motorSpeed = 100;  // Variable to store the motor speed from the slider
const int maxSpeed = 255;
const int minSpeed = 0;

char ssid[] = "iPhone (1793)"; // Enter your WIFI SSID
char pass[] = "yeehawhawyee";  // Enter your WIFI password

String output = "off";
String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  AFMS.begin();
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();
  printWifiStatus();
  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
  Motor3->run(FORWARD);
  Motor4->run(FORWARD);

  myservobackright.attach(0);
  myservobackleft.attach(1);
  myservofrontright.attach(2);
  myservofrontleft.attach(3);
  myservoneck.attach(4);
  stand();
  neck();
}

void loop() {
  webServer();
}

void webServer() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          stand();
          neck();
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/plain");
            client.println("Connection: close");
            client.println();
            // Check and execute commands based on audio recognition keywords
            if (header.indexOf("GET /on") >= 0) {
              forward();
            } else if (header.indexOf("GET /off") >= 0) {
              stop();
            } else if (header.indexOf("GET /backward") >= 0) {
              backward();
            } else if (header.indexOf("GET /left") >= 0) {
              turnLeft();
            } else if (header.indexOf("GET /right") >= 0) {
              turnRight();
            }
            else if (header.indexOf("GET /heel") >= 0) {
              heel(); // Call the heel function
            }
            else if (header.indexOf("GET /paw") >= 0) {
              paw();
            }
            else if (header.indexOf("GET /sit") >= 0) {
              sit();
            }
            else if (header.indexOf("GET /lay") >= 0) {
              lay();
            }
            else if (header.indexOf("GET /nod") >= 0) {
              nod();
            }
            else if (header.indexOf("GET /lift") >= 0) {
              lift();
            }
            else if (header.indexOf("GET /up") >= 0) {
              standup();
            }

            int speedIndex = header.indexOf("GET /speed/");
            if (speedIndex >= 0) {
              int newSpeed = header.substring(speedIndex + 11).toInt();
              motorSpeed = newSpeed;
              setSpeed(newSpeed);
            }

            // Send acknowledgment to the client
            client.println("Command received and processed.");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
  }
}

void heel() {
  int targetRSSI = -49; // Adjust as needed for proximity
  output = "heel";
  while (WiFi.RSSI() < targetRSSI) {
    forward(); // Adjust speed if needed
    delay(100);       // Brief pause to prevent constant checking
  }
  stop(); // Stop when the target RSSI is reached
}

void setSpeed(int speed) {
  if (output == "on" || output == "spin") {
    Motor1->setSpeed(speed);
    Motor2->setSpeed(speed);
    Motor3->setSpeed(speed);
    Motor4->setSpeed(speed);
  }
}

void backward() {
  output = "on";
  Motor1->run(BACKWARD);
  Motor2->run(BACKWARD);
  Motor3->run(BACKWARD);
  Motor4->run(BACKWARD);
  setSpeed(100);
}

void turnLeft() {
  output = "on";
  Motor1->run(BACKWARD);
  Motor2->run(FORWARD);
  Motor3->run(FORWARD);
  Motor4->run(BACKWARD);
  setSpeed(100);
}

void turnRight() {
  output = "on";
  Motor1->run(FORWARD);
  Motor2->run(BACKWARD);
  Motor3->run(BACKWARD);
  Motor4->run(FORWARD);
  setSpeed(100);
}

void forward() {
  output = "on";
  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
  Motor3->run(FORWARD);
  Motor4->run(FORWARD);
  setSpeed(100);
}

void stop() {
  setSpeed(0);
  output = "off";
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("Now open this URL on your browser --> http://");
  Serial.println(ip);
}

void stand(){
  myservobackleft.write(90);
  myservobackright.write(90);
  myservofrontleft.write(90);
  myservofrontright.write(90);
}

void neck(){
  myservoneck.write(90);
}

void paw(){
  for (int pos = 90; pos >= 1; pos--) {
    myservofrontright.write(pos);    // Back left motor
    myservobackleft.write(75);
    myservobackright.write(105);
    myservofrontleft.write(90);
    delay(20);                     // Delay for smooth movement
  }

  delay(5000);
  
  for (int pos = 1; pos <= 90; pos++) {
    myservofrontright.write(pos);
    myservobackleft.write(75);
    myservobackright.write(105);
    myservofrontleft.write(90);
    delay(20);                     // Delay for smooth movement
  }
}

// Function to perform the servo movements
void sit() {
  // Back motors: Step from 90 to 20 and 90 to 160, then back
  for (int pos = 90; pos >= 30; pos--) {
    myservobackleft.write(pos);    // Back left motor
    myservobackright.write(90 + (90-pos)); // Back right motor
    myservofrontright.write(130);
    myservofrontleft.write(50);
    myservoneck.write(110);
    delay(20);                     // Delay for smooth movement
  }

  delay(10000);
  
  for (int pos = 30; pos <= 90; pos++) {
    myservobackleft.write(pos);    // Back left motor
    myservobackright.write(90 + (90 - pos)); // Back right motor
    myservofrontright.write(90);
    myservofrontleft.write(90);
    delay(20);                     // Delay for smooth movement
  }
}

void lay() {
  // Back motors: Step from 90 to 20 and 90 to 160, then back
  for (int pos = 90; pos <= 170; pos++) {
    myservobackleft.write(pos);    // Back left motor
    myservobackright.write(90 + (90-pos)); // Back right motor
    myservofrontright.write(90 + (90 - pos));
    myservofrontleft.write(pos);
    delay(20);                     // Delay for smooth movement
  }
}

void standup(){
    for (int pos = 170; pos >= 90; pos--) {
    myservobackleft.write(pos);    // Back left motor
    myservobackright.write(90 + (90 - pos)); // Back right motor
    myservofrontright.write(90 + (90 - pos));
    myservofrontleft.write(pos);
    delay(20);                     // Delay for smooth movement
  }
}


void nod(){
  for (int i = 0; i <= 10; i++){
    for (int pos = 90; pos <= 130; pos++) {
      myservoneck.write(pos);
      delay(5);                     // Delay for smooth movement
    }
    for (int pos = 130; pos >= 90; pos--) {
      myservoneck.write(pos);
      delay(5);                     // Delay for smooth movement
    }
  }
}


void lift(){
  for (int pos = 90; pos <= 130; pos++) {
    myservoneck.write(pos);
    delay(100);                     // Delay for smooth movement
  }
  for (int pos = 130; pos >= 90; pos--) {
    myservoneck.write(pos);
    delay(200);                     // Delay for smooth movement
  }
}
