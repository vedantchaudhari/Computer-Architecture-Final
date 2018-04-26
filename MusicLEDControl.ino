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
};

struct Color                            // How much to increment, decrement a color by each cycle
{
  int r;
  int g;
  int b;
};
//----------------------------------------------------------------------------------------------------------------
// Global Variables
int activeLEDS = NUM_LEDS;              // Number of LEDs to display to
int lightMode = 0;                      // Lighting mode (Reactive, Pulse, Static, etc.) ****NOT IN USE YET
int songMode = NORMAL;                  // Set color based on mode
unsigned long songAvg;                  // Average sound measurement for the last CYCLES
int iter = 0;                           // Number of iterations since song_avg was reset
float fadeScale = 1.2;                  // Speed LEDs fade at if not relit
CRGB leds[NUM_LEDS];                    // LED Strip array
int avgs[AVERAGE_LENGTH] = {-1};        // "Normalize" input values instead of using sensor output directly
int longAvg[LONG_SECTOR] = {-1};        // Longer sound average ^^
struct Time high;                       // Keep track of how long high mode lasts
struct Color color;                     // Color
//----------------------------------------------------------------------------------------------------------------
// Helper Functions
int     computeAvg(int* avgs, int len);
void    insert(int val, int* avgs, int len);
float   scale( float origMin, float origMax, float start, float fin, float val, float curve);
void    visualize();
//----------------------------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, NUM_LEDS); // Initialize Fast LED
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 255, 0); 
  }
	FastLED.show(); 
	delay(1000); 	

	//bootstrap average with some low values
	for (int i = 0; i < AVERAGE_LENGTH; i++) {	
		insert(250, avgs, AVERAGE_LENGTH);
	}

	//Initial values
	high.times = 0;
	high.timeStart = millis();
 	color.r = 0;	
	color.g = 0;
  color.b = 1;
}

void loop()
{
}

//----------------------------------------------------------------------------------------------------------------
// Helper Functions
// Compute the average of an array
int computeAvg(int* avgs, int len)
{
  int sum = 0;

  asm("ldi r16, len \n\t"         // Clear counting register
      "loop: \n\t"
      "inc r16 \n\t"              // Increase counting register
      // Add each array value to sum
      
      "cpi r16, 0 \n\t"           // Compare counting register with 0
      "brne loop \n\t"            // Repeat the loop if not equal to 0
      : "=r" (sum)                // Sum in register, write only
      : "r" (avgs)               // Array, read + write
     );
}

// Insert a value into the array, shift it down if the first value of the array is already full
void insert(int val, int* avgs, int len)
{
  
}

// From Arduino.cc, scaling function with curve
float scale( float origMin, float origMax, float start, float fin, float val, float curve)
{
  float origRange = 0;
  float newRange = 0;
  float zeroRefCurvVal = 0;
  float normalizedCurvVal = 0;
}

// Visualize 
void visualize()
{
  int avg;
  int longAvg;
  int mapped;
  float sensorValue;

  // Read actual sensor value
  sensorValue = (float)analogRead(ANALOG_PIN);

  // Check if sensor value is 0, if it is then discard
  if (sensorValue == 0)
  {
    return;
  }

  // Smoothing, discard readings that deviate too much from the prev avg
  mapped = scale(MIC_MIN, MIC_MAX, MIC_MIN, MIC_MAX, sensorValue, 2.0);
  avg = computeAvg(avgs, AVERAGE_LENGTH);
}




