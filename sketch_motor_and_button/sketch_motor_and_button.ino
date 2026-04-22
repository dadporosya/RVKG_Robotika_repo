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
    float currentTime;

    // Start beep settings
    int preparationBeepPitch = 10;
    int beepsBeforeStart=3;
    int currentBeepCount=0;
    float beepDuration=1000;
    float gapBetweenBeeps=1000;

    long beepingStartTime=0;

    int readyStatePitch = 50;

    bool enableBeep=false;

  public:
    String state = "not active"; // not active, preparation
    
    int BEEP_CENTRAL_PIN;



    void Init(int BEEP_CENTRAL_PIN_IN){
      BEEP_CENTRAL_PIN = BEEP_CENTRAL_PIN_IN;
    }

    void Update(){
      currentTime = millis();

      // switch(state){
      //   case "preparation":
      //     PreparationCoroutine();
      //     break;
      //   case "ready state":
      //     digitalWrite(BEEP_CENTRAL_PIN, startBeepPitch);
      //     break;
      // }
      // Serial.println(state);
      if (IsPreparation()){
        PreparationCoroutine(currentTime - beepingStartTime);
      } else if (IsReady()){
        ReadyStateCoroutine();
        // analogWrite(BEEP_CENTRAL_PIN, startBeepPitch);
      }

    }

    void StartGame(){
      currentBeepCount=0;
      // analogWrite(BEEP_CENTRAL_PIN, preparationBeepPitch);
      SetPreparation();
    }

    void PreparationCoroutine(long dTime){
        if (currentBeepCount > beepsBeforeStart){
          ChangeState("ready");
          return;
        }
        // long dTime = currentTime - beepingStartTime;
        Serial.println(dTime);
        if (dTime > beepDuration + gapBetweenBeeps){
          currentBeepCount++;

          beepingStartTime = currentTime;
          if (enableBeep) Beep(preparationBeepPitch);
          Serial.println("----BEEP----");
          
          
        } else if (dTime > beepDuration){
          Serial.println("----STOP---");
          if (enableBeep) Beep(0);
          
        }
    }

    void ReadyStateCoroutine(){
      // Serial.println(state);
      if (enableBeep) Beep(readyStatePitch);
    }

    void SetPreparation(){
      ChangeState("preparation");
    }

    bool IsPreparation(){
      return state == "preparation";
    }

    void SetReady(){
      ChangeState("ready");
    }

    bool IsReady(){
      return state == "ready"
    }

    void ChangeState(String stateIn){
      state = stateIn;
      Serial.println(state);
    }

    void Beep(int pitch=0){
      analogWrite(BEEP_CENTRAL_PIN, pitch);
    }

};

/// ============================
///           SETUP
///          ↓↓↓↓↓↓↓
int START_GAME_BTN_PIN = 13;
int PLAYER1_BTN_PIN = 12;
int PLAYER2_BTN_PIN = 11;

int MOTOR1_PIN = 3;
int MOTOR2_PIN = 4;

int BEEP1_PIN = 9;
int PLAYER1_BTN_PIN = 9;
int PLAYER2_BTN_PIN = 9;


Button btn1;
Motor motor1;

Game game;

void EmptyFunc(){

}

void OnLowTemp(){

}

void OnEnterLowTemp(){
  // // motor1.SetSpeed(255);
  // if (motor1.isActive()){
  //   motor1.EndMovement();
  //   // analogWrite(BEEP1_PIN, 0);
  //   return;
  // }

  // // analogWrite(BEEP1_PIN, 1);
  // motor1.StartMovement(255, 1000);

  game.StartGame();
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
  pinMode(START_GAME_PIN, INPUT_PULLUP); // button 1

  btn1.Init(
    START_GAME_PIN,
    EmptyFunc,
    OnEnterLowTemp,
    EmptyFunc,
    OnEnterUpTemp
  );

  motor1.Init(MOTOR1_PIN);

  game.Init(BEEP1_PIN);
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
  game.Update();  
}
///          ↑↑↑↑↑↑↑↑
///          MAIN LOOP
/// ============================

