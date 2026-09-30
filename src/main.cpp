#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define THROTTLE_DEADZONE     40
#define TURNING_DEADZONE      60
#define VERTICAL_DEADZONE     40
#define MAX_THRUST_VERTICAL   300
#define MAX_THRUST_HORIZONTAL 200
#define SLOW_MODE_SCALE       75

#define SERVO_1_PIN 5 // digital pins, left
#define SERVO_2_PIN 6 // right
#define SERVO_3_PIN 7 // middle

#define JOYSTICK_1_XAXIS_PIN A0 // left joystick
#define JOYSTICK_1_YAXIS_PIN A1
#define JOYSTICK_1_SWBTN_PIN 10 // digital
#define JOYSTICK_2_XAXIS_PIN A2 // right joystick
#define JOYSTICK_2_YAXIS_PIN A3
#define JOYSTICK_2_SWBTN_PIN 11 // digital

void main_statictest();
void loop_statictest();

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo servoLeft;
Servo servoRight;
Servo servoMiddle;

bool isSlowMode = false;
bool lastSlowModeState = HIGH;

int readJoystick(int pin, bool inverted, int deadzone, int maxThrust) {
    int raw = analogRead(pin);
    int offset = raw - 512;
    if (abs(offset) < deadzone) return 0;

    int output;
    if (offset > 0) {
        output = map(raw, 512 + deadzone, 1023, 0, maxThrust);
    } else {
        output = map(raw, 0, 512 - deadzone, -maxThrust, 0);
    }

    output = constrain(output, -maxThrust, maxThrust);
    if (inverted) output = -output;
    return output;
}

int applyCurve(int input, int maxThrust) {
    long magnitude = abs(input);
    long curved = magnitude * magnitude / maxThrust;
    if (input < 0) curved = -curved;
    return (int)curved;
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
    display.println("Test");
    display.display();

    pinMode(JOYSTICK_1_XAXIS_PIN, INPUT);
    pinMode(JOYSTICK_1_YAXIS_PIN, INPUT);
    pinMode(JOYSTICK_1_SWBTN_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_2_XAXIS_PIN, INPUT);
    pinMode(JOYSTICK_2_YAXIS_PIN, INPUT);
    pinMode(JOYSTICK_2_SWBTN_PIN, INPUT_PULLUP);

    servoLeft.attach(SERVO_1_PIN);
    servoRight.attach(SERVO_2_PIN);
    servoMiddle.attach(SERVO_3_PIN);

    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1500);
    servoMiddle.writeMicroseconds(1500);
    delay(3000);
}

void loop() {
    // slow mode check
    bool rtSlowMode = digitalRead(JOYSTICK_1_SWBTN_PIN);
    if (lastSlowModeState == HIGH && rtSlowMode == LOW) {
        isSlowMode = !isSlowMode;
        delay(30);
    }
    lastSlowModeState = rtSlowMode;

    // read joystick
    int throttle = readJoystick(JOYSTICK_1_XAXIS_PIN, false, THROTTLE_DEADZONE, MAX_THRUST_HORIZONTAL);
    int turn     = readJoystick(JOYSTICK_1_YAXIS_PIN, false, TURNING_DEADZONE,  MAX_THRUST_HORIZONTAL);
    int vertical = readJoystick(JOYSTICK_2_XAXIS_PIN, true,  VERTICAL_DEADZONE, MAX_THRUST_VERTICAL);

    // curve for smoother turning
    turn = applyCurve(turn, MAX_THRUST_HORIZONTAL);

    // calculate l/r thruster
    int leftThrust  = throttle + turn;
    int rightThrust = throttle - turn;

    // enforce max thrust
    int maxMagnitude = max(abs(leftThrust), abs(rightThrust));
    if (maxMagnitude > MAX_THRUST_HORIZONTAL) {
        leftThrust  = (long)leftThrust * MAX_THRUST_HORIZONTAL / maxMagnitude;
        rightThrust = (long)rightThrust * MAX_THRUST_HORIZONTAL / maxMagnitude;
    }

    // apply slow mode
    if (isSlowMode) {
        leftThrust  = (long)leftThrust  * SLOW_MODE_SCALE / 100;
        rightThrust = (long)rightThrust * SLOW_MODE_SCALE / 100;
    }

    // send command
    servoLeft.writeMicroseconds(thrustToPWM(leftThrust));
    servoRight.writeMicroseconds(thrustToPWM(rightThrust));
    servoMiddle.writeMicroseconds(thrustToPWM(vertical));
    delay(20);
}
