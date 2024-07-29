#include <functional>
#include "I0Servo.h"

I0Servo::I0Servo() {
  REFRESH_CPS = PWM_FREQUENCY;
  this->_ticks = DEFAULT_PULSE_WIDTH_TICKS;
  this->_timer_width = DEFAULT_TIMER_WIDTH;
  this->_pinNumber = -1;  // make it clear that we haven't attached a pin to this channel
  this->_timer_width_ticks = pow(2, this->_timer_width);
}

I0Servo::I0Servo(float minAngle, float maxAngle, float minMicroSeconds, float maxMicroSeconds, Direction direction)
  : I0Servo() {
  _minAngle = minAngle;
  _maxAngle = maxAngle;
  _minMicroSeconds = minMicroSeconds;
  _maxMicroSeconds = maxMicroSeconds;
  _direction = direction;
}

long I0Servo::angle2MicroSeconds(float x) {
  return angle2MicroSeconds(x, _minAngle, _maxAngle, _minMicroSeconds, _maxMicroSeconds, _direction);
}

long I0Servo::angle2MicroSeconds(float x, float inMin, float inMax, float outMin, float outMax, Direction direction) {
  const float run = inMax - inMin;
  if (run == 0) {
    return -1;  // AVR returns -1, SAM returns 0
  }
  const float rise = outMax - outMin;
  float delta = x - inMin;
  if (direction == CCW) {
    delta = (inMax - x) - inMin;
  }
  return (delta * rise) / run + outMin;
}

float I0Servo::microSeconds2Angle(int x, float inMin, float inMax, float outMin, float outMax, Direction direction) {
  const float run = inMax - inMin;
  if (run == 0) {
    return -1;  // AVR returns -1, SAM returns 0
  }
  const float rise = outMax - outMin;
  float delta = x - inMin;
  if (direction == CCW) {
    delta = (inMax - x);
  }
  return (delta * rise) / run + outMin;
}

void I0Servo::setToMicroSeconds(int targetMicroseconds) {
  int currentMicroseconds = readMicroseconds();
  if (currentMicroseconds < targetMicroseconds) {
    for (int microseconds = currentMicroseconds; microseconds <= targetMicroseconds; microseconds++) {
      writeMicroseconds(microseconds);
      // speed control
      delay(speed);
    }
  } else {
    for (int microseconds = currentMicroseconds; microseconds >= targetMicroseconds; microseconds--) {
      writeMicroseconds(microseconds);
      // speed control
      delay(speed);
    }
  }
}

void I0Servo::setToAngle(float angle, uint16_t servoSpeed) {
  int targetMicroseconds = I0Servo::angle2MicroSeconds(angle, _minAngle, _maxAngle, _minMicroSeconds, _maxMicroSeconds, _direction);
  setToMicroSeconds(targetMicroseconds);
}

String I0Servo::getStatus() {
  char status[200];
  sprintf(status, "A %.2f US %d MINA %.2f MAXA %.2f MINUS %.2f MAXUS %.2f", readAngle(), readMicroseconds(), _minAngle, _maxAngle, _minMicroSeconds, _maxMicroSeconds);
  return status;
}

ESP32PWMX* I0Servo::getPwm() {
  return &pwm;
}

int I0Servo::attach(int pin) {
  return (this->attach(pin, _minMicroSeconds, _maxMicroSeconds));
}

int I0Servo::attach(int pin, int minMicroSeconds, int maxMicroSeconds) {
  // ESP32 Recommend only the following pins 2,4,12-19,21-23,25-27,32-33
  // ESP32-S2 only the following pins 1-21,26,33-42
  if (pwm.hasPwm(pin)) {

    // OK to proceed; first check for new/reuse
    if (this->_pinNumber < 0)  // we are attaching to a new or previously detached pin; we need to initialize/reinitialize
    {
      this->_ticks = DEFAULT_PULSE_WIDTH_TICKS;
      this->_timer_width = DEFAULT_TIMER_WIDTH;
      this->_timer_width_ticks = pow(2, this->_timer_width);
    }
    this->_pinNumber = pin;
  }

  // min/max checks
  if (minMicroSeconds < DEFAULT_SERVO_MIN_MICRO_SECONDS)  // ensure pulse width is valid
    minMicroSeconds = DEFAULT_SERVO_MIN_MICRO_SECONDS;
  if (maxMicroSeconds > DEFAULT_SERVO_MAX_MICRO_SECONDS)
    maxMicroSeconds = DEFAULT_SERVO_MAX_MICRO_SECONDS;

  _minMicroSeconds = minMicroSeconds;  //store this value in uS
  _maxMicroSeconds = maxMicroSeconds;  //store this value in uS
  // Set up this channel
  // if you want anything other than default timer width, you must call setTimerWidth() before attach

  pwm.attachPin(this->_pinNumber, REFRESH_CPS, this->_timer_width);  // GPIO pin assigned to channel
  return pwm.getChannel();
}

int I0Servo::attach(int pin, int minMicroSeconds, int maxMicroSeconds, int minAngle, int maxAngle) {
  // ESP32 Recommend only the following pins 2,4,12-19,21-23,25-27,32-33
  // ESP32-S2 only the following pins 1-21,26,33-42
  if (pwm.hasPwm(pin)) {

    // OK to proceed; first check for new/reuse
    if (this->_pinNumber < 0)  // we are attaching to a new or previously detached pin; we need to initialize/reinitialize
    {
      this->_ticks = DEFAULT_PULSE_WIDTH_TICKS;
      this->_timer_width = DEFAULT_TIMER_WIDTH;
      this->_timer_width_ticks = pow(2, this->_timer_width);
    }
    this->_pinNumber = pin;
  }

  if (minMicroSeconds < DEFAULT_SERVO_MIN_MICRO_SECONDS)  // ensure pulse width is valid
    minMicroSeconds = DEFAULT_SERVO_MIN_MICRO_SECONDS;
  if (maxMicroSeconds > DEFAULT_SERVO_MAX_MICRO_SECONDS)
    maxMicroSeconds = DEFAULT_SERVO_MAX_MICRO_SECONDS;
  if (minAngle < DEFAULT_SERVO_MIN_ANGLE)
    minAngle = DEFAULT_SERVO_MIN_ANGLE;
  if (maxAngle > DEFAULT_SERVO_MAX_ANGLE)
    maxAngle = DEFAULT_SERVO_MAX_ANGLE;

  _minMicroSeconds = minMicroSeconds;  //store this value in uS
  _maxMicroSeconds = maxMicroSeconds;
  _minAngle = minAngle;
  _maxAngle = maxAngle;

  // Set up this channel
  // if you want anything other than default timer width, you must call setTimerWidth() before attach

  pwm.attachPin(this->_pinNumber, REFRESH_CPS, this->_timer_width);  // GPIO pin assigned to channel
  return pwm.getChannel();
}

void I0Servo::detach() {
  if (this->attached()) {
    //keep track of detached servos channels so we can reuse them if needed
    pwm.detachPin(this->_pinNumber);

    this->_pinNumber = -1;
  }
}

void I0Servo::write(int value) {
  // treat values less than MIN_PULSE_WIDTH (500) as angles in degrees (valid values in microseconds are handled as microseconds)
  if (value < _minMicroSeconds) {
    if (value < 0)
      value = 0;
    else if (value > _maxAngle)
      value = _maxAngle;

    value = angle2MicroSeconds(value, _minAngle, _maxAngle, _minMicroSeconds, _maxMicroSeconds, _direction);
  }
  this->writeMicroseconds(value);
}

void I0Servo::writeMicroseconds(int value) {
  writeTicks(microSecondsToTicks(value));  // convert to ticks
}

void I0Servo::writeTicks(int value) {
  // calculate and store the values for the given channel
  if (this->attached())  // ensure channel is valid
  {
    if (value < microSecondsToTicks(_minMicroSeconds))  // ensure ticks are in range
      value = microSecondsToTicks(_minMicroSeconds);
    else if (value > microSecondsToTicks(_maxMicroSeconds))
      value = microSecondsToTicks(_maxMicroSeconds);
    this->_ticks = value;
    // do the actual write
    pwm.write(this->_ticks);
  }
}

void I0Servo::release() {
  if (this->attached())  // ensure channel is valid
    pwm.write(0);
}

float I0Servo::readAngle() {
  return microSeconds2Angle(readMicroseconds(), _minMicroSeconds, _maxMicroSeconds, _minAngle, _maxAngle, _direction);
}

int I0Servo::readMicroseconds() {
  int pulsewidthUsec;
  if (this->attached()) {
    pulsewidthUsec = ticksToMicroSeconds(this->_ticks);
  } else {
    pulsewidthUsec = 0;
  }

  return (pulsewidthUsec);
}

int I0Servo::readTicks() {
  return this->_ticks;
}

bool I0Servo::attached() {
  return (pwm.attached());
}

void I0Servo::setDirection(Direction direction) {
  _direction = direction;
}

void I0Servo::setTimerWidth(int value) {
  // only allow values between 10 and 14 for ESP32-C3
  // only allow values between 16 and 20 for other ESP32
  if (value < MINIMUM_TIMER_WIDTH)
    value = MINIMUM_TIMER_WIDTH;
  else if (value > MAXIMUM_TIMER_WIDTH)
    value = MAXIMUM_TIMER_WIDTH;

  // Fix the current ticks value after timer width change
  // The user can reset the tick value with a write() or writeUs()
  int widthDifference = this->_timer_width - value;
  // if positive multiply by diff; if neg, divide
  if (widthDifference > 0) {
    this->_ticks = widthDifference * this->_ticks;
  } else if (widthDifference < 0) {
    this->_ticks = this->_ticks / -widthDifference;
  }

  this->_timer_width = value;
  this->_timer_width_ticks = pow(2, this->_timer_width);

  // If this is an attached servo, clean up
  if (this->attached()) {
    // detach, setup and attach again to reflect new timer width
    pwm.detachPin(this->_pinNumber);
    pwm.attachPin(this->_pinNumber, REFRESH_CPS, this->_timer_width);
  }
}

int I0Servo::readTimerWidth() {
  return (this->_timer_width);
}

int I0Servo::microSecondsToTicks(int microSeconds) {
  return (int)((float)microSeconds / ((float)REFRESH_USEC / (float)this->_timer_width_ticks) * (((float)REFRESH_CPS) / (float)PWM_FREQUENCY));
}

int I0Servo::ticksToMicroSeconds(int ticks) {
  return (int)((float)ticks * ((float)REFRESH_USEC / (float)this->_timer_width_ticks) / (((float)REFRESH_CPS) / (float)PWM_FREQUENCY));
}
