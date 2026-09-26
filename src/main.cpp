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

int axisConvertServo(int jAxisValue) {
  if (jAxisValue >= 470 && jAxisValue <= 550) return 1500;
  return map(jAxisValue, 0, 1023, 1000, 2000);
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
  display.print(joystick); 
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
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);

  pinMode(JOYSTICK_1_XAXIS_PIN, INPUT);
  pinMode(JOYSTICK_1_YAXIS_PIN, INPUT);
  pinMode(JOYSTICK_2_XAXIS_PIN, INPUT);
  pinMode(JOYSTICK_2_YAXIS_PIN, INPUT);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Servo initialize");
  display.display();

  servoLeft.attach(SERVO_1_PIN);
  servoRight.attach(SERVO_2_PIN);
  servoMiddle.attach(SERVO_3_PIN);
  delay(1000);

  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  servoMiddle.writeMicroseconds(1500);
  delay(20);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("OK");
  display.display();
  Serial.println("OK");
  delay(2000);
}

void loop() {
  int xJoystick1Val = analogRead(JOYSTICK_1_XAXIS_PIN);
  int yJoystick1Val = analogRead(JOYSTICK_1_YAXIS_PIN);
  int xJoystick2Val = analogRead(JOYSTICK_2_XAXIS_PIN);
  int yJoystick2Val = analogRead(JOYSTICK_2_YAXIS_PIN);

  int yPWM1Val = axisConvertServo(yJoystick1Val);
  int xPWM2Val = axisConvertServo(xJoystick2Val);
  int yPWM2Val = axisConvertServo(yJoystick2Val);

  int leftServoPWM = 1500;
  int rightServoPWM = 1500;

  // Control Left-Right Motion
  // TURN LEFT
  if (xJoystick1Val < 470) { 
    int offset = map(xJoystick1Val, 470, 0, 0, 500);
    rightServoPWM = 1500 + offset; // turn CW
    leftServoPWM  = 1500 - offset; // turn CCW
  }
  // TURN RIGHT
  else if (xJoystick1Val > 550) { 
    int offset = map(xJoystick1Val, 550, 1023, 0, 500);
    rightServoPWM = 1500 - offset; // turn CCW
    leftServoPWM  = 1500 + offset; // turn CW
  }
  else {
    leftServoPWM  = 1500;
    rightServoPWM = 1500;
  }

  // Output commands to Servos 
  servoLeft.writeMicroseconds(leftServoPWM);
  servoRight.writeMicroseconds(rightServoPWM);
  servoMiddle.writeMicroseconds(xPWM2Val); // Float/Sink control

  printAxis("LX", xJoystick1Val, leftServoPWM);
  printAxis("LY", yJoystick1Val, yPWM1Val);
  printAxis("RX", xJoystick2Val, xPWM2Val);
  printAxis("RY", yJoystick2Val, yPWM2Val);
  Serial.println();

  // OLED Display 
  display.clearDisplay();
  display.setCursor(0, 0);
  oledPrintAxis("LX", xJoystick1Val, leftServoPWM);
  oledPrintAxis("LY", yJoystick1Val, yPWM1Val);
  oledPrintAxis("RX", xJoystick2Val, xPWM2Val);
  oledPrintAxis("RY", yJoystick2Val, yPWM2Val);
  display.display(); // FIXED: Added display.display() to render on OLED

  delay(500); 
}