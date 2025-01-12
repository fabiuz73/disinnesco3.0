#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <TM1637Display.h>
#include <SPI.h>
#include <Keypad.h>
#include <EEPROM.h>

// Pin del Display ST7735
#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

// Pin del Display TM1637
#define CLK 2
#define DIO 3
TM1637Display tm1637(CLK, DIO);

// Pin del buzzer
#define BUZZER 4

// Configurazione del tastierino 4x4
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Inizializza il display ST7735
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Variabili globali
int correctCode[5];  // Codice salvato in EEPROM
int newCode[5];      // Nuovo codice modificabile
int codeLength = 4;  // Lunghezza del codice (da 2 a 5 numeri)
int timerValue = 30; // Timer di default in secondi

// Dichiarazioni delle funzioni
void drawPresentationScreen();
void showMainMenu();
void showDisinnescoCodiceMenu();
void modificaCodice();
void modificaTimer();
void avvioCountdown();
void salvaCodiceInEEPROM();
void leggiCodiceDaEEPROM();
bool verificaCodiceInserito();
void animateVirusSymbol();
void drawVirusSymbol(uint16_t color);
void suonaBuzzer(int frequenza, int durata);

void setup() {
  // Inizializza i display e il buzzer
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tm1637.setBrightness(0x0f);
  pinMode(BUZZER, OUTPUT);

  // Carica il codice salvato in EEPROM
  leggiCodiceDaEEPROM();

  // Disegna la schermata di presentazione
  drawPresentationScreen();
}

void loop() {
  char key = keypad.getKey();

  if (key == '#') {
    showMainMenu(); // Entra nel menu principale
  } else {
    animateVirusSymbol(); // Mostra animazione simbolo virus
  }
}

// Funzione: Disegna la schermata di presentazione
void drawPresentationScreen() {
  tft.fillScreen(ST77XX_BLACK);

  // Testo centrato "AIRSOFT"
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor((128 - (7 * 12)) / 2, 10);
  tft.println("AIRSOFT");

  // Testo centrato "SCENOGRAPHY"
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(1);
  tft.setCursor((128 - (11 * 6)) / 2, 40);
  tft.println("SCENOGRAPHY");

  // Simbolo virus al centro
  drawVirusSymbol(ST77XX_GREEN);

  // Scritta centrata "PRESS #"
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.setCursor((128 - (7 * 12)) / 2, 140);
  tft.println("PRESS #");
}

// Funzione: Menu principale
void showMainMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.setCursor((128 - (14 * 6)) / 2, 10);
  tft.println("MENU DISINNESCO");

  tft.setCursor(10, 40);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("A. Disinnesco");
  tft.setCursor(10, 55);
  tft.println("   con Codice");

  tft.setCursor(10, 75);
  tft.setTextColor(ST77XX_CYAN);
  tft.println("B. Disinnesco");
  tft.setCursor(10, 90);
  tft.println("   con Fili");

  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("*. Ritorna");

  while (true) {
    char key = keypad.getKey();
    if (key == 'A') {
      showDisinnescoCodiceMenu(); // Opzione A: Sottomenu disinnesco codice
      break;
    } else if (key == 'B') {
      // In futuro: Aggiungi sottomenu disinnesco fili
      break;
    } else if (key == '*') {
      drawPresentationScreen(); // Ritorna alla schermata principale
      break;
    }
  }
}

// Funzione: Sottomenu Disinnesco con Codice
void showDisinnescoCodiceMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.setCursor((128 - (14 * 6)) / 2, 10);
  tft.println("DISINNESCO CON");
  tft.setCursor((128 - (6 * 6)) / 2, 25);
  tft.println("CODICE");

  tft.setCursor(10, 40);
  tft.setTextColor(ST77XX_GREEN);
  tft.println("1. Modifica Codice");
  tft.setCursor(10, 60);
  tft.println("2. Modifica Timer");
  tft.setCursor(10, 80);
  tft.println("3. Avvio Countdown");

  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("#. Ritorna");

  while (true) {
    char key = keypad.getKey();
    if (key == '1') {
      modificaCodice(); // Opzione 1: Modifica codice
      break;
    } else if (key == '2') {
      modificaTimer(); // Opzione 2: Modifica timer
      break;
    } else if (key == '3') {
      avvioCountdown(); // Opzione 3: Avvio countdown
      break;
    } else if (key == '#') {
      showMainMenu(); // Ritorna al menu principale
      break;
    }
  }
}

// Funzione: Modifica codice
void modificaCodice() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println("MODIFICA CODICE");
  tft.setCursor(10, 30);
  tft.println("(PRESS # PER CONF.)");

  int index = 0;
  while (true) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9' && index < 5) {
      newCode[index] = key - '0';
      tft.setCursor(10 + index * 12, 50);
      tft.print(key);
      index++;
    } else if (key == '#') {
      codeLength = index;
      salvaCodiceInEEPROM();
      tft.fillScreen(ST77XX_GREEN);
      tft.setCursor(10, 30);
      tft.println("CODICE SALVATO!");
      delay(2000);
      break;
    }
  }
  showDisinnescoCodiceMenu();
}

// Funzione: Modifica timer
void modificaTimer() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println("MODIFICA TIMER");
  tft.setCursor(10, 30);
  tft.println("(PRESS # PER CONF.)");

  int tempTimer = 0; // Variabile temporanea per il nuovo timer
  while (true) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      tempTimer = tempTimer * 10 + (key - '0'); // Aggiunge il numero digitato al timer
      tft.setCursor(10, 50);
      tft.fillRect(10, 50, 100, 20, ST77XX_BLACK); // Pulisce la precedente visualizzazione
      tft.print(tempTimer); // Visualizza il nuovo valore del timer
    } else if (key == '#') {
      timerValue = tempTimer; // Salva il nuovo valore del timer
      tft.fillScreen(ST77XX_GREEN);
      tft.setCursor(10, 30);
      tft.println("TIMER SALVATO!");
      delay(2000);
      break; // Esce dalla funzione
    }
  }
  showDisinnescoCodiceMenu(); // Torna al menu disinnesco codice
}

// Funzione: Avvio countdown
void avvioCountdown() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println("COUNTDOWN INIZIATO");

  for (int i = timerValue; i >= 0; i--) {
    tm1637.showNumberDec(i, true);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 50);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED);
    tft.print("TIMER: ");
    tft.println(i);

    suonaBuzzer(1000, 100); // Suono durante il countdown
    delay(1000);

    if (i == 0) {
      tft.fillScreen(ST77XX_RED);
      tft.setCursor(10, 50);
      tft.println("ESPLOSO!");
      suonaBuzzer(200, 2000); // Suono esplosione
      delay(2000);
      break;
    }

    if (keypad.getKey() == '#') {
      if (verificaCodiceInserito()) {
        tft.fillScreen(ST77XX_GREEN);
        tft.setCursor(10, 50);
        tft.println("DISINNESCATO!");
        suonaBuzzer(1500, 1000); // Suono disinnesco
        delay(2000);
        break;
      } else {
        i -= 3; // Accelerazione timer per codice errato
      }
    }
  }
  showDisinnescoCodiceMenu();
}

// Funzione: Verifica il codice inserito
bool verificaCodiceInserito() {
  int enteredCode[5];
  int enteredIndex = 0;

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("INSERISCI CODICE:");

  while (enteredIndex < codeLength) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      enteredCode[enteredIndex] = key - '0';
      tft.setCursor(10 + enteredIndex * 12, 50);
      tft.print("*");
      enteredIndex++;
    }
  }

  for (int i = 0; i < codeLength; i++) {
    if (enteredCode[i] != correctCode[i]) {
      return false;
    }
  }
  return true;
}

// Funzione: Suona il buzzer
void suonaBuzzer(int frequenza, int durata) {
  tone(BUZZER, frequenza, durata);
  delay(durata);
  noTone(BUZZER);
}

// Funzione: Salva il codice in EEPROM
void salvaCodiceInEEPROM() {
  for (int i = 0; i < codeLength; i++) {
    EEPROM.write(i, newCode[i]);
    correctCode[i] = newCode[i];
  }
}

// Funzione: Legge il codice da EEPROM
void leggiCodiceDaEEPROM() {
  for (int i = 0; i < 5; i++) {
    correctCode[i] = EEPROM.read(i);
    if (correctCode[i] < 0 || correctCode[i] > 9) {
      correctCode[i] = 0; // Valore di default se EEPROM non valida
    }
  }
}

// Funzione: Disegna simbolo virus
void drawVirusSymbol(uint16_t color) {
  int centerX = 64;
  int centerY = 90;
  tft.fillCircle(centerX, centerY, 15, color);
  tft.drawCircle(centerX, centerY, 20, color);
  for (int i = 0; i < 360; i += 45) {
    int x1 = centerX + 25 * cos(radians(i));
    int y1 = centerY + 25 * sin(radians(i));
    int x2 = centerX + 35 * cos(radians(i));
    int y2 = centerY + 35 * sin(radians(i));
    tft.drawLine(x1, y1, x2, y2, color);
    tft.fillCircle(x2, y2, 5, color);
  }
}

// Funzione: Animazione simbolo virus
void animateVirusSymbol() {
  static uint16_t colors[] = {ST77XX_GREEN, ST77XX_RED, ST77XX_YELLOW, ST77XX_BLUE};
  static int colorIndex = 0;
  static unsigned long previousMillis = 0;
  unsigned long interval = 500;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    colorIndex = (colorIndex + 1) % 4;
    drawVirusSymbol(colors[colorIndex]);
  }
}
