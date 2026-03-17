/**************************************************************************
This is an application for a minimalistic Heads-Up Display (HUD) on a 128x32
pixel OLED screen using the SSD1306 driver.

This application utilizes Arduino and the Adafruit SSD1306 library to interface
with the OLED display. It provides a compact HUD layout with basic information.

The code references the Adafruit SSD1306 library for OLED display functionality.
The library can be obtained from the Adafruit website:
---> http://www.adafruit.com/category/63_98

This application is developed based on the contributions from the open-source
community and Adafruit Industries. It is provided under the BSD license.

To set up the HUD display, connect the OLED display to the Arduino board
using I2C communication. Three pins are required: two for I2C (data and clock)
and one for reset.

Written by Duxy1996 with code references and inspiration from Adafruit and
the open-source community.
**************************************************************************/

// system libs
// #include <SPI.h>
#include <Wire.h>

// 3rd party libs

// screen lib
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// gyroscope lib
#include <Adafruit_MPU6050.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 56 // OLED display height, in pixels

#define TOTAL_RAM 2048

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// init adafruit class to manage display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Adafruit_MPU6050
Adafruit_MPU6050 mpu;

float roll_general = 0;
float pitch_general = 0;
// float yaw_general = 0;

unsigned long lastTime;

void setupScreen() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (1);
  }

  // Clear the buffer
  display.clearDisplay();
  aboutInfo();
}

void setupGyroscope() {
  if (!mpu.begin()) {
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  lastTime = millis();
}

void setup() {
  Serial.begin(57600);

  Wire.begin();

  setupScreen();
  setupGyroscope();
}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long now = millis();
  float dt = ((now - lastTime) / 1000.0);
  lastTime = now;

  roll_general += g.gyro.x * dt * 57.295791;
  pitch_general += g.gyro.y * dt * 57.295791;
  // yaw_general += g.gyro.z * dt * 57.295791;

  roll_general = 0.96 * roll_general + 0.04 * (atan2(a.acceleration.y, a.acceleration.z) * 57.295791);
  pitch_general = 0.96 * pitch_general + 0.04 * (atan2(-a.acceleration.x,
                  sqrt(a.acceleration.y * a.acceleration.y +
                       a.acceleration.z * a.acceleration.z)) * 57.295791);

  drawHud((int)roll_general, -(int)pitch_general);
}

/**
 * Convert degrees to radians.
 *
 * This function takes a degree value as input and converts it to radians using the formula
 * degree * PI / 180.0. It returns the corresponding value in radians.
 *
 * @param degree The degree value to be converted to radians.
 * @return The converted value in radians.
 */
float degToRad(float degree) {
  return degree * PI / 180.0;
}

/**
 * Convert radians to degrees.
 *
 * This function takes a radian value as input and converts it to degrees using the formula
 * degree / PI * 180.0. It returns the corresponding value in degrees.
 *
 * @param radian The radian value to be converted to degrees.
 * @return The converted value in degrees.
 */
float radToDeg(float degree) {
  return degree / PI * 180.0;
}

/**
 * Set up the text properties for display.
 *
 * This function configures the text size, color, and character set for the display.
 */
void setUpText()
{
  display.setTextSize(1);              // Set the text size to 1
  display.setTextColor(SSD1306_WHITE); // Set the text color to white
  display.cp437(true);                  // Enable CP437 character set for extended ASCII characters
}

/**
 * Draws a HUD (Head-Up Display) line on a display.
 *
 * @param pointX         An array of two float values representing the starting point of the line.
 * @param pointY         An array of two float values representing the ending point of the line.
 * @param center         An array of two float values representing the center point of rotation.
 * @param roll           The roll angle in degrees for rotating the line.
 * @param offsetY        The offset in the y-direction for positioning the line and degree string.
 * @param degreeString   A character array containing the degree string to be displayed.
 * @param deltaDrawLine  The delta value for adjusting the length of the line.
 */
void drawHudLine(int pointX[2], int pointY[2], int center[2], float roll, float offsetY, const __FlashStringHelper* degreeString, int deltaDrawLine)
{
    int pointA[2];
    int pointB[2];

    int pointAH[2];
    int pointBH[2];

    // Rotate pointX and pointY based on the roll angle and store the results in pointA and pointB
    rotatePoint(pointA, pointX[0] + deltaDrawLine, pointX[1] + offsetY, center[0], center[1], degToRad(roll));
    rotatePoint(pointB, pointY[0] - deltaDrawLine, pointY[1] + offsetY, center[0], center[1], degToRad(roll));

    // Calculate new points for displaying the degree string slightly above the line
    rotatePoint(pointAH, pointX[0] + deltaDrawLine, pointX[1] + offsetY - 3, center[0], center[1], degToRad(roll));
    rotatePoint(pointBH, pointY[0] - deltaDrawLine, pointY[1] + offsetY - 3, center[0], center[1], degToRad(roll));

    // Set the cursor position and display the degree string
    display.setCursor(pointB[0] - 5, pointB[1] + 2);
    display.print(degreeString);

    // Draw the lines to form the HUD line
    display.drawLine(pointA[0], pointA[1], pointAH[0], pointAH[1], SSD1306_WHITE);
    display.drawLine(pointB[0], pointB[1], pointBH[0], pointBH[1], SSD1306_WHITE);
    display.drawLine(pointA[0], pointA[1], pointB[0], pointB[1], SSD1306_WHITE);
}

/**
 * Draws a long line on the HUD (Heads-Up Display) based on the given parameters.
 *
 * @param pointX[2]    An array containing the X-coordinates of the two points defining the line segment.
 * @param pointY[2]    An array containing the Y-coordinates of the two points defining the line segment.
 * @param center[2]    An array containing the X and Y coordinates of the center point of the HUD.
 * @param roll         The roll angle used for rotating the line segment.
 * @param offsetY      The offset value to adjust the line segment vertically.
 * @param degreeString A character array to display the degree information on the HUD.
 */
void drawHudLongLine(int pointX[2], int pointY[2], int center[2], float roll, float offsetY, const __FlashStringHelper* degreeString)
{
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, 0);
}

/**
 * Draws a short line on the HUD (Heads-Up Display) based on the given parameters.
 *
 * @param pointX[2]    An array containing the X-coordinates of the two points defining the line segment.
 * @param pointY[2]    An array containing the Y-coordinates of the two points defining the line segment.
 * @param center[2]    An array containing the X and Y coordinates of the center point of the HUD.
 * @param roll         The roll angle used for rotating the line segment.
 * @param offsetY      The offset value to adjust the line segment vertically.
 * @param degreeString A character array to display the degree information on the HUD.
 */
void drawHudShortLine(int pointX[2], int pointY[2], int center[2], float roll, float offsetY, const __FlashStringHelper* degreeString)
{
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, 20);
}

/**
 * Sets the speed label in the HUD (Heads-Up Display).
 *
 * @param speedHud            The speed value to be displayed in the HUD. (Non-negative)
 * @param halfScreenSpeed  The Y-coordinate position on the screen where the speed label will be drawn.
 */
void setSpeedLabel(float speedHud, int halfScreenSpeed)
{
    if (speedHud < 0) {
        // Ensure the speed is non-negative
        speedHud = 0;
    }

    display.setCursor(10, halfScreenSpeed);
    char buffer[5];
    dtostrf(ramUsedPercent(), 5, 2, buffer);

    display.write(buffer);
}

/**
 * Draw the HUD leader lines.
 *
 * This function is responsible for drawing the leader lines in the HUD (Head-Up Display). It takes the
 * coordinates of two points, the center point, and the roll angle as input parameters. The leader lines
 * indicate specific angles and are drawn using the drawHudLongLine() and drawHudShortLine() methods.
 *
 * @param pointX An array containing the X-coordinates of the two points.
 * @param pointY An array containing the Y-coordinates of the two points.
 * @param center An array containing the X and Y coordinates of the center point.
 * @param roll The roll angle in degrees.
 */
void drawHudLeader(int pointX[2], int pointY[2], int center[2], float roll, float pitch)
{
    if((pitch < 30)  && (pitch > -30))
    {
      drawHudLongLine(pointX, pointY, center, roll, 0, F("0"));
    }

    // -------------------------------------------------------------------
    if((pitch < 10)  && (pitch > -50))
    {
      drawHudShortLine(pointX, pointY, center, roll, 15, F("-20"));
    }
    // -------------------------------------------------------------------

    if((pitch < 50)  && (pitch > -10))
    {
      drawHudShortLine(pointX, pointY, center, roll, -15, F("20"));
    }
    // -------------------------------------------------------------------

    if((pitch < 70)  && (pitch > 10))
    {
      drawHudShortLine(pointX, pointY, center, roll, -30, F("40"));
    }
    // -------------------------------------------------------------------

    if((pitch < -10)  && (pitch > -70))
    {
      drawHudShortLine(pointX, pointY, center, roll, 30, F("-40"));
    }
    // -------------------------------------------------------------------

    if((pitch < 90)  && (pitch > 20))
    {
      drawHudShortLine(pointX, pointY, center, roll, -45, F("60"));
    }
    // -------------------------------------------------------------------

    if((pitch < -30)  && (pitch > -90))
    {
      drawHudShortLine(pointX, pointY, center, roll, 45, F("-60"));
    }
    // -------------------------------------------------------------------

    if((pitch < 90)  && (pitch > 40))
    {
      drawHudShortLine(pointX, pointY, center, roll, -60, F("80"));
    }
    // -------------------------------------------------------------------

    if((pitch < -40)  && (pitch > -90))
    {
      drawHudShortLine(pointX, pointY, center, roll, 60, F("-80"));
    }
    //-------------------------------------------------------------------
}

/**
 * Draws the heads-up display (HUD) on the display screen.
 *
 * @param roll       - The roll value.
 * @param pitch      - The pitch value.
 */
void drawHud(int roll, int pitch)
{
    setUpText();

    display.clearDisplay();

    display.setRotation(0);

    int pointX[2] = {(display.width()-1) / 2 - 35,(display.height() * 2 -1) / 2 + pitch};
    int pointY[2] = {(display.width()-1) / 2 + 35, (display.height() * 2 -1) / 2 + pitch};
    int center[2] = {((display.width()-1) / 2), ((display.height() * 2 -1) / 2)};

    drawHudLeader(pointX, pointY, center, roll, pitch);

    setSpeedLabel(0, (display.height() -1) / 2);

    if(abs(roll_general) > 50) {
      display.setTextSize(2);
      display.setCursor(40, 0);
      display.print(F("BANK\n   ANGLE"));
    }

    display.display();
}

/**
 * Rotates a 2D point around a center point by a given angle.
 *
 * @param point      - The 2D point to rotate, represented as an array of size 2.
 * @param x          - The x-coordinate of the point to rotate.
 * @param y          - The y-coordinate of the point to rotate.
 * @param center_x   - The x-coordinate of the center point.
 * @param center_y   - The y-coordinate of the center point.
 * @param angle      - The angle in radians by which to rotate the point.
 */
void rotatePoint(int (& point)[2], int x, int y, int center_x, int center_y, float angle)
{
  point[0] = (x - center_x) * cos(angle) - (y - center_y) * sin(angle) + center_x;
  point[1] = (x - center_x) * sin(angle) + (y - center_y) * cos(angle) + center_y;
}

/**
 * Displays information about ArduinoHUD.
 *
 * This function clears the display and then writes information about ArduinoHUD,
 * including the project name, creator, and a brief description, on the display.
 */
void aboutInfo() {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setRotation(0);

  display.setCursor(0, 0);
  display.cp437(true);
  display.print(F("ArduinoHUD\n"));

  display.setTextSize(1);
  display.print(F("Little HUD made\nfor Arduino "));
  display.print(F("Duxy1996"));

  display.display();
  delay(1500);
}

extern unsigned int __heap_start;
extern void *__brkval;

int freeRam() {
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}

float ramUsedPercent() {
  return ((TOTAL_RAM - (freeRam() - 390)) * 100.0) / TOTAL_RAM;
}
