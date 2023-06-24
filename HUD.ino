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

  testdrawHud();  

  delay(1000);

  testdrawbitmap();
}

void loop() {
  testdrawHud();
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
 * Perform a test drawing of the HUD.
 * 
 * This function clears the display and performs a series of rotations and delays
 * to simulate the display of the HUD. It rotates the roll and pitch values within
 * specified ranges and delays between each rotation to control the speed. The purpose
 * of this test is to verify the functionality of the drawHud() method.
 */
void testdrawHud() {
  
  display.clearDisplay();

  for (int roll = 0; roll < 60; roll++) {
    delay(10);
    drawHud(roll, 0);
  }

  for (int roll = 60; roll > -45; roll--) {
    delay(10);
    drawHud(roll, 0);
  }

  for (int roll = -45; roll < 0; roll++) {
    delay(10);
    drawHud(roll, 0);
  }

  for (int pitch = 0; pitch < 45; pitch++) {
    delay(10);
    drawHud(0, pitch);
  }

  for (int pitch = 45; pitch > 0; pitch--) {
    delay(10);
    drawHud(0, pitch);
  }

  for (int pitch = 0; pitch < 45; pitch++) {
    delay(10);
    drawHud(pitch, pitch);    
  }
  
  delay(1000); // Pause for 2 seconds
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
void drawHudLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[], float deltaDrawLine)
{
    float pointA[2];
    float pointB[2];

    float pointAH[2];
    float pointBH[2];
    
    // Rotate pointX and pointY based on the roll angle and store the results in pointA and pointB
    rotatePoint(pointA, pointX[0] + deltaDrawLine, pointX[1] + offsetY, center[0], center[1], degToRad(roll));
    rotatePoint(pointB, pointY[0] - deltaDrawLine, pointY[1] + offsetY, center[0], center[1], degToRad(roll));

    // Calculate new points for displaying the degree string slightly above the line
    rotatePoint(pointAH, pointX[0] + deltaDrawLine, pointX[1] + offsetY - 3, center[0], center[1], degToRad(roll));
    rotatePoint(pointBH, pointY[0] - deltaDrawLine, pointY[1] + offsetY - 3, center[0], center[1], degToRad(roll));

    // Set the cursor position and display the degree string
    display.setCursor(pointB[0] - 5, pointB[1] + 2); 
    display.write(degreeString);

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
void drawHudLongLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[])
{
    float deltaDrawLine = 0;    
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, deltaDrawLine);
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
void drawHudShortLine(float pointX[2], float pointY[2], float center[2], float roll, float offsetY, char degreeString[])
{
    float deltaDrawLine = 20;    
    drawHudLine(pointX, pointY, center, roll, offsetY, degreeString, deltaDrawLine);
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

    char buffer[10];
    itoa(speedHud, buffer, 10);

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
void drawHudLeader(float pointX[2], float pointY[2], float center[2], float roll)
{
    char degreeString[] = "XXX";

    strcpy(degreeString, "0");
    
    drawHudLongLine(pointX, pointY, center, roll, 0, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-20");
    drawHudShortLine(pointX, pointY, center, roll, 10 * 1.5, degreeString);    
    // -------------------------------------------------------------------   
    strcpy(degreeString, "20");
    drawHudShortLine(pointX, pointY, center, roll, -10 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "40");
    drawHudShortLine(pointX, pointY, center, roll, -20 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-40");
    drawHudShortLine(pointX, pointY, center, roll, 20 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "60");
    drawHudShortLine(pointX, pointY, center, roll, -30 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-60");
    drawHudShortLine(pointX, pointY, center, roll, 30 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "80");
    drawHudShortLine(pointX, pointY, center, roll, -40 * 1.5, degreeString);
    // -------------------------------------------------------------------
    strcpy(degreeString, "-80");
    drawHudShortLine(pointX, pointY, center, roll, 40 * 1.5, degreeString);
    // -------------------------------------------------------------------
    // strcpy(degreeString, "90");
    // drawHudShortLine(pointX, pointY, center, roll, -45 * 1.5, degreeString);
    // -------------------------------------------------------------------
    // strcpy(degreeString, "-90");
    // drawHudShortLine(pointX, pointY, center, roll, 45 * 1.5, degreeString);
    // -------------------------------------------------------------------
}

/**
 * Draws the heads-up display (HUD) on the display screen.
 *
 * @param roll       - The roll value.
 * @param pitch      - The pitch value.
 */
void drawHud(float roll, float pitch)
{  
    setUpText();
    
    display.clearDisplay();

    display.setRotation(0);

    int hudSegmentLongitude = 35;
    
    double halfScreen = (display.height()-1) / 2 + pitch;
    double halfScreenSpeed = (display.height()-1) / 2;

    float startHudPointX = (display.width()-1) / 2 - hudSegmentLongitude;
    float startHudPointY = halfScreen;

    float endHudPointX = (display.width()-1) / 2 + hudSegmentLongitude;
    float endHudPointY = halfScreen;

    float centerX = ((display.width()-1) / 2);
    float centerY = ((display.height()-1) / 2);    

    float pointX[2] = {startHudPointX, startHudPointY};
    float pointY[2] = {endHudPointX, endHudPointY};
    float center[2] = {centerX, centerY};

    drawHudLeader(pointX, pointY, center, roll);

    randomNumber += random(3);
    randomNumber -= random(2);
    
    setSpeedLabel(randomNumber, halfScreenSpeed);

    if(roll > 50)
    {
      display.setTextSize(3);
      display.setCursor(30, 17);
      display.write("BANK\n ANGLE");
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
void rotatePoint(float (& point)[2], float x, float y, float center_x, float center_y, float angle)
{  
  float x_end = (x - center_x) * cos(angle) - (y - center_y) * sin(angle) + center_x;
  float y_end = (x - center_x) * sin(angle) + (y - center_y) * cos(angle) + center_y;

  point[0] = x_end;
  point[1] = y_end;
}

/**
 * Displays information about ArduinoHUD.
 * 
 * This function clears the display and then writes information about ArduinoHUD,
 * including the project name, creator, and a brief description, on the display.
 */
void aboutInfo()
{
  display.clearDisplay();

  display.setTextSize(2);         
  display.setTextColor(SSD1306_WHITE);      
  display.setRotation(0);                           // Set the display rotation to 0 degrees (no rotation)

  const char projectName[] = "ArduinoHUD\n";                    // Project name
  const char creatorName[] = "Duxy1996";                      // Creator's name
  const char description[] = "Little HUD made\nfor Arduino";  // Project description

  display.setCursor(0, 0);                          
  display.cp437(true);                               
  display.write(projectName);                        

  display.setTextSize(1);                           
  display.write("\n");                               
  display.write(description);                        
  display.write("\n\n");                             
  display.write(creatorName);                        

  display.display();                                
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
