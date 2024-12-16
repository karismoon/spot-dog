#include <Adafruit_MotorShield.h>
#include <Servo.h>

// Motor setup
#define relay 2
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
Servo myservojaw;

int motorSpeed = 50;  // Variable to store the motor speed from the slider
int fetchDistance = 10; // Distance from ball to sensor needed for fetch
int maxDistance = 30; // Max distance the ball could be in the beginning
const int maxSpeed = 255;
const int minSpeed = 0;

// Ultrasonic sensor setup
const int trigPin = 11;
const int echoPin = 10;

float duration, distance;

void setup() {
  // Motor initialization
  AFMS.begin();
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  Motor1->run(FORWARD);
  Motor2->run(FORWARD);
  Motor3->run(FORWARD);
  Motor4->run(FORWARD);

  myservobackright.attach(0);
  myservobackleft.attach(1);
  myservofrontright.attach(2);
  myservofrontleft.attach(3);
  myservoneck.attach(4);
  myservojaw.attach(9);
  stand();

  // stayLaying();

  // Ultrasonic sensor initialization
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Serial.println("No ball");
  // // Turn in small increments until the ball is detected
  // if (detectBall() > 0 && detectBall() < maxDistance) {
  //   Serial.println("Ball detected!");
  //   stopMovement();
  //   moveTowardsBall();
  // } else {
  //   stand();
  //   turn();
  //   lay();
  // }
  // delay(100); // Small delay to avoid excessive sensor reads
  stand();
  myservoneck.write(90);
  detectBall();
}

void bite() {
  int angle = 180;
  Serial.print("bite angle: ");
  Serial.println(angle);
  myservojaw.write(angle);  
}

// Function to turn the robot incrementally
void turn() {
  Serial.println("Turning right");
  
  speedSet(motorSpeed);
  Motor1->run(FORWARD);
  Motor2->run(BACKWARD);
  Motor3->run(BACKWARD);
  Motor4->run(FORWARD);

  delay(500); // Adjust this for the size of the increment turn
  stopMovement();
}

// Function to detect the ball using the ultrasonic sensor
int detectBall() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2; // Convert duration to distance

  Serial.print("Distance: ");
  Serial.println(distance);
  
  return distance;
}

// Function to set speed of all motors
void speedSet(int speed) {
  Motor1->setSpeed(speed);
  Motor2->setSpeed(speed);
  Motor3->setSpeed(speed);
  Motor4->setSpeed(speed);
}

// Function to stop all motor movement
void stopMovement() {
  speedSet(0);
}

// Function to move the robot towards the ball
void moveTowardsBall() {
  Serial.println("Moving towards the ball...");
  stayLaying();
  
  while (detectBall() > fetchDistance) {
    speedSet(motorSpeed);

    Motor1->run(FORWARD);
    Motor2->run(FORWARD);
    Motor3->run(FORWARD);
    Motor4->run(FORWARD);
  }

  Serial.println("Ball is close enough.");
  stopMovement();
}

// Function to make the robot stand
void stand() {
  stopMovement();
  myservobackleft.write(90);
  myservobackright.write(90);
  myservofrontleft.write(90);
  myservofrontright.write(90);
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

  delay(2000);
  
  for (int pos = 170; pos >= 90; pos--) {
    myservobackleft.write(pos);    // Back left motor
    myservobackright.write(90 + (90 - pos)); // Back right motor
    myservofrontright.write(90 + (90 - pos));
    myservofrontleft.write(pos);
    delay(20);                     // Delay for smooth movement
  }
}

void stayLaying() {
  int pos = 170;
  myservobackleft.write(pos);    // Back left motor
  myservobackright.write(90 + (90-pos)); // Back right motor
  myservofrontright.write(90 + (90 - pos));
  myservofrontleft.write(pos);
}
