//----------------------------------------------------------------------------------------------------------------
// ****TO-DO:
//          - Convert average function to assembly
#include "FastLED.h"
//----------------------------------------------------------------------------------------------------------------
// Configuration Details
#define NUM_LEDS        60              // Total amount of LEDs
#define LED_STRIP_PIN   6               // Pin that controls the LED strip
#define ANALOG_PIN      0               // Pin that we read sound sensor values from
#define MIC_MIN         0.0             // Minimum mic value
#define MIC_MAX         737.0           // Maximum mic value
//----------------------------------------------------------------------------------------------------------------
// Macro Details
#define AVERAGE_LENGTH  5               // Amount of prev sensor values effecting the operating average
#define LONG_SECTOR     20              // Amount of prev sensor values used to determine a song's peak
#define HIGH            3               // Mneumonics | Enumeration
#define NORMAL          2               // ^^
#define MSECS           30*100          // How long the "current average" sound is held onto
#define CYCLES          MSECS / DELAY   // ^^
#define DEV_THRESHOLD   0.8             // Amount a sound reading is allowed to deviate from the average
#define DELAY           1               // Arduino loop delay
//----------------------------------------------------------------------------------------------------------------
// Data Structures
struct Time                             // Keep track how often & how long a specific mode lasts
{
  unsigned long timeStart;
  short times;
}

struct Color                            // How much to increment, decrement a color by each cycle
{
  int r;
  int g;
  int b;
}
//----------------------------------------------------------------------------------------------------------------
// Global Variables
int activeLEDS = NUM_LEDS;              // Number of LEDs to display to
int lightMode = 0;                      // Lighting mode (Reactive, Pulse, Static, etc.) ****NOT IN USE YET
int songMode = NORMAL;                  // Set color based on mode
unsigned long songAvg;                  // Average sound measurement for the last CYCLES
int iter = 0;                           // Number of iterations since song_avg was reset
float fadeScale = 1.2;                  // Speed LEDs fade at if not relit
CRGB leds[NUM_LEDS];                    // LED Strip array
int avgs[AVERAGE_LENGTH];               // "Normalize" input values instead of using sensor output directly
int longAvg[LONG_SECTOR];               // Longer sound average ^^
struct Time high;                       // Keep track of how long high mode lasts
struct Color color;                     // Color
//----------------------------------------------------------------------------------------------------------------
// Helper Functions
int     avg(int* avgs, int len);
void    insert(int val, int* avgs, int len);
float   scale( float origMin, float origMax, float start, float fin, float val, float curve);
void    visualize();
//----------------------------------------------------------------------------------------------------------------

CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, NUM_LEDS); // Initialize Fast LED
}

void loop()
{
  
}

//----------------------------------------------------------------------------------------------------------------
// Helper Functions
// Compute the average of an array
int avg(int* avgs, int len)
{
  int sum = 0;

  asm("ldi r16, len \n\t"   // Clear counting register
      "loop: \n\t"
      "inc r16 \n\t"        // Increase counting register
      // Add each array value to sum
      "cpi r16, 0 \n\t"          // Compare counting register with 0
      "brne loop \n\t"           // Repeat the loop if not equal to 0
      : "=r" (sum)          // Sum in register, write only
     );
}

// Insert a value into the array, shift it down if the first value of the array is already full
void insert(int val, int* avgs, int len)
{
  
}

// From Arduino.cc, scaling function with curve
float scale( float origMin, float origMax, float start, float fin, float val, float curve)
{ 
}

// Visualize 
void visualize()
{
  int avg;
  int longAvg;
  int mapped;
  int sensorValue;

  // Read actual sensor value
  sensorValue = analogRead(ANALOG_PIN);
}




