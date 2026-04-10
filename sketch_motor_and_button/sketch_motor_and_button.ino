int BTN1_PIN = 7;
int MOTOR1_PIN = 3;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR1_PIN, 1); // motor 1
  pinMode(BTN1_PIN, INPUT_PULLUP); // button 1
}

String velocityInput="";
void loop() {
  
  if (IsBtnPressed(BTN1_PIN)){
    analogWrite(MOTOR1_PIN, 255);
  } else {
    analogWrite(MOTOR1_PIN, 0);
  }

  if (Serial.available()) { // if there was input
    char c =Serial.read();
    if (c!=10){
      velocityInput+=c;
    } else {
      analogWrite(3, velocityInput.toInt());

      Serial.println("Velocity: ");
      Serial.println(velocityInput);
      velocityInput="";
    }
  }
}

bool IsBtnPressed(int btnPin){
  /// :param: int pin of the button
  /// :return: true, if pressed. Else: false
  
  return digitalRead(btnPin)==LOW;
}
