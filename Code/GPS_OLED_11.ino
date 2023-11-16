#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C
#define Anzeigeintervall 1000

SSD1306AsciiWire oled;

// der String, aus dem die zwei Teilstrings links und rechts der Gradzahl als stilisiertes Kompassband begildet wird
// ein Zeichen entspricht 10 Grad auf der Kompassrose
String stringOne = "-Nw--N----No--O----So--S----Sw--W----Nw--N----No--O----So--S----";

// Das TinyGPSPlus Objekt
TinyGPSPlus gps;

// Die serielle Verbindung zum GPS-Chip
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  ss.begin(GPSBaud);

  Wire.begin();
  Wire.setClock(400000L);

  oled.begin(&Adafruit128x32, I2C_ADDRESS);

  oled.clear();
  oled.setFont(System5x7);
  oled.println("+-------------------+");
  // Begrüßungsbilldschirm
  oled.setFont(Arial_bold_14);
  oled.println("|Ski - Speedo - HUD|");
  oled.setFont(System5x7);
  oled.println("+-------------------+");

  smartDelay(5 * Anzeigeintervall);
}

void loop()
{
  AnzeigeAusgeben(gps.altitude.meters(),
                  gps.speed.kmph(),
                  gps.course.deg(),
                  gps.satellites.value());

  smartDelay(Anzeigeintervall);

  if (
    (millis() > (10 * Anzeigeintervall))
    &&
    (gps.charsProcessed() < 10)
  )
  {
    oled.setFont(System5x7);
    oled.println(F("No GPS data received:"));
    oled.println(F("check wiring/Chip"));
    smartDelay(15 * Anzeigeintervall);
  };
}

// Diese veränderte Implementation von delay() läßt weiter Information
// über ss. eingehen und leitet diese an das gps-Objekt weiter

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


void AnzeigeAusgeben (float AnzeigeHoehe,
                      float AnzeigeGeschwindigkeit,
                      float AnzeigeKurs,
                      int AnzeigeSatelliten) {

  oled.clear(); //Löschen der aktuellen Displayanzeige

  oled.setFont(System5x7);
  oled.print( "A ");
  oled.setFont(fixed_bold10x15);
  oled.print(AnzeigeHoehe, 0);

  oled.setFont(System5x7);
  oled.print( " V ");
  oled.setFont(fixed_bold10x15);

  oled.print(AnzeigeGeschwindigkeit, 0); // KM/H

  oled.setFont(System5x7);
  oled.print( " S ");

  oled.print(AnzeigeSatelliten);

  oled.setFont(fixed_bold10x15);
  oled.println();

  int kurs_360 = (int) AnzeigeKurs;
  int kurs = kurs_360 / 10;

  oled.print(stringOne.substring(kurs,  kurs + 5));

  oled.setFont(System5x7);
  oled.print(kurs_360);

  oled.setFont(fixed_bold10x15);
  oled.print(stringOne.substring(kurs + 5,  kurs + 11));

  return ;
};
