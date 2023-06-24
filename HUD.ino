/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16 // Px
#define LOGO_WIDTH    16 // Px
static const unsigned char PROGMEM logo_bmp[] =
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x1F, 0xE0, 0x3F, 0xE0, 0x38, 0x00, 0x38, 0x00, 0x38, 0x00,
0x38, 0x00, 0x38, 0x00, 0x38, 0x20, 0x3F, 0xE0, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int randomNumber = 200;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();


  aboutInfo();

  delay(1000);

  testdrawline();

  delay(1000);

  //testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void loop() {
}

float degToRad(float degree) {
  return degree * PI / 180.0;
}

void testdrawline() {

  display.clearDisplay();

  for (int speedCounter = 50; speedCounter > 10; speedCounter = speedCounter - 10) {

  for (int thisPin = 0; thisPin < 45; thisPin++) {
    delay(speedCounter);
    drawHud(thisPin, 0);
  }

  for (int thisPin = 45; thisPin > -45; thisPin--) {
    delay(speedCounter);
    drawHud(thisPin, 0);
  }

  for (int thisPin = -45; thisPin < 0; thisPin++) {
    delay(speedCounter);
    drawHud(thisPin, 0);
  }

  for (int roll = 0; roll < 45; roll++) {
    delay(speedCounter);
    drawHud(0, roll);
  }

  }

  delay(80000); // Pause for 2 seconds
}

void setUpText()
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
}

void drawHudLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[], float deltaDrawLine)
{
    float pointA[2];
    float pointB[2];

    float pointAH[2];
    float pointBH[2];

    rotatePoint(pointA, pointX[0] + deltaDrawLine, pointX[1] + offsetY, center[0], center[1], degToRad(roll));
    rotatePoint(pointB, pointY[0] - deltaDrawLine, pointY[1] + offsetY, center[0], center[1], degToRad(roll));

    rotatePoint(pointAH, pointX[0] + deltaDrawLine, pointX[1] + offsetY - 3, center[0], center[1], degToRad(roll));
    rotatePoint(pointBH, pointY[0] - deltaDrawLine, pointY[1] + offsetY - 3, center[0], center[1], degToRad(roll));

    display.setCursor(pointB[0] - 5, pointB[1] + 2);
    display.write(degreeString);

    display.drawLine(pointA[0], pointA[1], pointAH[0], pointAH[1], SSD1306_WHITE);
    display.drawLine(pointB[0], pointB[1], pointBH[0], pointBH[1], SSD1306_WHITE);
    display.drawLine(pointA[0], pointA[1], pointB[0], pointB[1], SSD1306_WHITE);
}

void drawHudLongLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[])
{
    float deltaDrawLine = 0;
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, deltaDrawLine);
}

void drawHudShortLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[])
{
    float deltaDrawLine = 20;
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, deltaDrawLine);
}

void drawHud(float thisPin, float pitch)
{
    setUpText();

    display.clearDisplay();

    //display.setRotation(3);

    double halfScreen = (display.height()-1) / 2 + pitch;
    double halfScreenSpeed = (display.height()-1) / 2;

    float x = (display.width()-1) / 2 - 35;
    float y = halfScreen;

    float i_p = (display.width()-1) / 2 + 35;
    float j_p = halfScreen;

    float centerX = ((display.width()-1) / 2);
    float centerY = ((display.height()-1) / 2);

    display.setCursor(10, halfScreenSpeed);

     randomNumber += random(3);
     randomNumber -= random(2);

    char buffer[10];

    itoa(randomNumber, buffer, 10);

    display.write(buffer);

    float point[2];
    float point2[2];

    rotatePoint(point, x, y, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p, j_p, centerX, centerY, degToRad(thisPin));

    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.write("0");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);

    char degreeString[] = "XXX";

    strcpy(degreeString, "0");

    float pointX[2] = {x, y};
    float pointY[2] = {i_p, j_p};
    float center[2] = {centerX, centerY};

    drawHudLongLine(pointX, pointY, center, thisPin, 0, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-20");
    drawHudShortLine(pointX, pointY, center, thisPin, 10, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "20");
    drawHudShortLine(pointX, pointY, center, thisPin, -10, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "40");
    drawHudShortLine(pointX, pointY, center, thisPin, -20, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-40");
    drawHudShortLine(pointX, pointY, center, thisPin, 20, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "60");
    drawHudShortLine(pointX, pointY, center, thisPin, -30, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-60");
    drawHudShortLine(pointX, pointY, center, thisPin, 30, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "80");
    drawHudShortLine(pointX, pointY, center, thisPin, -40, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-80");
    drawHudShortLine(pointX, pointY, center, thisPin, 40, degreeString);
    // -------------------------------------------------------------------
    display.display();
}

void rotatePoint(float (& point)[2], float x, float y, float center_x, float center_y, float angle)
{
  float x_end = (x - center_x) * cos(angle) - (y - center_y) * sin(angle) + center_x;
  float y_end = (x - center_x) * sin(angle) + (y - center_y) * cos(angle) + center_y;

  point[0] = x_end;
  point[1] = y_end;
}

void aboutInfo(void) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.setRotation(0);
  display.write("ArduinoHUD\n");
  display.setTextSize(1);
  display.write("\nLittle HUD made\nfor Arduino\n");
  display.write("Duxy1996\n");
  display.display();
  delay(5000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}
