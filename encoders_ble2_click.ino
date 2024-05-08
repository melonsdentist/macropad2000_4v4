#include <BleKeyboard.h>
#include <Encoder.h>
const uint8_t LEFT_BRACKET = 0x5B;   // [
const uint8_t RIGHT_BRACKET = 0x5D;  // ]
const uint8_t EQUALS = 0x3D;         // =
const uint8_t MINUS = 0x2D;          // -
const uint8_t SPACE = 0x20;          //
const uint8_t CHAR_E = 0x65;         // e
const uint8_t CHAR_I = 0x69;         // i
const uint8_t CHAR_K = 0x6B;         // k
const uint8_t CHAR_L = 0x6C;         // l
const uint8_t CHAR_M = 0x6D;         // m
const uint8_t CHAR_O = 0x6F;         // o
const uint8_t CHAR_S = 0x73;         // s
const uint8_t CHAR_Z = 0x7A;         // z

class MEncoder {
private:
  Encoder e;
  uint8_t out1, out2;
  long prevValue;
public:
  MEncoder()
    : e(0, 1){};
  MEncoder(int pin1, int pin2, uint8_t a, uint8_t b)
    : e(pin1, pin2),
      out1(a),
      out2(b),
      prevValue(0) {}
  uint8_t process() {
    long value = e.read();
    int delta = value - prevValue;
    if (delta && abs(delta) >= 2) {  // delta >= 2 to make one pulse on one step, by default there are 2
      Serial.print("In encoder ");
      Serial.println(out1);
      uint8_t symbol = delta > 0 ? out1 : out2;
      prevValue = value;
      return symbol;
    } else return 0;
  }
};
struct mAction {
  String type;
  uint8_t key1;
  uint8_t key2;
  mAction(String type, uint8_t key1, uint8_t key2)
    : type(type),
      key1(key1),
      key2(key2) {}
};
struct mButton {
  int n;  // number of bit in data byte
  mAction action;
  bool pressed = false;
  bool changed = false;
  mButton(int n, String type, uint8_t aKey1, uint8_t aKey2 = 0)
    : action(type, aKey1, aKey2),
      n(n) {}
};
MEncoder
  e1(5, 6, EQUALS, MINUS),
  e2(7, 8, CHAR_K, CHAR_L),
  e3(9, 10, CHAR_I, CHAR_O),
  e4(0, 1, LEFT_BRACKET, RIGHT_BRACKET);
mButton
  b1(0, "Combo", KEY_LEFT_CTRL, CHAR_Z),
  b2(1, "Press", CHAR_E),
  b3(2, "Press", SPACE),
  b4(3, "Press", KEY_LEFT_CTRL),
  b5(4, "Press", KEY_LEFT_SHIFT),
  b6(5, "Combo", KEY_LEFT_CTRL, CHAR_S),
  b7(6, "Press", CHAR_M),
  b8(7, "Combo", KEY_LEFT_SHIFT, CHAR_I);
MEncoder *encoders[4] = { &e1, &e2, &e3, &e4 }; // somehow first encoder will not work in loop without pointers
mButton btns[8] = { b1, b2, b3, b4, b5, b6, b7, b8 };

BleKeyboard bleKeyboard("macropad_2000");

const int dataPin = 2;  /* Q7 #9 */
const int clockPin = 3; /* CP #2 */
const int latchPin = 4; /* PL #1 */

void setup() {
  // Setup 74HC165 connections
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  bleKeyboard.begin();
  Serial.begin(115200);
  Serial.println("Started");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    for (int i = 0; i < 4; i++) {
      uint8_t x = encoders[i]->process();
      if (x) bleKeyboard.write(x);
    }
    //process btn actions
    readButtons();
    for (int i = 0; i < 8; i++) {
      if (btns[i].changed) {
        Serial.print("Button ");
        Serial.print(i);
        Serial.println(btns[i].pressed ? " pressed" : " released");
        if (btns[i].action.type == "Press") {
          if (btns[i].pressed) bleKeyboard.press(btns[i].action.key1);
          else bleKeyboard.release(btns[i].action.key1);
        } else if (btns[i].action.type == "Combo") {
          if (btns[i].pressed) {
            bleKeyboard.press(btns[i].action.key1);
            bleKeyboard.press(btns[i].action.key2);
          } else bleKeyboard.releaseAll();  // or one by one
        }
      }
    }
  }
}

void readButtons() {
  // get values from shift register and update btn structs states accordingly
  // Step 1: Sample
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
  // Step 2: Shift
  uint8_t x = ~shiftIn(dataPin, clockPin, MSBFIRST);

  for (int i = 0; i < 8; i++) {
    bool reading = bitRead(x, btns[i].n);
    if (reading != btns[i].pressed) {
      btns[i].pressed = reading;
      btns[i].changed = true;
    } else btns[i].changed = false;
  }
}
