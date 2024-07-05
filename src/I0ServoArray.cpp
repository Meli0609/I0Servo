#include "I0ServoArray.h"

template <int SERVOS_COUNT>
void I0ServoArray<SERVOS_COUNT>::moveServo3Series(float servo1Angle, float servo2Angle, float servo3Angle) {
  servos[0].setToAngle(servo1Angle, SERVO_SPEED);
  servos[1].setToAngle(servo2Angle, SERVO_SPEED);
  servos[2].setToAngle(servo3Angle, SERVO_SPEED);
}

template <int SERVOS_COUNT>
void I0ServoArray<SERVOS_COUNT>::moveServo3Sync(float servo1Angle, float servo2Angle, float servo3Angle) {
  int microSeconds1 = servos[0].angle2MicroSeconds(servo1Angle);
  int microSeconds2 = servos[1].angle2MicroSeconds(servo2Angle);
  int microSeconds3 = servos[2].angle2MicroSeconds(servo3Angle);

  ServoSync servoSync[this->SERVOS_COUNT];

  servoSync[0] = { microSeconds1 - servos[0].readMicroseconds(), 1, servos[0].readMicroseconds() };
  servoSync[1] = { microSeconds2 - servos[1].readMicroseconds(), 1, servos[1].readMicroseconds() };
  servoSync[2] = { microSeconds3 - servos[2].readMicroseconds(), 1, servos[2].readMicroseconds() };

  int maxDiffMicroSeconds = getMaxMicroSecondsDiff(servoSync[0].diffMicroSeconds, servoSync[1].diffMicroSeconds, servoSync[2].diffMicroSeconds);

  servoSync[0].stepMicroSeconds = (float)servoSync[0].diffMicroSeconds / (float)maxDiffMicroSeconds;
  servoSync[1].stepMicroSeconds = (float)servoSync[1].diffMicroSeconds / (float)maxDiffMicroSeconds;
  servoSync[2].stepMicroSeconds = (float)servoSync[2].diffMicroSeconds / (float)maxDiffMicroSeconds;

  for (int i = 0; i < maxDiffMicroSeconds; i++) {
    for (int j = 0; j < this->SERVOS_COUNT; j++) {
      servoSync[j].microSeconds = servoSync[j].microSeconds + servoSync[j].stepMicroSeconds;
      int microSeconds = round(servoSync[j].microSeconds);
      servos[j].writeMicroseconds(microSeconds);
      // Serial.printf("[servo %d] microSeconds: %d servoSync[j].microSeconds: %.2f \r\n", j, microSeconds, servoSync[j].microSeconds);
    }
    // speed control
    delay(servos[0].speed);
  }

  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target microSeconds: %d actual microSeconds: %d\r\n", 1, servo1Angle, servos[0].read(), microSeconds1, servos[0].microSeconds);
  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target microSeconds: %d actual microSeconds: %d\r\n", 2, servo2Angle, servos[1].read(), microSeconds2, servos[1].microSeconds);
  // Serial.printf("[servo %d] target angle: %.2f actual angle: %d target microSeconds: %d actual microSeconds: %d\r\n", 3, servo3Angle, servos[2].read(), microSeconds3, servos[2].microSeconds);
}

template <int SERVOS_COUNT>
int I0ServoArray<SERVOS_COUNT>::getMaxMicroSecondsDiff(int diff1MicroSeconds, int diff2MicroSeconds, int diff3MicroSeconds) {
  int arr[this->SERVOS_COUNT] = { diff1MicroSeconds, diff2MicroSeconds, diff3MicroSeconds };
  int maxAngleDiff = abs(arr[0]);
  for (int i = 1; i <= this->SERVOS_COUNT - 1; i++) {
    if (abs(arr[i]) > maxAngleDiff) {
      maxAngleDiff = abs(arr[i]);
    }
  }

  return maxAngleDiff;
}