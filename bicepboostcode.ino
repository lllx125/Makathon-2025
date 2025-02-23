#include <ezButton.h>

#define VRY_PIN  A0 // Arduino pin connected to VRY pin
#define SW_PIN   2  // Arduino pin connected to SW  pin
#define ENB 10 // Arduino pin to connected to ENB pin
#define motor1pin1 4
#define motor1pin2 5
#define UP_LIMIT_PIN 6
#define LOW_LIMIT_PIN 7 

ezButton jt_button(SW_PIN); // joystick button
ezButton upper_limit(UP_LIMIT_PIN); // upper limit switch
ezButton lower_limit(LOW_LIMIT_PIN); // lower limit switch

int yValue = 0; // To store value of the Y axis
int bValue = 0; // To store value of the button
int bState = 0; // To store state of button
int pwm = 200;
int clickCount = 0; // to check for double click
int speedState = 1;

void setup() {
  Serial.begin(9600) ;
  jt_button.setDebounceTime(50); // set debounce time to 50 milliseconds
  upper_limit.setDebounceTime(50);
  lower_limit.setDebounceTime(50);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(ENB, OUTPUT); // pin for speed control
}

void loop() {
  joystick();
}

void joystick(){
  jt_button.loop(); // MUST call the loop() function first
  
  // read analog Y analog values
  yValue = analogRead(VRY_PIN);

  // Read the button value
  bValue = jt_button.getState();

  while(750<yValue && yValue<1050){
    Serial.println("Up Y");
    driveUp();
    yValue = analogRead(VRY_PIN);
    limitSwitch();
  }
  stopMotor();

  while(-50<yValue && yValue<150){
    Serial.println("Down Y");
    Serial.print(pwm);
    driveDown();
    yValue = analogRead(VRY_PIN);
    limitSwitch();
  }
  stopMotor();
  
  if (bValue != bState) {
    bState = bValue;
    if (bValue == 0){
      clickCount += 1;
      if (clickCount>1){
        pwm = changeSpeed();
        clickCount = 0;
      }
    }
  }
}

void driveUp(){
  analogWrite(ENB, pwm); //PWM to control speed (min=0, max=255) 
  digitalWrite(motor1pin1,  HIGH);
  digitalWrite(motor1pin2, LOW);
}

void driveDown(){
  analogWrite(ENB, pwm); //PWM to control speed (min=0, max=255) 
  digitalWrite(motor1pin1,  LOW);
  digitalWrite(motor1pin2, HIGH);
}

int changeSpeed(){
  if (speedState==0){
    speedState=1;
    return 200;
  }
  else if(speedState==1){
    speedState=2;
    return 230;
  }
  else{
    speedState=0;
    return 255;
  }
}

void stopMotor(){
  digitalWrite(motor1pin1,  LOW);
  digitalWrite(motor1pin2, LOW);
}

void limitSwitch(){
  upper_limit.loop();
  lower_limit.loop();

  int upperState = upper_limit.getState();
  int lowerState = lower_limit.getState();

  if(upperState == 0){
    Serial.print("GO DOWN");
    stopMotor();
    delay(10);
    driveDown();
    delay(200);
  }

  if(lowerState == 0){
    Serial.print("GO UP");
    stopMotor();
    delay(10);
    driveUp();
    delay(200);
  }

  delay(100);
}