#include <FastLED.h>
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
// === CONFIG ===
#define SD_CS 10
#define LED_PIN 6
#define NUM_LEDS 12
#define SPEAKER_PIN 9
#define AUDIO_SENSE_PIN A0

#define BUTTON_PLAY_MASK (1 << PD2)
#define BUTTON_NEXT_MASK (1 << PD3)
#define BUTTON_MODE_MASK (1 << PD4)

CRGB leds[NUM_LEDS];
TMRpcm audio;

uint8_t currentMode = 0;
uint8_t currentSongIndex = 0;
bool isPlaying = false;

const char* songs[] = {
  "001.wav", "002.wav", "003.wav", "004.wav", "005.wav", "006.wav", "007.wav"
};
const int numSongs = sizeof(songs) / sizeof(songs[0]);

void lcd_init() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  lcd.setCursor(col, row);
}

void lcd_print(const char* str) {
  lcd.print(str);
}
// === BUTTONS via GPIO (PD2, PD3, PD4) ===
void initButtons() {
  DDRD &= ~(BUTTON_PLAY_MASK | BUTTON_NEXT_MASK | BUTTON_MODE_MASK); // input
  PORTD |= BUTTON_PLAY_MASK | BUTTON_NEXT_MASK | BUTTON_MODE_MASK;   // pull-up
}

bool buttonPressed(uint8_t mask) {
  return !(PIND & mask); // LOW = pressed
}

// === INIT ===
void setup() {
  Serial.begin(9600);
  initButtons();
  lcd_init();
  lcd_set_cursor(0, 0); lcd_print("Sistem pornit");
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear(); FastLED.show();
  pinMode(AUDIO_SENSE_PIN, INPUT);

  // === SPI Init manual ===
  DDRB |= (1 << PB3) | (1 << PB5); // MOSI, SCK output
  DDRB &= ~(1 << PB4);             // MISO input
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // enable SPI, master, fosc/16

  if (!SD.begin(SD_CS)) {
    lcd.clear(); lcd.print("Eroare SD card");
    while (true);
  }

  audio.speakerPin = SPEAKER_PIN;
  audio.setVolume(5);
  audio.quality(1);
}

// === MAIN LOOP ===
void loop() {
  static uint32_t lastDebounce = 0;
  if (millis() - lastDebounce > 300) {
    if (buttonPressed(BUTTON_PLAY_MASK)) {
      if (!isPlaying) playAudio();
      else stopAudio();
      lastDebounce = millis();
    }
    if (buttonPressed(BUTTON_NEXT_MASK)) {
      selectNextSong();
      lastDebounce = millis();
    }
    if (buttonPressed(BUTTON_MODE_MASK)) {
      currentMode = (currentMode + 1) % 3;
      lcd.clear();
      lcd.setCursor(0, 0);
      if (currentMode == 0) lcd.print("Mod: Energic");
      if (currentMode == 1) lcd.print("Mod: Unda");
      if (currentMode == 2) lcd.print("Mod: Disco");
      lastDebounce = millis();
    }
  }

  if (isPlaying) updateLEDEffects();
}

// === AUDIO ===
void playAudio() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Redare:");
  lcd.setCursor(0, 1); lcd.print(songs[currentSongIndex]);
  audio.play(songs[currentSongIndex]);
  isPlaying = true;
}

void stopAudio() {
  audio.stopPlayback();
  isPlaying = false;
}

void selectNextSong() {
  stopAudio();
  currentSongIndex = (currentSongIndex + 1) % numSongs;
  playAudio();
}

// === LED EFFECTS ===
void updateLEDEffects() {
  static uint8_t pos = 0;
  static uint32_t lastUpdate = 0;
  static uint8_t discoColor = 0;
  static uint32_t lastDiscoChange = 0;

  if (millis() - lastUpdate < 20) return;
  lastUpdate = millis();

  int audioLevel = analogRead(AUDIO_SENSE_PIN);
  int intensity = map(audioLevel, 0, 1023, 0, 255);
  intensity = constrain(intensity, 0, 200);

  switch (currentMode) {
    case 0: {
      uint8_t hueBase = random(0, 255);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hueBase + i * 10, 240, intensity);
      }
      break;
    }
    case 1: {
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t wave = sin8(i * 32 + pos * 4);
        uint8_t brightness = (wave * intensity) / 255;
        leds[i] = CHSV(i * 16, 220, brightness);
      }
      pos++;
      break;
    }
    case 2: {
      if (millis() - lastDiscoChange > 150) {
        FastLED.clear();
        discoColor += 30;
        int count = map(intensity, 50, 200, 3, 6);
        count = constrain(count, 2, NUM_LEDS);
        for (int i = 0; i < count; i++) {
          int index = random(NUM_LEDS);
          leds[index] = CHSV(discoColor + i * 10, 230, intensity);
        }
        lastDiscoChange = millis();
      }
      break;
    }
  }

  FastLED.show();
}