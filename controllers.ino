#include <HID.h>
#define JOY1_PIN 2   // Up
#define JOY2_PIN 3   // Down
#define JOY3_PIN 4   // Left, Paddle X button
#define JOY4_PIN 5   // Right, Paddle Y button
#define JOY6_PIN 6   // Fire
#define POTX_PIN A0  // (JOY9) Paddle X Pot
#define POTY_PIN A3  // (JOY5) Paddle Y Pot
#define POTXREF_PIN 10 // Output pin for paddle pot reference +5/0v
#define POTYREF_PIN 14 // Output pin for paddle pot reference +5/0v

static const uint8_t hidDescriptorJoystick[] PROGMEM = {
  0x05, 0x01, // USAGE_PAGE (Generic Desktop)
  0x09, 0x05, // USAGE (Gamepad)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, 0x02, // REPORT_ID (2)
    0x05, 0x09, // USAGE_PAGE (Button)
    0x19, 0x01, // USAGE_MINIMUM (Button 1)
    0x29, 0x03, // USAGE_MAXIMUM (Button 3)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0x01, // LOGICAL_MAXIMUM (1)
    0x75, 0x01, // REPORT_SIZE (1)
    0x95, 0x03, // REPORT_COUNT (3)
    0x55, 0x00, // UNIT_EXPONENT (0)
    0x65, 0x00, // UNIT (None)
    0x81, 0x02, // INPUT (Data,Var,Abs)
    0x75, 0x01, // REPORT_SIZE (1)
    0x95, 0x01, // REPORT_COUNT (1)
    0x81, 0x03, // INPUT (Const,Var,Abs)
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x39, // USAGE (Hat Switch)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0x07, // LOGICAL_MAXIMUM (7)
    0x35, 0x00, // PHYSICAL_MINIMUM (0)
    0x46, 0x3B, 0x01, // PHYSICAL_MAXIMUM (315)
    0x65, 0x14, // UNIT (Eng Rot:Angular Pos)
    0x75, 0x04, // REPORT_SIZE (4)
    0x95, 0x01,	// REPORT_COUNT (1)
    0x81, 0x02, // INPUT (Data,Var,Abs)
    0x09, 0x01, // USAGE (Pointer)
    0x16, 0x01, 0x80, // LOGICAL_MINIMUM (-32767)
    0x26, 0xFF, 0x7F, // LOGICAL_MAXIMUM (32767)
    0x75, 0x10, // REPORT_SIZE (16)
    0x95, 0x02, // REPORT_COUNT (2)
    0xA1, 0x00, // COLLECTION (Physical)
      0x09, 0x30, // USAGE (X)
      0x09, 0x33,	// USAGE (Rx)
      0x81, 0x02, // INPUT (Data,Var,Abs)
    0xc0, // END_COLLECTION (Physical)
  0xc0, // END_COLLECTION
};
static const uint8_t hidDescriptorMouse[] PROGMEM = {  
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION
};

void hid_init() {
    static HIDSubDescriptor mouse(hidDescriptorMouse, sizeof(hidDescriptorMouse));
    HID().AppendDescriptor(&mouse);
    static HIDSubDescriptor joy(hidDescriptorJoystick, sizeof(hidDescriptorJoystick));
    HID().AppendDescriptor(&joy);
}

void mouse_report(signed char x, signed char y, char buttons) {
	uint8_t m[4];
  static char prev_buttons = 0;
  if (buttons == prev_buttons && !x && !y) return;
	m[0] = buttons;
	m[1] = x;
	m[2] = y;
	HID().SendReport(1, m, 3);
  prev_buttons = buttons;
}
unsigned char pins2hat[16] = { 8,8,8,8,8,3,1,2,8,5,7,6,8,4,0,8 };
void joy_report(int buttons, int dir, int potx, int poty) {
	uint8_t m[5];
  static int prev_potx, prev_poty, prev_buttons;
  buttons |= (pins2hat[dir&15] << 4);
  if (buttons == prev_buttons && potx == prev_potx && poty == prev_poty) return;
  m[0] = buttons;
  m[1] = potx & 255;
  m[2] = potx >> 8;
  m[3] = poty & 255;
  m[4] = poty >> 8;
  HID().SendReport(2, m, 5);
  prev_buttons = buttons;
  prev_potx = potx;
  prev_poty = poty;
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  pinMode(JOY1_PIN, INPUT_PULLUP);
  pinMode(JOY2_PIN, INPUT_PULLUP);
  pinMode(JOY3_PIN, INPUT_PULLUP);
  pinMode(JOY4_PIN, INPUT_PULLUP);
  pinMode(JOY6_PIN, INPUT_PULLUP);
  pinMode(POTXREF_PIN, OUTPUT);
  pinMode(POTYREF_PIN, OUTPUT);
  analogRead(POTX_PIN);
  digitalWrite(POTXREF_PIN, 0);
  digitalWrite(POTYREF_PIN, 1);
  hid_init();
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

#define NREADS 16
#define PADDETECT (255 * NREADS)
#define PADMIN (580 * NREADS)
#define PADMAX (1023 * NREADS)

struct abuf { int pin; int val[NREADS], pos, tot; } bPOTX = { POTX_PIN }, bPOTY = { POTY_PIN };
int bufAnalogRead(struct abuf *a) {
  int cur = analogRead(a->pin);
  a->tot += cur - a->val[a->pos];
  a->val[a->pos] = cur;
  a->pos = (a->pos + 1) % NREADS;
  return a->tot;
}

int padXVal, padYVal;
int lc = 0;
int paddlesConnected = 0;
int potState1 = 0;
int potState2 = 0;
int buttonState5 = 1;
int buttonState9 = 1;

void loop() {
  lc++;
  int buttonState1 = digitalRead(JOY1_PIN);
  int buttonState2 = digitalRead(JOY2_PIN);
  int buttonState3 = digitalRead(JOY3_PIN);
  int buttonState4 = digitalRead(JOY4_PIN);
  int buttonState6 = digitalRead(JOY6_PIN);
  if ((lc & 63) == 32) {
    potState1 = bufAnalogRead(&bPOTX);
    buttonState5 = digitalRead(POTY_PIN);
    analogRead(POTY_PIN);
    digitalWrite(POTYREF_PIN, 0);
    digitalWrite(POTXREF_PIN, 1);
  } else if ((lc & 63) == 0) {
    buttonState9 = digitalRead(POTX_PIN);
    potState2 = bufAnalogRead(&bPOTY);
    analogRead(POTX_PIN);
    digitalWrite(POTXREF_PIN, 0);
    digitalWrite(POTYREF_PIN, 1);

    paddlesConnected = potState1 > PADDETECT && potState2 > PADDETECT;
    if (paddlesConnected) {
      padXVal = (potState1 < PADMIN ? 0 : potState1 - PADMIN) * 0xFFFEL / (PADMAX - PADMIN) - 0x7FFFL;
      padYVal = (potState2 < PADMIN ? 0 : potState2 - PADMIN) * 0xFFFEL / (PADMAX - PADMIN) - 0x7FFFL;
    } else {
      padXVal = padYVal = 0;
    }
  }
  static int skip = 1;

  static int eerr;
  mousedecode(buttonState1, buttonState3, &ey, &eerr, skip);
  mousedecode(buttonState2, buttonState4, &ex, &eerr, skip);
  skip = 0;
  static long ts0;
  long ts = millis();
  if (ts - ts0 < 10) return;
  ts0 = ts;
  skip = 0;

  if (paddlesConnected) {
    int buttons = 3 ^ (buttonState3 + 2 * buttonState4);
    joy_report(buttons, 15, padXVal, padYVal);
    mouse_report(0, 0, 0);
  } else {
    int buttons = 7 ^ (buttonState6 + 2 * buttonState9 + 4 * buttonState5);
    joy_report(buttons, buttonState1 + buttonState2 * 2 + buttonState3 * 4 + buttonState4 * 8, 0, 0);
    mouse_report(ex.val * 2, ey.val * 2, buttons);
  }
  ex.val = ey.val = 0;
#ifdef DEBUG
  static int lc1 = 0;
  Serial.print(lc - lc1);
  Serial.print(' ');
  lc1 = lc;
  Serial.print(padXVal);
  Serial.print(' ');
  Serial.print(padYVal);
  Serial.print(' ');
  Serial.println(eerr);
#endif
}
