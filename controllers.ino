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
#define MAPPOT

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

uint8_t potmap[512] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 4, 6, 7, 8, 10, 11, 13, 14, 15, 17, 18, 20, 21, 22, 24, 25, 26, 28, 29, 31, 32, 33,
    35, 36, 38, 39, 40, 42, 43, 45, 46, 47, 49, 50, 52, 53, 54, 56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74,
    75, 77, 78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 113, 114, 115, 116, 117, 118, 119, 120, 121, 121, 122, 123, 124, 125, 126, 127, 128, 129, 129,
    130, 131, 132, 133, 134, 135, 136, 137, 137, 138, 139, 140, 141, 142, 143, 144, 145, 145, 146, 147, 148, 149, 150,
    151, 152, 153, 153, 154, 155, 156, 157, 158, 159, 160, 161, 161, 162, 163, 164, 165, 166, 167, 168, 169, 169, 170,
    171, 172, 173, 174, 175, 176, 177, 177, 178, 179, 180, 181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 189, 190,
    190, 191, 191, 192, 192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 196, 197, 197, 198, 198, 199, 199, 199, 200,
    200, 201, 201, 202, 202, 203, 203, 203, 204, 204, 205, 205, 206, 206, 206, 207, 207, 208, 208, 209, 209, 209, 210,
    210, 211, 211, 212, 212, 213, 213, 213, 214, 214, 215, 215, 216, 216, 216, 217, 217, 218, 218, 219, 219, 220, 220,
    220, 221, 221, 222, 222, 223, 223, 223, 224, 224, 225, 225, 226, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230,
    230, 231, 231, 232, 232, 233, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 237, 238, 238, 239, 239, 240, 240,
    240, 241, 241, 242, 242, 243, 243, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 245,
    245, 245, 245, 245, 245, 245, 245, 245, 245, 245, 245, 245, 245, 245, 246, 246, 246, 246, 246, 246, 246, 246, 246,
    246, 246, 246, 246, 246, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 248, 248, 248,
    248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
    249, 249, 249, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251,
    251, 251, 251, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
    252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254,
    254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255,
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
  pinMode(POTX_PIN, INPUT);
  pinMode(POTY_PIN, INPUT);
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

int mouse_skip = 0;
int mouse_err;
struct encoder { int val, prev; } ex = { 0, 3 }, ey = {0, 3};
void mousedecode(int s1, int s2, struct encoder *e) {
  int s = s1 * 2 + s2;
  if (!mouse_skip && e->prev != s) {
    int code = e->prev * 4 + s;
    if ((PLUS >> code) & 1) e->val++;
    if ((MINUS >> code) & 1) e->val--;
#ifdef DEBUG
    if ((TERR >> code) & 1) mouse_err++;
#endif
  }
  e->prev = s;
}

#define NREADS 16
#define PADDETECT (255 * NREADS)
#define PADMIN (580 * NREADS)
#ifdef MAPPOT
#define PADMAX ((1023+255) * NREADS)
#else
#define PADMAX (1023 * NREADS)
#endif
struct abuf { int pin; uint16_t val[NREADS], pos, tot; } bPOTX = { POTX_PIN }, bPOTY = { POTY_PIN };
uint16_t bufAnalogRead(struct abuf *a) {
  int cur = analogRead(a->pin);
#ifdef MAPPOT
  cur += cur & 512 ? potmap[cur & 511] : 0;
#endif
  a->tot += cur - a->val[a->pos];
  a->val[a->pos] = cur;
  a->pos = (a->pos + 1) % NREADS;
  return a->tot;
}

int padXVal, padYVal;
int lc = 0;
int paddlesConnected = 0;
uint16_t potState1 = 0;
uint16_t potState2 = 0;
int buttonState5 = 1;
int buttonState9 = 1;
int mouseMaybeConnected = 0;

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
  if (!buttonState1 && !buttonState2 || !buttonState3 && !buttonState4) mouseMaybeConnected = 1;
  mousedecode(buttonState1, buttonState3, &ey);
  mousedecode(buttonState2, buttonState4, &ex);
  static long ts0;
  long ts = millis();
  if (ts - ts0 < 10) return;
  ts0 = ts;

  if (paddlesConnected) {
    int buttons = 3 ^ (buttonState3 + 2 * buttonState4);
    joy_report(buttons, 15, padXVal, padYVal);
    if (mouseMaybeConnected) mouse_report(0, 0, 0);
  } else {
    int buttons = 7 ^ (buttonState6 + 2 * buttonState9 + 4 * buttonState5);
    joy_report(buttons, buttonState1 + buttonState2 * 2 + buttonState3 * 4 + buttonState4 * 8, 0, 0);
    if (mouseMaybeConnected) mouse_report(ex.val * 2, ey.val * 2, buttons);
  }
  ex.val = ey.val = 0;
#ifdef DEBUG
  static int lc1 = 0;
  Serial.print(lc - lc1);
  Serial.print(' ');
  lc1 = lc;
  Serial.print(potState1/NREADS);
  Serial.print(' ');
  Serial.print(potState2/NREADS);
  Serial.print(' ');
  Serial.println(mouse_err);
#endif
}
