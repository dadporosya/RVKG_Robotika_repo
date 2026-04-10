// class Button {
//   private:
//     int PIN;
//     bool hold = false;

//     void (*onLow)();
//     void (*onEnteringLow)();

//     void (*onUp)();
//     void (*onEnteringUp)();

//   public:
//     void Init(
//         int pinIn,
//         void (*lowIn)(void*),
//         void (*enterLowIn)(void*),
//         void (*upIn)(void*),
//         void (*enterUpIn)(void*)
//     ){
//       PIN = pinIn;

//       onLow = lowIn;
//       onEnteringLow = enterLowIn;

//       onUp = upIn;
//       onEnteringUp = enterUpIn;

//     }

//     void Update(){
//       if (!IsPressed()) {
//         if (!hold) {
//           if (onUp) onUp();
//           return;
//         }
//         hold = false;
//         if (onEnteringUp) onEnteringUp();
//         return;
//       }

//       if (hold){
//         if (onLow) onLow();
//         return;
//       }

//       hold = true;
//       if (onEnteringLow) onEnteringLow();
//       return;

//     }

//     bool IsPressed(){
//       return digitalRead(PIN) == LOW;
//     }
// };

