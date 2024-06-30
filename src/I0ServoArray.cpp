#include <I0ServoArray.h>

void I0ServoArray::moveServo3Series(double servo1Angle, double servo2Angle, double servo3Angle) {
  servos[0].moveServoSyncX(servo1Angle, SERVO_SPEED);
  servos[1].moveServoSyncX(servo2Angle, SERVO_SPEED);
  servos[2].moveServoSyncX(servo3Angle, SERVO_SPEED);
}

void I0ServoArray::moveServo3Sync(double servo1Angle, double servo2Angle, double servo3Angle) {
  int uSeconds1 = I0Servo::angle2uSeconds(servo1Angle, (float)SERVO_MIN_ANGLE, (float)SERVO_MAX_ANGLE, (float)SERVO_MIN_USECONDS, (float)SERVO_MAX_USECONDS);
  int uSeconds2 = I0Servo::angle2uSeconds(servo2Angle, (float)SERVO_MIN_ANGLE, (float)SERVO_MAX_ANGLE, (float)SERVO_MIN_USECONDS, (float)SERVO_MAX_USECONDS);
  int uSeconds3 = I0Servo::angle2uSeconds(servo3Angle, (float)SERVO_MIN_ANGLE, (float)SERVO_MAX_ANGLE, (float)SERVO_MIN_USECONDS, (float)SERVO_MAX_USECONDS);

  ServoSync servoSync[SERVOS_COUNT];

  servoSync[0] = { uSeconds1 - servos[0].uSeconds, 1, servos[0].uSeconds };
  servoSync[1] = { uSeconds2 - servos[1].uSeconds, 1, servos[1].uSeconds };
  servoSync[2] = { uSeconds3 - servos[2].uSeconds, 1, servos[2].uSeconds };

  int maxDiffuSeconds = getMaxuSecondsDiff(servoSync[0].diffuSeconds, servoSync[1].diffuSeconds, servoSync[2].diffuSeconds);

  servoSync[0].stepuSeconds = (double)servoSync[0].diffuSeconds / (double)maxDiffuSeconds;
  servoSync[1].stepuSeconds = (double)servoSync[1].diffuSeconds / (double)maxDiffuSeconds;
  servoSync[2].stepuSeconds = (double)servoSync[2].diffuSeconds / (double)maxDiffuSeconds;


  for (int i = 0; i < maxDiffuSeconds; i++) {
    for (int j = 0; j < SERVOS_COUNT; j++) {
      servoSync[j].uSeconds = servoSync[j].uSeconds + servoSync[j].stepuSeconds;
      int uSeconds = round(servoSync[j].uSeconds);
      servos[j].writeMicroseconds(uSeconds);
      servos[j].uSeconds = uSeconds;
      // Serial.printf("[servo %d] uSeconds: %d servoSync[j].uSeconds: %.2f \r\n", j, uSeconds, servoSync[j].uSeconds);
    }
    // speed control
    delay(5);
  }

  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target uSeconds: %d actual uSeconds: %d\r\n", 1, servo1Angle, servos[0].read(), uSeconds1, servos[0].uSeconds);
  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target uSeconds: %d actual uSeconds: %d\r\n", 2, servo2Angle, servos[1].read(), uSeconds2, servos[1].uSeconds);
  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target uSeconds: %d actual uSeconds: %d\r\n", 3, servo3Angle, servos[2].read(), uSeconds3, servos[2].uSeconds);
}

int I0ServoArray::getMaxuSecondsDiff(int diffuSeconds1, int diffuSeconds2, int diffuSeconds3) {
  int arr[SERVOS_COUNT] = { diffuSeconds1, diffuSeconds2, diffuSeconds3 };
  int maxAngleDiff = abs(arr[0]);
  for (int i = 1; i <= SERVOS_COUNT - 1; i++) {
    if (abs(arr[i]) > maxAngleDiff) {
      maxAngleDiff = abs(arr[i]);
    }
  }

  return maxAngleDiff;
}