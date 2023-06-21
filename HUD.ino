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

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
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

  testdrawline();      // Draw many lines

  testdrawchar();      // Draw characters of the default font

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

void drawHud(float thisPin, float pitch)
{
  
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

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, halfScreenSpeed);
    display.cp437(true); 

     randomNumber += random(3);
     randomNumber -= random(2);

     char buffer[10];
  
    itoa(randomNumber, buffer, 10);
  
    display.write(buffer);

    float point[2];
    float point2[2];   
  
    rotatePoint(point, x, y, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p, j_p, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("0");
  
    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y + 10, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p + 10, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("-20");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y - 10, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p - 10, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("20");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y - 20, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p - 20, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("40");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y + 20, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p + 20, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("-40");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y - 30, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p - 30, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("60");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y + 30, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p + 30, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("-60");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y - 40, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p - 40, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("80");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
    // -------------------------------------------------------------------
    rotatePoint(point, x + 20, y + 40, centerX, centerY, degToRad(thisPin));
    rotatePoint(point2, i_p - 20, j_p + 40, centerX, centerY, degToRad(thisPin));

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(point2[0] - 5, point2[1] + 2);
    display.cp437(true);  
  
    display.write("-80");

    display.drawLine(point[0], point[1], point2[0], point2[1], SSD1306_WHITE);
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

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.setRotation(2);
  display.write("Carlos\n");
  display.display();
  delay(2000);
  display.setRotation(0);
  display.write("Raquel\n");  
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setRotation(1);
  display.write("\nCarlos\n");
  display.write("Raquel\n");
  display.display();

  delay(5000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
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

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
