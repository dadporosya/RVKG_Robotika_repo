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
      pinMode(PIN, INPUT_PULLUP);

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
      pinMode(PIN, 1);
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

class Diode{
  private:
    int PIN;
    int currentValue=0;
  public:
    void Init(int PIN_IN){
      PIN = PIN_IN;
      pinMode(PIN, OUTPUT);
    }

    void TurnOn(){
      digitalWrite(PIN, 1);
    }

    void TunOff(){
      digitalWrite(PIN, 0);
    }

    void ChangeState(){
      digitalWrite(PIN, !digitalRead(PIN));
    }
    
    void Set(int value){
      currentValue = value;
      analogWrite(PIN, currentValue);
    }

    void Change(int value){
      currentValue += value;
      if (currentValue > 255) currentValue = 255;
      else if (currentValue < 0) currentValue = 0;
      analogWrite(PIN, currentValue);
    }
};

class Beep{
  private:
    int PIN;
    int currentValue=0;
  public:
    void Init(int PIN_IN){
      PIN = PIN_IN;
      pinMode(PIN, OUTPUT);
    }

    void TurnOn(){
      digitalWrite(PIN, 1);
    }

    void TunOff(){
      digitalWrite(PIN, 0);
    }

    void ChangeState(){
      digitalWrite(PIN, !digitalRead(PIN));
    }
    
    void Set(int value){
      currentValue = value;
      analogWrite(PIN, currentValue);
    }

    void Change(int value){
      currentValue += value;
      if (currentValue > 255) currentValue = 255;
      else if (currentValue < 0) currentValue = 0;
      analogWrite(PIN, currentValue);
    }
};

class Game {
  private:
    float currentTime;

    // Start beep settings
    int preparationBeepPitch = 3;
    int beepsBeforeStart=3;
    int currentBeepCount=0;
    float beepDuration=1000;
    float gapBetweenBeeps=1000;

    long beepingStartTime=0;

    int readyStatePitch = 70;

    bool enableBeep=true; // ENABLE BEEP

    int DIODE_RED;
    int DIODE_YELLOW;
    int DIODE_GREEN;

  public:
    String state = "not active"; // not active, preparation
    
    int BEEP_CENTRAL_PIN;



    void Init(
      int BEEP_CENTRAL_PIN_IN,
      int DIODE_RED_IN,
      int DIODE_YELLOW_IN,
      int DIODE_GREEN_IN
    ){
      BEEP_CENTRAL_PIN = BEEP_CENTRAL_PIN_IN;
      int DIODE_RED = DIODE_RED_IN;
      int DIODE_YELLOW = DIODE_YELLOW_IN;
      int DIODE_GREEN = DIODE_GREEN_IN;
      SetFinished();
    }

    void Update(){
      currentTime = millis();

      if (IsPreparation()){
        PreparationCoroutine(currentTime - beepingStartTime);
      } else if (IsReady()){
        ReadyStateCoroutine();
      }

    }

    void StartGame(){
      
      // analogWrite(BEEP_CENTRAL_PIN, preparationBeepPitch);
      SetPreparation();
    }

    void PreparationCoroutine(long dTime){
      if (dTime < 0) dTime = currentTime;

      if (currentBeepCount > beepsBeforeStart){
        SetReady();
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

    void ReadyStateCoroutine(long dTime=-1){
      // Serial.println(state);
      if (dTime < 0) dTime = currentTime;
    }

    void SetPreparation(){
      ChangeState("preparation");

      currentBeepCount=0;
      digitalWrite(DIODE_RED, !digitalRead(DIODE_RED));
      Serial.println(!digitalRead(DIODE_RED));
      
    }

    bool IsPreparation(){
      return state == "preparation";
    }

    void SetReady(){
      ChangeState("ready");

      if (enableBeep) Beep(readyStatePitch);

      digitalWrite(DIODE_RED, !digitalRead(DIODE_RED));
      Serial.println(digitalRead(DIODE_RED));
      
    }

    bool IsReady(){
      return state == "ready";
    }

    void SetFinished(){
      Beep(0);
      digitalWrite(DIODE_RED, !digitalRead(DIODE_RED));
      state = "finished";
    }

    bool IsFinished(){
      return state == "finished";
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
///      VARIABLES AND ETC
///          ↓↓↓↓↓↓↓
int START_GAME_BTN_PIN = 13;
int PLAYER1_BTN_PIN = 12;
int PLAYER2_BTN_PIN = 11;

int MOTOR1_PIN = 3;
int MOTOR2_PIN = 4;

int BEEP1_PIN = 9;

int DIODE_RED = 7;
int DIODE_YELLOW = 6;
int DIODE_GREEN = 5;

Button gameBtn;
Motor motor1;

Button player1Btn;
Button player2Btn;

Game game;

void Empty(){

}

void OnEnterGameButton(){
  // // motor1.SetSpeed(255);
  // if (motor1.isActive()){
  //   motor1.EndMovement();
  //   // analogWrite(BEEP1_PIN, 0);
  //   return;
  // }

  // // analogWrite(BEEP1_PIN, 1);
  // motor1.StartMovement(255, 1000);

  if (!game.IsFinished()) return;

  game.StartGame();
}

void OnEnterPlayer(int playerBtnPin=-1){
  if (playerBtnPin < 0) return;

  if (game.IsReady()){
    PlayerLoose(playerBtnPin);
  } else if (game.IsPreparation()){
    PlayerWin(playerBtnPin);
  }
}

void PlayerWin(int PLAYER_PIN){
  game.SetFinished();
  Serial.println(PLAYER_PIN);
  Serial.println("LOSE!!!");
}

void PlayerLoose(int PLAYER_PIN){
  game.SetFinished();
  Serial.println(PLAYER_PIN);
  Serial.println("WINS!!!");
}

void OnEnterPlayer1(){
  OnEnterPlayer(PLAYER1_BTN_PIN);
}

void OnEnterPlayer2(){
  OnEnterPlayer(PLAYER2_BTN_PIN);
}
///           ↑↑↑↑↑↑↑
///      VARIABLES AND ETC
/// ============================


/// ============================
///           SETUP
///          ↓↓↓↓↓↓↓
void setup() {
  test();

  Serial.begin(9600);
  // pinMode(MOTOR1_PIN, 1); // motor 1
  // pinMode(BEEP1_PIN, OUTPUT);

  //DIODES
  // pinMode(DIODE_RED, OUTPUT);
  // pinMode(DIODE_YELLOW, OUTPUT);
  // pinMode(DIODE_GREEN, OUTPUT);

  //BUTTONS
  // pinMode(START_GAME_BTN_PIN, INPUT_PULLUP); // button 1
  // pinMode(PLAYER1_BTN_PIN, INPUT_PULLUP); // button 1
  // pinMode(PLAYER2_BTN_PIN, INPUT_PULLUP); // button 1

  //INITS
  gameBtn.Init(
    START_GAME_BTN_PIN,
    Empty,
    OnEnterGameButton,
    Empty,
    Empty
  );

  player1Btn.Init(
    PLAYER1_BTN_PIN,
    Empty,
    OnEnterPlayer1,
    Empty,
    Empty
  );

  player2Btn.Init(
    PLAYER2_BTN_PIN,
    Empty,
    OnEnterPlayer2,
    Empty,
    Empty
  );

  motor1.Init(MOTOR1_PIN);

  game.Init(
    BEEP1_PIN,
    DIODE_RED,
    DIODE_YELLOW,
    DIODE_GREEN
    );
}
///           ↑↑↑↑↑↑↑
///            SETUP
/// ============================


/// ============================
///          MAIN LOOP
///          ↓↓↓↓↓↓↓↓

void loop() {
  gameBtn.Update();
  player1Btn.Update();
  player2Btn.Update();
  motor1.Update();
  game.Update();  

  digitalWrite(DIODE_RED, HIGH);
  Serial.println(digitalRead(DIODE_RED));
  // Serial.println(DIODE_RED);
}
///          ↑↑↑↑↑↑↑↑
///          MAIN LOOP
/// ============================

