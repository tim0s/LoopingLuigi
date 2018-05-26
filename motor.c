#define MOTOR_ENABLE_PIN 5
#define H_BRIDGE_A 6
#define H_BRIDGE_B 7


void set_motor_speed_and_direction(int spd, int dir) {
  // spd should be between 0 and 100
  // dir can be zero or one
  spd =  spd * 25.5;
  if (spd > 255) spd = 255;
  if (spd < 0) spd = 0;
  
  
  // turn of the motor
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  
  if (dir == 0) {
      // NEVER SET BOTH TO THE SAME VALUE WHILE MOTOR IS ON!
      digitalWrite(H_BRIDGE_A, LOW);
      digitalWrite(H_BRIDGE_B, HIGH);
  }
  else {
    // NEVER SET BOTH TO THE SAME VALUE WHILE MOTOR IS ON!
    digitalWrite(H_BRIDGE_A, HIGH);
    digitalWrite(H_BRIDGE_B, LOW);
  }
  analogWrite(MOTOR_ENABLE_PIN, 120);
  delay(200);
  analogWrite(MOTOR_ENABLE_PIN, spd);
  
}


void setup()
{
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(H_BRIDGE_A, OUTPUT);
  pinMode(H_BRIDGE_B, OUTPUT);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
}

void loop()
{
  for (int spd=0; spd<100; spd += 10) {
    set_motor_speed_and_direction(spd, 1);
    delay(3000);
  }
  
}
