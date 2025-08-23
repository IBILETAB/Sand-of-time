/*Author: Wilfred Adeyi AKA THE RAM
Code and hardware by "THE PINOUT CREW" For "Ala Praxis"*/ 
#include <Arduino.h>
#include "HX711.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ---------- Pins ----------
#define HX711_DOUT A1
#define HX711_SCK  A0
#define DF_RX_PIN 10   // Nano TX -> DF RX (through 1k)
#define DF_TX_PIN 11   // Nano RX <- DF TX
#define TARE_BTN  4    // to GND, uses INPUT_PULLUP

// ---------- Weight & Volume Params ----------
const long   MAX_WEIGHT_G = 20000;  // 20 kg in grams
const uint8_t VOL_MAX = 30;         // loudest
const uint8_t VOL_MIN = 5;          // quietest (DFPlayer valid: 0..30)
const uint8_t VOL_STEP_HYST = 1;    // deadband in volume steps

// Smoothing (exponential moving average)
const float  ALPHA = 0.2f;

// ---------- Globals ----------
HX711 scale;
SoftwareSerial mp3Serial(DF_TX_PIN, DF_RX_PIN); // (DF TX, DF RX)
DFRobotDFPlayerMini df;

float emaGrams = 0.0f;
long lastVolCmd = -1;

// *** Calibration ***
// After wiring, run, press tare, then place a known mass (e.g., 2000 g)
// Update CALIBRATION_FACTOR below until readout matches.
float CALIBRATION_FACTOR = -26.725715f; // example value; adjust to your setup

// Map with clamping
long mapConstrain(long x, long in_min, long in_max, long out_min, long out_max) {
  if (x <= in_min) return out_min;
  if (x >= in_max) return out_max;
  return map(x, in_min, in_max, out_min, out_max);
}

void setVolumeSafe(uint8_t vol) {
  vol = constrain(vol, 0, 30);
  if ((long)vol != lastVolCmd) {
    df.volume(vol);
    lastVolCmd = vol;
  }
}

void tareScale() {
  scale.tare();
}

// Convert HX711 raw to grams using calibration constant
float readGrams() {
  // average of multiple readings improves stability
  float units = scale.get_units(5);      // library returns "units" based on calibration_factor
  return units; // we'll set calibration so units == grams
}

void setup() {
  pinMode(TARE_BTN, INPUT_PULLUP);

  Serial.begin(115200);

  // HX711 init
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale(CALIBRATION_FACTOR);
  tareScale();

  // DFPlayer init
  mp3Serial.begin(9600);
  if (!df.begin(mp3Serial)) {
    // If init fails, keep going but report
    Serial.println(F("DFPlayer init fail. Check wiring/SD."));
  } else {
    df.volume(VOL_MAX);   // start loud at zero weight
    df.EQ(DFPLAYER_EQ_NORMAL);
    // Play track 1 on loop
    df.loop(1);           // loops 0001.mp3
  }
}

void loop() {
  // Tare if button pressed
  if (digitalRead(TARE_BTN) == LOW) {
    tareScale();
    delay(300); // debounce
  }

  // Read weight
  float grams = readGrams();
  if (isnan(grams) || isinf(grams)) return;

  // Clamp negative noise to 0
  if (grams < 0) grams = 0;

  // Exponential smoothing
  emaGrams = (ALPHA * grams) + (1.0f - ALPHA) * emaGrams;

  // Convert to integer grams for mapping
  long g = (long)(emaGrams + 0.5f);

  // Map 0..20kg => VOL_MAX..VOL_MIN (inverse relationship)
  long targetVol = mapConstrain(g, 0, MAX_WEIGHT_G, VOL_MAX, VOL_MIN);

  // Add small hysteresis to reduce chatter
  if (lastVolCmd >= 0 && abs((long)targetVol - lastVolCmd) < VOL_STEP_HYST) {
    // keep previous
  } else {
    setVolumeSafe((uint8_t)targetVol);
  }

  // Optional: debug output
  static uint32_t tLast = 0;
  if (millis() - tLast > 500) {
    tLast = millis();
    Serial.print(F("Weight[g]: "));
    Serial.print(g);
    Serial.print(F("  Volume: "));
    Serial.println((int)lastVolCmd);
  }
}
