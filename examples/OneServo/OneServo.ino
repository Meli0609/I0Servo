#include "I0Servo.h"

#define SERVO_PIN 14

#define DS3235_SERVO_MIN_MICRO_SECONDS 500
#define DS3235_SERVO_MAX_MICRO_SECONDS 2500
#define DS3235_SERVO_MIN_ANGLE 0
#define DS3235_SERVO_MAX_ANGLE 270

// serial
#define BAUDRATE 115200

I0Servo i0Servo = I0Servo();

void serialSetup() {
  Serial.begin(BAUDRATE);
}

void setup() {
  // serial
  serialSetup();

  //servo
  ESP32PWMX::allocateTimer(0);
  ESP32PWMX::allocateTimer(1);
  ESP32PWMX::allocateTimer(2);
  ESP32PWMX::allocateTimer(3);

  i0Servo.attach(SERVO_PIN, DS3235_SERVO_MIN_MICRO_SECONDS, DS3235_SERVO_MAX_MICRO_SECONDS, DS3235_SERVO_MIN_ANGLE, DS3235_SERVO_MAX_ANGLE);
  i0Servo.write(90);

  delay(1000);

  Serial.printf("Servo angle: %.2f\r\n", i0Servo.readAngle());
  Serial.printf("Servo pwm microseconds: %d\r\n", i0Servo.readMicroseconds());

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}
