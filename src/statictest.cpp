#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define SERVO_1_PIN 2
#define SERVO_2_PIN 3
#define SERVO_3_PIN 4

#define JOYSTICK_1_XAXIS_PIN A1
#define JOYSTICK_1_YAXIS_PIN A2
#define JOYSTICK_2_XAXIS_PIN A3
#define JOYSTICK_2_YAXIS_PIN A4

static Servo servo1;
static Servo servo2;
static Servo servo3;

void main_statictest() {
    Serial.begin(9600);
    Serial.println("Init");
    servo1.attach(SERVO_1_PIN);
    servo2.attach(SERVO_2_PIN);
    servo3.attach(SERVO_3_PIN);
    delay(1000);

    servo1.writeMicroseconds(1500);
    servo2.writeMicroseconds(1500);
    servo3.writeMicroseconds(1500);
    delay(1000);
}

void loop_statictest() {
    Serial.println("2000");
    servo1.writeMicroseconds(2000);
    servo2.writeMicroseconds(2000);
    servo3.writeMicroseconds(2000);
    delay(5020);
    Serial.println("1000");
    servo1.writeMicroseconds(1000);
    servo2.writeMicroseconds(1000);
    servo3.writeMicroseconds(1000);
    delay(5020);
    Serial.println("1500");
    servo1.writeMicroseconds(1500);
    servo2.writeMicroseconds(1500);
    servo3.writeMicroseconds(1500);
    delay(5020);


}