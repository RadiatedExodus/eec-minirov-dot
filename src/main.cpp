#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define JOYSTICK_DEADZONE 40
#define MAX_THRUST 200

#define SERVO_1_PIN 5 // digital pins
#define SERVO_2_PIN 6
#define SERVO_3_PIN 7

#define JOYSTICK_1_XAXIS_PIN A0 // left joystick
#define JOYSTICK_1_YAXIS_PIN A1
#define JOYSTICK_2_XAXIS_PIN A2 // right joystick
#define JOYSTICK_2_YAXIS_PIN A3

void main_statictest();
void loop_statictest();

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo servoLeft;
Servo servoRight;
Servo servoMiddle;

int readJoystick(int pin, bool inverted) {
    int raw = analogRead(pin);
    int offset = raw - 512;
    if (abs(offset) < JOYSTICK_DEADZONE) return 0;

    int output;
    if (offset > 0) {
        output = map(raw, 512 + JOYSTICK_DEADZONE, 1023, 0, MAX_THRUST);
    } else {
        output = map(raw, 0, 512 - JOYSTICK_DEADZONE, -MAX_THRUST, 0);
    }

    output = constrain(output, -MAX_THRUST, MAX_THRUST);
    if (inverted) output = -output;
    return output;
}

int thrustToPWM(int thrust) {
    return 1500 + thrust;
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
    display.display();

    pinMode(JOYSTICK_1_XAXIS_PIN, INPUT);
    pinMode(JOYSTICK_1_YAXIS_PIN, INPUT);
    pinMode(JOYSTICK_2_XAXIS_PIN, INPUT);
    pinMode(JOYSTICK_2_YAXIS_PIN, INPUT);

    servoLeft.attach(SERVO_1_PIN);
    servoRight.attach(SERVO_2_PIN);
    servoMiddle.attach(SERVO_3_PIN);

    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1500);
    servoMiddle.writeMicroseconds(1500);
    delay(3000);
}

void loop() {
    int throttle = readJoystick(JOYSTICK_1_XAXIS_PIN, false);
    int turn     = readJoystick(JOYSTICK_1_YAXIS_PIN, false);
    int vertical = readJoystick(JOYSTICK_2_XAXIS_PIN, true);

    int leftThrust  = throttle + turn;
    int rightThrust = throttle - turn;

    // enforce max thrust
    int maxMagnitude = max(abs(leftThrust), abs(rightThrust));
    if (maxMagnitude > MAX_THRUST) {
        leftThrust = (long)leftThrust * MAX_THRUST / maxMagnitude;
        rightThrust = (long)rightThrust * MAX_THRUST / maxMagnitude;
    }

    servoLeft.writeMicroseconds(thrustToPWM(leftThrust));
    servoRight.writeMicroseconds(thrustToPWM(rightThrust));
    servoMiddle.writeMicroseconds(thrustToPWM(vertical));
    delay(20);
}
