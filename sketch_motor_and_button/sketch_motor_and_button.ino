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

    int defaultSpeed = 200;
    float DEG_CONST= (200 * 200) / 90; // CONST. 200 * 200 ~= 90 deg.  t = s / v. t = 90 / 200; 1 deg ~= 200 * 200 / 90; m // TODO

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

    void MoveDegrees(float degrees){
      StartMovement(defaultSpeed, durationSecIn:);
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
      Serial.println("TURN ON " + String(PIN));
      digitalWrite(PIN, 1);
    }

    void TurnOff(){
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

    void TurnOff(){
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

    void GetValue(){
      // Serial.println("analog: ", analogRead);
    }
};

class Game {
  private:
    float currentTime;

    // Start beep settings
    int preparationBeepPitch = 3;

    int beepsBeforeStart=3;
    int currentBeepCount=0;
    int maxBeepCount=5;

    float defaultBeepDuration=1000;
    float beepDurationDistribution=250;
    float currentBeepDuration=1000;

    long beepingStartTime=0;

    int readyStatePitch = 70;

    bool enableBeep = false; // ENABLE BEEP

    Diode DIODE_RED;
    Diode DIODE_YELLOW;
    Diode DIODE_GREEN;

    Beep beep;

  public:
    String state = "not active"; // not active, preparation
    
    void Init(
      Beep BEEP_CENTRAL_PIN_IN,
      Diode DIODE_RED_IN,
      Diode DIODE_YELLOW_IN,
      Diode DIODE_GREEN_IN
    ){
      beep = BEEP_CENTRAL_PIN_IN;
      DIODE_RED = DIODE_RED_IN;
      DIODE_YELLOW = DIODE_YELLOW_IN;
      DIODE_GREEN = DIODE_GREEN_IN;
      SetFinished();
    }

    void Update(){
      currentTime = millis();

      // DIODE_RED.TurnOn();

      if (IsPreparation()){
        PreparationCoroutine(currentTime - beepingStartTime);
      } else if (IsReady()){
        ReadyStateCoroutine();
      }

    }

    void StartGame(){
      
      // analogWrite(BEEP_CENTRAL, preparationBeepPitch);
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
      if (dTime > currentBeepDuration + currentBeepDuration){
        currentBeepCount++;

        beepingStartTime = currentTime;
        if (enableBeep) Beep(preparationBeepPitch+currentBeepCount);
        DIODE_YELLOW.TurnOn();
        Serial.println("----BEEP----");
        
        
      } else if (dTime > currentBeepDuration){
        Serial.println("----STOP---");
        if (enableBeep) Beep(0);
        DIODE_YELLOW.TurnOff();
      }
    }

    void ReadyStateCoroutine(long dTime=-1){
      // Serial.println(state);
      if (dTime < 0) dTime = currentTime;
    }

    void SetPreparation(){
      ChangeState("preparation");

      currentBeepCount=0;
      DIODE_RED.TurnOn();
      
      currentBeepDuration = defaultBeepDuration + random(-beepDurationDistribution, beepDurationDistribution);
      beepsBeforeStart = random(1, maxBeepCount+1);

    }

    bool IsPreparation(){
      return state == "preparation";
    }

    void SetReady(){
      ChangeState("ready");

      if (enableBeep) Beep(readyStatePitch);

      DIODE_RED.TurnOff();
      DIODE_YELLOW.TurnOff();
      DIODE_GREEN.TurnOn();
      
    }

    bool IsReady(){
      return state == "ready";
    }

    void SetFinished(){
      Beep(0);
      DIODE_RED.TurnOff();
      DIODE_YELLOW.TurnOff();
      DIODE_GREEN.TurnOff();

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
      beep.Set(pitch);
    }

};

/// ============================
///      VARIABLES AND ETC
///          ↓↓↓↓↓↓↓
int START_GAME_BTN_PIN = 13;

int PLAYER1_BTN_PIN = 12;
int P1_DIODE_RED = 6;
int P1_DIODE_GREEN = 5;

int PLAYER2_BTN_PIN = 11;
int P2_DIODE_RED = 8;
int P2_DIODE_GREEN = 7;


int MOTOR_PIN = 9;
// int MOTOR2_PIN = 3;

int CENTRAL_BEEP_PIN = 10;

int DIODE_RED_PIN = 4;
int DIODE_YELLOW_PIN = 3;
int DIODE_GREEN_PIN = 2;

Button gameBtn;

// motor.StartMovement(201, 200); ~= 90 deg
Motor motor;

Button player1Btn;
Diode P1_RED;
Diode P1_GREEN;

Button player2Btn;
Diode P2_RED;
Diode P2_GREEN;

Diode DIODE_RED;
Diode DIODE_YELLOW;
Diode DIODE_GREEN;

Beep CENTRAL_BEEP;

Game game;

void Empty(){

}

void OnEnterGameButton(){
  // motor1.SetSpeed(255);
  if (motor.isActive()){
    motor.EndMovement();
    // analogWrite(BEEP1_PIN, 0);
    return;
  }

  // analogWrite(BEEP1_PIN, 1);
  motor.StartMovement(201, 200);

  if (!game.IsFinished()) return;

  ResetAllPlayers();
  game.StartGame();
}

void OnEnterPlayer(
  int playerBtnPin, Diode playerDiodeWin, Diode playerDiodeLose,
  int opponentBtnPin, Diode opponentDiodeWin, Diode opponentDiodeLose
  ){
  if (playerBtnPin < 0) return;
  if (game.IsFinished()) return;

  ResetAllPlayers();

  if (game.IsReady()){
    PlayerWin(playerBtnPin, playerDiodeWin);
    PlayerLose(opponentBtnPin, opponentDiodeLose);
    
  } else if (game.IsPreparation()){
    PlayerLose(playerBtnPin, playerDiodeLose);
    PlayerWin(opponentBtnPin, opponentDiodeWin);

  }
}

void ResetPlayer(Diode diodeWin, Diode diodeLose){
  diodeWin.TurnOff();
  diodeLose.TurnOff();
}

void ResetAllPlayers(){
  ResetPlayer1();
  ResetPlayer2();
}

void ResetPlayer1(){
  ResetPlayer(P1_GREEN,  P1_RED);
}

void ResetPlayer2(){
  ResetPlayer(P2_GREEN,  P2_RED);
}

void PlayerWin(
  int PLAYER_PIN,
  Diode diode
  ){
  game.SetFinished();
  Serial.println(PLAYER_PIN);
  Serial.println("LOSE!!!");
  diode.TurnOn();
}

void PlayerLose(
  int PLAYER_PIN,
  Diode diode
  ){
  game.SetFinished();
  Serial.println(PLAYER_PIN);
  Serial.println("WINS!!!");
  diode.TurnOn();
}

void OnEnterPlayer1(){
  OnEnterPlayer(PLAYER1_BTN_PIN, P1_GREEN,  P1_RED, PLAYER2_BTN_PIN, P2_GREEN,  P2_RED);
}

void OnEnterPlayer2(){
  OnEnterPlayer(PLAYER2_BTN_PIN, P2_GREEN,  P2_RED, PLAYER1_BTN_PIN, P1_GREEN,  P1_RED);
}
///           ↑↑↑↑↑↑↑
///    VARIABLES, LAMBDAS, ETC
/// ============================


/// ============================
///           SETUP
///          ↓↓↓↓↓↓↓
void setup() {
  Serial.begin(9600);

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
  P1_RED.Init(P1_DIODE_RED);
  P1_GREEN.Init(P1_DIODE_GREEN);

  player2Btn.Init(
    PLAYER2_BTN_PIN,
    Empty,
    OnEnterPlayer2,
    Empty,
    Empty
  );
  P2_RED.Init(P2_DIODE_RED);
  P2_GREEN.Init(P2_DIODE_GREEN);

  motor.Init(MOTOR_PIN);

  DIODE_RED.Init(DIODE_RED_PIN);
  DIODE_YELLOW.Init(DIODE_YELLOW_PIN);
  DIODE_GREEN.Init(DIODE_GREEN_PIN);

  CENTRAL_BEEP.Init(CENTRAL_BEEP_PIN);

  game.Init(
    CENTRAL_BEEP,
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
  motor.Update();
  game.Update();  

  // digitalWrite(DIODE_RED, HIGH);
  // Serial.println(digitalRead(DIODE_RED));
  // Serial.println(DIODE_RED);

  // Serial.println();
}
///          ↑↑↑↑↑↑↑↑
///          MAIN LOOP
/// ============================

