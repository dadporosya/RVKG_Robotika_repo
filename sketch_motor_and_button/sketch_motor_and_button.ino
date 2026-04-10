class Button {
  private:
    int PIN;
    bool hold = false;

    void (*onLow)();
    void (*onEnteringLow)();

    void (*onUp)();
    void (*onEnteringUp)();

  public:
    void Init(
        int pinIn,
        void (*lowIn)(void*),
        void (*enterLowIn)(void*),
        void (*upIn)(void*),
        void (*enterUpIn)(void*)
    ){
      PIN = pinIn;

      onLow = lowIn;
      onEnteringLow = enterLowIn;

      onUp = upIn;
      onEnteringUp = enterUpIn;

    }

    // void Init(
    //     int pinIn,
    //     setup()::<lambda()>,
    //     setup()::<lambda()>,
    //     setup()::<lambda()>,
    //     setup()::<lambda()>
    // ){
    //   PIN = pinIn;

    //   onLow = lowIn;
    //   onEnteringLow = enterLowIn;

    //   onUp = upIn;
    //   onEnteringUp = enterUpIn;
    // }

    void Update(){
      if (!IsPressed()) {
        if (!hold) {
          if (onUp) onUp();
          return;
        }
        hold = false;
        if (onEnteringUp) onEnteringUp();
        return;
      }

      if (hold){
        if (onLow) onLow();
        return;
      }

      hold = true;
      if (onEnteringLow) onEnteringLow();
      return;

    }

    bool IsPressed(){
      return digitalRead(PIN) == LOW;
    }
};



/// ============================
///           SETUP
///          ↓↓↓↓↓↓↓
int BTN1_PIN = 13;
int MOTOR1_PIN = 3;

void OnLowTemp(){
  // Serial.println("Low");
}

void OnEnterLowTemp(){
  analogWrite(MOTOR1_PIN, 255);
}

void OnUpTemp(){
  // Serial.println("Up");
}

void OnEnterUpTemp(){
  analogWrite(MOTOR1_PIN, 0);
}

Button btn1;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR1_PIN, 1); // motor 1
  pinMode(BTN1_PIN, INPUT_PULLUP); // button 1

  btn1.Init(
    BTN1_PIN,
    OnLowTemp,
    OnEnterLowTemp,
    OnUpTemp,
    OnEnterUpTemp
  );

  // btn1.Init(
  //   BTN1_PIN,
  //   [](){ Serial.println("Low"); },
  //   [](){ analogWrite(MOTOR1_PIN, 255); },
  //   [](){ Serial.println("Up"); },
  //   [](){ analogWrite(MOTOR1_PIN, 0); }
  // );

}
///           ↑↑↑↑↑↑↑
///            SETUP
/// ============================

/// ============================
///          MAIN LOOP
///          ↓↓↓↓↓↓↓↓


void loop() {
  
  // if (digitalRead(BTN1_PIN) == LOW){
  //   analogWrite(MOTOR1_PIN, 255);
  // } else {
  //   analogWrite(MOTOR1_PIN, 0);
  // }

  btn1.Update();
}
///          ↑↑↑↑↑↑↑↑
///          MAIN LOOP
/// ============================

