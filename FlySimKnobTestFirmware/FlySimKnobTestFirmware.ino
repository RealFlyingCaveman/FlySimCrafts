#include <Adafruit_NeoPixel.h>

#define LED_PIN     16
#define NUMPIXELS   1

#define OUTER_A     2
#define OUTER_B     3
#define INNER_A     4
#define INNER_B     5
#define PUSH_PIN    6

#define ENCODER_THRESHOLD 2

Adafruit_NeoPixel pixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int lastOuterState;
int lastInnerState;
int outerAccumulator = 0;
int innerAccumulator = 0;

long outerPosition = 0;
long innerPosition = 0;
long outerCW = 0;
long outerCCW = 0;
long innerCW = 0;
long innerCCW = 0;
long buttonPresses = 0;

bool lastButton = HIGH;

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
}

int readEncoderState(int pinA, int pinB) {
  return (digitalRead(pinA) << 1) | digitalRead(pinB);
}

int decodeEncoder(int pinA, int pinB, int &lastState, int &accumulator) {
  int state = readEncoderState(pinA, pinB);
  if (state == lastState) return 0;

  int transition = (lastState << 2) | state;

  if (transition == 0b0001 || transition == 0b0111 ||
      transition == 0b1110 || transition == 0b1000) {
    accumulator++;
  }
  else if (transition == 0b0010 || transition == 0b1011 ||
           transition == 0b1101 || transition == 0b0100) {
    accumulator--;
  }

  lastState = state;

  if (accumulator >= ENCODER_THRESHOLD) {
    accumulator = 0;
    return 1;
  }

  if (accumulator <= -ENCODER_THRESHOLD) {
    accumulator = 0;
    return -1;
  }

  return 0;
}

void printStatus() {
  Serial.println();
  Serial.println("----- Current Counts -----");
  Serial.print("Outer Position: ");
  Serial.print(outerPosition);
  Serial.print(" | CW: ");
  Serial.print(outerCW);
  Serial.print(" | CCW: ");
  Serial.println(outerCCW);

  Serial.print("Inner Position: ");
  Serial.print(innerPosition);
  Serial.print(" | CW: ");
  Serial.print(innerCW);
  Serial.print(" | CCW: ");
  Serial.println(innerCCW);

  Serial.print("Button Presses: ");
  Serial.println(buttonPresses);
  Serial.println("--------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  pixel.begin();

  pinMode(OUTER_A, INPUT_PULLUP);
  pinMode(OUTER_B, INPUT_PULLUP);
  pinMode(INNER_A, INPUT_PULLUP);
  pinMode(INNER_B, INPUT_PULLUP);
  pinMode(PUSH_PIN, INPUT_PULLUP);

  lastOuterState = readEncoderState(OUTER_A, OUTER_B);
  lastInnerState = readEncoderState(INNER_A, INNER_B);

  setColor(0, 255, 0);

  Serial.println();
  Serial.println("==================================");
  Serial.println(" FlySimCrafts Factory Test v4");
  Serial.println(" Dual Encoder + Button Diagnostics");
  Serial.println("==================================");
  Serial.println("Outer: GP2/GP3");
  Serial.println("Inner: GP4/GP5");
  Serial.println("Button: GP6");
  Serial.println("Threshold: 2");
  Serial.println();
}

void loop() {
  int outerMove = decodeEncoder(OUTER_A, OUTER_B, lastOuterState, outerAccumulator);

  if (outerMove == 1) {
    outerPosition++;
    outerCW++;
    Serial.print("Outer CW  | Position: ");
    Serial.println(outerPosition);
  }
  else if (outerMove == -1) {
    outerPosition--;
    outerCCW++;
    Serial.print("Outer CCW | Position: ");
    Serial.println(outerPosition);
  }

  int innerMove = decodeEncoder(INNER_A, INNER_B, lastInnerState, innerAccumulator);

  if (innerMove == 1) {
    innerPosition++;
    innerCW++;
    Serial.print("Inner CW  | Position: ");
    Serial.println(innerPosition);
  }
  else if (innerMove == -1) {
    innerPosition--;
    innerCCW++;
    Serial.print("Inner CCW | Position: ");
    Serial.println(innerPosition);
  }

  bool button = digitalRead(PUSH_PIN);

  if (button != lastButton) {
    delay(15);
    button = digitalRead(PUSH_PIN);

    if (button != lastButton) {
      lastButton = button;

      if (button == LOW) {
        buttonPresses++;
        Serial.print("Button Pressed | Count: ");
        Serial.println(buttonPresses);
        setColor(255, 255, 255);
        printStatus();
      } else {
        Serial.println("Button Released");
        setColor(0, 255, 0);
      }
    }
  }
}