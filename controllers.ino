#define JOY1_PIN 5   // Up
#define JOY2_PIN 4   // Down
#define JOY3_PIN 2   // Left, Paddle X button
#define JOY4_PIN 3   // Right, Paddle Y button
#define JOY6_PIN 6   // Fire
#define POTX_PIN A0  // (JOY9) Paddle X Pot
#define POTY_PIN A1  // (JOY5) Paddle Y Pot
#define POTREF_PIN 8 // Output pin for paddle pot reference +5/0v

unsigned char pins2hat[16] = { 8,8,8,8,8,3,1,2,8,5,7,6,8,4,0,8 };

void setup() {
  Serial.begin(9600);
  pinMode(JOY1_PIN, INPUT_PULLUP);
  pinMode(JOY2_PIN, INPUT_PULLUP);
  pinMode(JOY3_PIN, INPUT_PULLUP);
  pinMode(JOY4_PIN, INPUT_PULLUP);
  pinMode(JOY6_PIN, INPUT_PULLUP);
  pinMode(POTREF_PIN, OUTPUT);
  digitalWrite(POTREF_PIN, 0);
}

#define MINUS 0x4182
#define PLUS  0x2814
#define ZERO  0x8421
#define TERR  0x1248

struct encoder { int val, prev; } ex = { 0, 3 }, ey = {0, 3};
void mousedecode(int s1, int s2, struct encoder *e, int *err, int skip) {
  int s = s1 * 2 + s2;
  if (!skip && e->prev != s) {
    int code = e->prev * 4 + s;
    if ((PLUS >> code) & 1) e->val++;
    if ((MINUS >> code) & 1) e->val--;
    if ((TERR >> code) & 1) (*err)++;
  }
  e->prev = s;
}


#define PADMIN (580 * 16)
#define PADMAX (1023 * 16)
int padXVal, padYVal;
struct abuf { int pin; int val[16], pos, tot; } bPOTX = { POTX_PIN }, bPOTY = { POTY_PIN };
int bufAnalogRead(struct abuf *a) {
  int cur = analogRead(a->pin);
  a->tot += cur - a->val[a->pos];
  a->val[a->pos] = cur;
  a->pos = (a->pos + 1) % 16;
  return a->tot;
}
void loop() {
  static int lc = 0;
  static int paddlesConnected = 0;
  lc++;
  int buttonState1 = digitalRead(JOY1_PIN);
  int buttonState2 = digitalRead(JOY2_PIN);
  int buttonState3 = digitalRead(JOY3_PIN);
  int buttonState4 = digitalRead(JOY4_PIN);
  int buttonState6 = digitalRead(JOY6_PIN);
  static int potState1 = 0;
  static int potState2 = 0;
  static int buttonState5 = 1;
  static int buttonState9 = 1;
  if ((lc & 63) == 32) {
    potState1 = bufAnalogRead(&bPOTX);
    analogRead(POTY_PIN);
  } else if ((lc & 63) == 32 + 16) {
    potState2 = bufAnalogRead(&bPOTY);
    analogRead(POTX_PIN);
    paddlesConnected = potState1 > 255*16 && potState2 > 255*16;
    if (paddlesConnected) {
      padXVal = (potState1 < PADMIN ? 0 : potState1 - PADMIN) * 0xFFFFL / (PADMAX - PADMIN) - 0x8000L;
      padYVal = (potState2 < PADMIN ? 0 : potState2 - PADMIN) * 0xFFFFL / (PADMAX - PADMIN) - 0x8000L;
    } else {
      padXVal = padYVal = 0;
    }
    digitalWrite(POTREF_PIN, 1);
  } else if ((lc & 63) == 0) {
    buttonState9 = digitalRead(POTX_PIN);
    buttonState5 = digitalRead(POTY_PIN);
    digitalWrite(POTREF_PIN, 0);
  }
  static int skip = 1;

  static int eerr;
  mousedecode(buttonState1, buttonState3, &ey, &eerr, skip);
  mousedecode(buttonState2, buttonState4, &ex, &eerr, skip);
  skip = 0;
  int actual_buffersize = Serial.availableForWrite();
  if (actual_buffersize < 30) return;
  static int lc1 = 0;
  skip = 1;

  int xaxis, yaxis, buttons;
  if (paddlesConnected) {
    xaxis = padXVal;
    yaxis = padYVal;
    buttons = 3 ^ (buttonState3 + 2 * buttonState4);
  } else {
    xaxis = !buttonState3 ? -0x8000 : !buttonState4 ? 0x7FFF : 0;
    yaxis = !buttonState1 ? -0x8000 : !buttonState2 ? 0x7FFF : 0;
    buttons = 7 ^ (buttonState6 + 2 * buttonState9 + 4 * buttonState5);
  }

  Serial.print(lc - lc1);
  lc1 = lc;
  Serial.print(' ');

  static long ts0;
  long ts = millis();
  Serial.print(ts - ts0);
  Serial.print(' ');
  lc1 = lc;
  ts0 = ts;
  // Mouse
  Serial.print(ex.val);
  Serial.print(' ');
  Serial.print(ey.val);
  Serial.print(' ');  
  Serial.print(eerr);
  Serial.print(' ');
  ex.val = ey.val = 0;
    
  // Joy
  Serial.print(buttons);
  Serial.print(' ');
  Serial.print(xaxis);
  Serial.print(' ');
  Serial.println(yaxis);
}
