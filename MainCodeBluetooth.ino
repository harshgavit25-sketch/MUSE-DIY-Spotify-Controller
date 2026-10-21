
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C

#define BTN_PREV 26
#define BTN_PLAY 27
#define BTN_NEXT 14
#define BTN_MUTE 13

#define LED_PREV 16
#define LED_PLAY 17
#define LED_NEXT 18

Adafruit_SSD1306 display(
  OLED_WIDTH, OLED_HEIGHT, &Wire, -1
);

String songTitle = "Waiting...";
String artistName = "Open Spotify";

long positionMs = 0;
long durationMs = 0;

const int buttonPins[] = {
  BTN_PREV, BTN_PLAY, BTN_NEXT, BTN_MUTE
};

const int ledPins[] = {
  LED_PREV, LED_PLAY, LED_NEXT
};

bool stableState[4] = {HIGH, HIGH, HIGH, HIGH};
bool lastReading[4] = {HIGH, HIGH, HIGH, HIGH};

unsigned long lastChange[4] = {0, 0, 0, 0};
unsigned long ledOffAt[3] = {0, 0, 0};

bool playbackIndicator = false;
bool displayDirty = true;

unsigned long lastDisplayUpdate = 0;

// ---------- HELPERS ----------

String shortenText(String text, int maxLength) {
  text.trim();

  if (text.length() > maxLength) {
    return text.substring(0, maxLength - 2) + "..";
  }

  return text;
}

String formatTime(long milliseconds) {
  long seconds = max(0L, milliseconds / 1000L);

  char buffer[12];

  snprintf(
    buffer, sizeof(buffer), "%ld:%02ld",
    seconds / 60, seconds % 60
  );

  return String(buffer);
}

void drawSpotifyMark(int x, int y) {
  display.fillCircle(x + 6, y + 6, 6, SSD1306_WHITE);

  display.drawLine(
    x + 3, y + 4, x + 9, y + 5, SSD1306_BLACK
  );

  display.drawLine(
    x + 3, y + 6, x + 9, y + 8, SSD1306_BLACK
  );

  display.drawLine(
    x + 4, y + 9, x + 8, y + 10, SSD1306_BLACK
  );
}

void drawPreviousIcon(int x, int y) {
  display.drawFastVLine(x, y, 13, SSD1306_WHITE);

  display.fillTriangle(
    x + 2, y,
    x + 2, y + 12,
    x + 9, y + 6,
    SSD1306_WHITE
  );
}

void drawNextIcon(int x, int y) {
  display.drawFastVLine(x + 9, y, 13, SSD1306_WHITE);

  display.fillTriangle(
    x + 7, y,
    x + 7, y + 12,
    x, y + 6,
    SSD1306_WHITE
  );
}

// ---------- OLED ----------

void drawUI() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  drawSpotifyMark(1, 1);

  display.setCursor(16, 4);
  display.print("sharvith_07");

  display.drawFastHLine(0, 15, 128, SSD1306_WHITE);

  String title = shortenText(songTitle, 10);

  display.setTextSize(2);

  int titleX = (128 - title.length() * 12) / 2;
  if (titleX < 0) titleX = 0;

  display.setCursor(titleX, 18);
  display.print(title);

  display.setTextSize(1);

  drawPreviousIcon(5, 36);
  drawNextIcon(114, 36);

  String artist = shortenText(artistName, 15);
  int artistX = (128 - artist.length() * 6) / 2;

  display.setCursor(artistX, 37);
  display.print(artist);

  String timeText =
    formatTime(positionMs) + "/" + formatTime(durationMs);

  int timeX = (128 - timeText.length() * 6) / 2;

  display.setCursor(timeX, 47);
  display.print(timeText);

  display.drawRect(3, 56, 122, 7, SSD1306_WHITE);

  if (durationMs > 0) {
    long filled = constrain(
      positionMs * 118L / durationMs, 0L, 118L
    );

    if (filled > 0) {
      display.fillRect(5, 58, filled, 3, SSD1306_WHITE);
    }
  }

  display.display();
}

// ---------- RECEIVE PYTHON METADATA ----------

void processBluetoothInput() {
  if (!SerialBT.available()) return;

  String line = SerialBT.readStringUntil('\n');
  line.trim();

  if (line.startsWith("TITLE:")) {
    songTitle = line.substring(6);
    displayDirty = true;
  }
  else if (line.startsWith("ARTIST:")) {
    artistName = line.substring(7);
    displayDirty = true;
  }
  else if (line.startsWith("PROG:")) {
    int comma = line.indexOf(',');

    if (comma > 5) {
      positionMs = max(
        0L, line.substring(5, comma).toInt()
      );

      durationMs = max(
        0L, line.substring(comma + 1).toInt()
      );

      displayDirty = true;
    }
  }
}

// ---------- BUTTONS ----------

void handleButtons(unsigned long now) {
  for (int i = 0; i < 4; i++) {
    bool reading = digitalRead(buttonPins[i]);

    if (reading != lastReading[i]) {
      lastReading[i] = reading;
      lastChange[i] = now;
    }

    if (
      now - lastChange[i] >= 30 &&
      reading != stableState[i]
    ) {
      stableState[i] = reading;

      if (stableState[i] == LOW) {
        if (i == 0) {
          SerialBT.println("BTN:PREV");

          digitalWrite(LED_PREV, HIGH);
          ledOffAt[0] = now + 180;
        }
        else if (i == 1) {
          SerialBT.println("BTN:PLAY");

          playbackIndicator = !playbackIndicator;

          digitalWrite(
            LED_PLAY,
            playbackIndicator ? HIGH : LOW
          );
        }
        else if (i == 2) {
          SerialBT.println("BTN:NEXT");

          digitalWrite(LED_NEXT, HIGH);
          ledOffAt[2] = now + 180;
        }
        else if (i == 3) {
          SerialBT.println("BTN:MUTE");
        }
      }
    }
  }

  if (ledOffAt[0] && now >= ledOffAt[0]) {
    digitalWrite(LED_PREV, LOW);
    ledOffAt[0] = 0;
  }

  if (ledOffAt[2] && now >= ledOffAt[2]) {
    digitalWrite(LED_NEXT, LOW);
    ledOffAt[2] = 0;
  }
}

// ---------- SETUP ----------

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);

  SerialBT.setTimeout(50);

  Wire.begin(21, 22);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true) delay(10);
  }

  drawUI();

  if (!SerialBT.begin("MUSE-Sharvith")) {
    Serial.println("Bluetooth start failed!");

    while (true) delay(1000);
  }

  Serial.println("Bluetooth ready: MUSE-Sharvith");
}

// ---------- MAIN LOOP ----------

void loop() {
  unsigned long now = millis();

  processBluetoothInput();

  handleButtons(now);

  if (displayDirty && now - lastDisplayUpdate >= 100) {
    drawUI();

    displayDirty = false;
    lastDisplayUpdate = now;
  }

  delay(2);
}
