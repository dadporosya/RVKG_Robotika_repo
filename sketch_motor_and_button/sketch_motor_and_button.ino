// #include "ButtonClass.ino"

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

class Motor {
  private:
    int PIN;

    int speed=0;

    long startTime;
    long currentTime;
    const long DISABLED=-1;
    

  public:
    long duration=DISABLED;

    void Init(
        int pinIn
    ){
      PIN = pinIn;
    }

    void Update(){
      if (duration == DISABLED) return;

      currentTime = millis();
      if (currentTime-startTime>= duration){
        EndMovement();
      }
    }

    void Move(){
      analogWrite(PIN, speed);
    }

    void SetSpeed(int value){
      speed = value;
      Move();
    }

    void ChangeSpeed(int value){
      speed += value;
      Move();
    }

    void StartMovement(int speedIn, float durationMiliIn=0, float durationSecIn=-1){
      SetSpeed(speedIn);
      startTime = millis();

      duration = durationSecIn > 0 ? durationSecIn/1000 : durationMiliIn;
    }

    void EndMovement(){
      duration=DISABLED;
      SetSpeed(0);
    }

    bool isActive(){
      return speed != 0;
    }
};

class Game {
  private:
    int beepsBeforeStart;
    int currentBeepCount=0;
  public:
    String state = "not active"; // not active, preparation
    
    void Init(){
      
    }

    void Update(){

    }

    void StartGame(){
      currentBeepCount=0;
      state = "preparation";
    }

    void PreparationCoroutine(){

    }

};

/// ============================
///           SETUP
///          ↓↓↓↓↓↓↓
int BTN1_PIN = 13;
int MOTOR1_PIN = 3;
int BEEP1_PIN = 9;

Button btn1;
Motor motor1;


void OnLowTemp(){

}

void OnEnterLowTemp(){
  // motor1.SetSpeed(255);
  if (motor1.isActive()){
    motor1.EndMovement();
    analogWrite(BEEP1_PIN, 0);
    return;
  }

  analogWrite(BEEP1_PIN, 1);
  motor1.StartMovement(255, 1000);
}

void OnUpTemp(){
  // Serial.println("Up");
}

void OnEnterUpTemp(){
  // motor1.SetSpeed(0);
}



     

   

void setup() {
  test();

  Serial.begin(9600);
  pinMode(MOTOR1_PIN, 1); // motor 1
  pinMode(BEEP1_PIN, OUTPUT);
  pinMode(BTN1_PIN, INPUT_PULLUP); // button 1

  btn1.Init(
    BTN1_PIN,
    OnLowTemp,
    OnEnterLowTemp,
    OnUpTemp,
    OnEnterUpTemp
  );

  motor1.Init(MOTOR1_PIN);
}
///           ↑↑↑↑↑↑↑
///            SETUP
/// ============================


/// ============================
///          MAIN LOOP
///          ↓↓↓↓↓↓↓↓

void loop() {
  btn1.Update();
  motor1.Update();  
  test();                                                                                                                                          
}
///          ↑↑↑↑↑↑↑↑
///          MAIN LOOP
/// ============================

