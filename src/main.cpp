#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SERVO_1_PIN 2
#define SERVO_2_PIN 3
#define SERVO_3_PIN 4

#define JOYSTICK_1_XAXIS_PIN A0
#define JOYSTICK_1_YAXIS_PIN A1
#define JOYSTICK_2_XAXIS_PIN A2
#define JOYSTICK_2_YAXIS_PIN A3

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo servoLeft;
Servo servoRight;
Servo servoMiddle;

void main_statictest();
void loop_statictest();

int axisConvertServo(int jAxisValue) {
  if (jAxisValue >= 470 && jAxisValue <= 550) return 1500;
  return map(jAxisValue, 0, 1023, 1000, 2000);
}

int inverseServo(int sAxisValue) {
  return map(sAxisValue, 1000, 2000, 2000, 1000);
}

void printAxis(const char* axis, int joystick, int pwm) {
  Serial.print(" ");
  Serial.print(axis);
  Serial.print(": ");
  Serial.print(joystick);
  Serial.print("; ");
  Serial.print(pwm);
}

void oledPrintAxis(const char* axis, int joystick, int pwm) {
  display.print(axis);
  display.print(": ");
  display.print(axis);
  display.print("; ");
  display.print(pwm);
  display.println();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Nano startup");

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();

  pinMode(JOYSTICK_1_XAXIS_PIN, INPUT);
  pinMode(JOYSTICK_1_YAXIS_PIN, INPUT);
  pinMode(JOYSTICK_2_XAXIS_PIN, INPUT);
  pinMode(JOYSTICK_2_YAXIS_PIN, INPUT);

  display.clearDisplay();
  display.print("Servo initialize");

  servoLeft.attach(SERVO_1_PIN);
  servoRight.attach(SERVO_2_PIN);
  servoMiddle.attach(SERVO_3_PIN);
  delay(1000);

  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  servoMiddle.writeMicroseconds(1500);
  delay(20);

  display.clearDisplay();
  display.print("OK");
  Serial.println("OK");
  delay(2000);
}

void loop() {
  int xJoystick1Val = analogRead(JOYSTICK_1_XAXIS_PIN);
  int yJoystick1Val = analogRead(JOYSTICK_1_YAXIS_PIN);
  int xJoystick2Val = analogRead(JOYSTICK_2_XAXIS_PIN);
  int yJoystick2Val = analogRead(JOYSTICK_2_YAXIS_PIN);

  int xPWM1Val = axisConvertServo(xJoystick1Val);               // horizontal control
  int yPWM1Val = inverseServo(axisConvertServo(yJoystick1Val)); // horizontal control
  int xPWM2Val = inverseServo(axisConvertServo(xJoystick2Val)); // vertical control
  int yPWM2Val = inverseServo(axisConvertServo(yJoystick2Val));

  // notes:
  // we have to inverse it (down is x+ top is x-) (right is y+ left is y-)
  // top-down is x
  // left right is y

  printAxis("LX", xJoystick1Val, xPWM1Val);
  printAxis("LY", yJoystick1Val, yPWM1Val);
  printAxis("RX", xJoystick1Val, xPWM2Val);
  printAxis("RY", yJoystick1Val, yPWM2Val);
  Serial.println();

  display.clearDisplay();
  oledPrintAxis("LX", xJoystick1Val, xPWM1Val);
  oledPrintAxis("LY", yJoystick1Val, yPWM1Val);
  oledPrintAxis("RX", xJoystick2Val, xPWM2Val);
  oledPrintAxis("RY", yJoystick2Val, yPWM2Val);

  servoLeft.writeMicroseconds(xPWM1Val);
  servoRight.writeMicroseconds(xPWM1Val);
  
  servoMiddle.writeMicroseconds(xPWM2Val);
  delay(100);
}
