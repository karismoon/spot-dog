#include "WiFiS3.h"
#include <Adafruit_MotorShield.h>

#define relay 2

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);

int motorSpeed = 50;  // Variable to store the motor speed from the slider
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
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /on") >= 0) {
              forward();
            } else if (header.indexOf("GET /off") >= 0) {
              stop();
            } else if (header.indexOf("GET /spin") >= 0) {
              spin();
            } else if (header.indexOf("GET /stopspin") >= 0) {
              stop_spin();
            } else if (header.indexOf("GET /speed/increase") >= 0) {
              increaseSpeed();
            } else if (header.indexOf("GET /speed/decrease") >= 0) {
              decreaseSpeed();
            }

            int speedIndex = header.indexOf("GET /speed/");
            if (speedIndex >= 0) {
              int newSpeed = header.substring(speedIndex + 11).toInt();
              motorSpeed = newSpeed;
              setSpeed(newSpeed);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Helvetica; text-align: center;}");
            client.println(".button { padding: 16px 40px; border-radius: 20px; font-size: 30px; cursor: pointer;}");
            client.println(".on { background-color: #FF0000; color: white; }");
            client.println(".off { background-color: #000000; color: white; }");
            client.println(".spin { background-color: #4CAF50; color: white; }");
            client.println(".stopspin { background-color: #FFA500; color: white; }</style></head>");

            client.println("<body><h1>PIE Project HTML Test Site</h1>");
            client.println("<p>Click to Start, Stop, Spin, or Stop Spin</p>");
            
            if (output == "on") {
              client.println("<p><a href=\"/off\"><button class=\"button on\">Stop</button></a></p>");
            } else {
              client.println("<p><a href=\"/on\"><button class=\"button off\">Forward</button></a></p>");
            }

            if (output == "spin") {
              client.println("<p><a href=\"/stopspin\"><button class=\"button stopspin\">Stop Spin</button></a></p>");
            } else {
              client.println("<p><a href=\"/spin\"><button class=\"button spin\">Spin</button></a></p>");
            }

            client.println("<h2>Motor Speed Control</h2>");
            client.println("<input type=\"range\" min=\"0\" max=\"255\" value=\"" + String(motorSpeed) + "\" class=\"slider\" id=\"speedRange\" onchange=\"updateSpeed(this.value)\">");
            client.println("<p>Speed: <span id=\"speedValue\">" + String(motorSpeed) + "</span></p>");

            client.println("<script>");
            client.println("function updateSpeed(val) {");
            client.println("  fetch('/speed/' + val);");
            client.println("  document.getElementById('speedValue').innerText = val;");
            client.println("}");
            client.println("</script>");

            client.println("</body></html>");
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

void setSpeed(int speed) {
  if (output == "on" || output == "spin") {
    Motor1->setSpeed(speed);
    Motor2->setSpeed(speed);
    Motor3->setSpeed(speed);
    Motor4->setSpeed(speed);
  }
}


void forward() {
  if (output == "spin") {
    stop_spin();  // Stop spin if currently active
  }
  output = "on";
  setSpeed(motorSpeed);
}

void stop() {
  output = "off";
  Motor1->setSpeed(0);
  Motor2->setSpeed(0);
  Motor3->setSpeed(0);
  Motor4->setSpeed(0);
}

void spin() {
  if (output == "on") {
    stop();  // Stop forward motion if currently active
  }
  output = "spin";
  Motor2->run(BACKWARD);
  Motor3->run(BACKWARD);
  setSpeed(motorSpeed);
}

void stop_spin() {
  Motor2->run(FORWARD);
  Motor3->run(FORWARD);
  output = "off";
  stop();
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

void fast() {
  motorSpeed = 150;
  setSpeed(motorSpeed);
}

void slow() {
  motorSpeed = 50;
  setSpeed(motorSpeed);
}
