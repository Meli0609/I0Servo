#include <I0Servo.h>

long I0Servo::angle2uSeconds(float x, float in_min, float in_max, float out_min, float out_max) {
  const float run = in_max - in_min;
  if (run == 0) {
    return -1;  // AVR returns -1, SAM returns 0
  }
  const float rise = out_max - out_min;
  const float delta = x - in_min;
  return (delta * rise) / run + out_min;
}

void I0Servo::setServoUSeconds(int targetUseconds) {
  // int currentUseconds = readMicroseconds()
  int currentUseconds = uSeconds;
  if (currentUseconds < targetUseconds) {
    for (int uSeconds = currentUseconds; uSeconds <= targetUseconds; uSeconds++) {
      writeMicroseconds(uSeconds);
      uSeconds = uSeconds;
    }
  } else {
    for (int uSeconds = currentUseconds; uSeconds >= targetUseconds; uSeconds--) {
      writeMicroseconds(uSeconds);
      uSeconds = uSeconds;
    }
  }
}

void I0Servo::moveServoSyncX(float angle, uint16_t servoSpeed) {
  int targetUseconds = I0Servo::angle2uSeconds(angle, (float)SERVO_MIN_ANGLE, (float)SERVO_MAX_ANGLE, (float)SERVO_MIN_USECONDS, (float)SERVO_MAX_USECONDS);
  setServoUSeconds(targetUseconds);
}

String I0Servo::getStatus() {
  char status[200];
  sprintf(status, "angle: %d uSeconds: %d uSeconds(internal): %d", read(), readMicroseconds(), uSeconds);
  return status;
}
