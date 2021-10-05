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
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const float W = 128;
static const float H = 64;
static const float R1 = 1;
static const float R2 = 2;
static const float K2 = 20;
static const float K1 = W * K2 * 2.5 / (8 * (R1 + R2));

#define theta_space 0.3
#define phi_space 0.3

static const float midX = W / 2;
static const float midY = H / 2;

void rotX(float *x, float *y, float *z, float A)
{
  //*x = x
  float oy = *y;
  float oz = *z;
  *y = oy * cos(A) + (oz)*sin(A);
  *z = -(oy)*sin(A) + (oz)*cos(A);
}

void rotY(float *x, float *y, float *z, float phi)
{
  float ox = *x;
  float oz = *z;
  *x = (ox)*cos(phi) + (oz)*sin(phi);
  //*y = y;
  *z = -(ox)*sin(phi) + (oz)*cos(phi);
}

void rotZ(float *x, float *y, float *z, float B)
{
  float ox = *x;
  float oy = *y;
  *x = ox * cos(B) + oy * sin(B);
  *y = -ox * sin(B) + oy * cos(B);
  //*z = z;
}
void setup()
{
  Serial.begin(9600);
  // put your setup code here, to run once:
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("NOT OK"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  Serial.println(F("OK"));
  display.display();
}

void frame(float A, float B)
{
  for (float t = 0; t < 2 * PI; t += theta_space)
  {
    float cosT = cos(t);
    float sinT = sin(t);
    for (float p = 0; p < 2 * PI; p += phi_space)
    {
      float x, y, z;
      x = (R2 + R1 * cosT);
      y = R1 * sinT;
      z = 0;
      rotY(&x, &y, &z, p);
      rotX(&x, &y, &z, A);
      rotZ(&x, &y, &z, B);
      z += K2;
      float ooz = 1 / z;
      int xp = midX + (x * K1 * ooz);
      int yp = midY + (y * K1 * ooz);
      display.drawPixel(xp, yp, SSD1306_WHITE);
    }
  }
}

static float A = 0;
static float B = 0;
void loop()
{
  // put your main code here, to run repeatedly:
  display.clearDisplay();
  frame(A, B);
  display.display();
  A += 0.03;
  B += 0.07;
}
