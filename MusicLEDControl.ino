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
void    checkHigh(int avg);
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
// Check wether lamp should enter high mode or revert to normal
void checkHigh(int avg)
{
  if (avg > (songAvg / iter * 1.1))
  {
    if (high.times != 0)
    {
      if (millis() - high.timeStart > 200.0)
      {
        high.times = 0;
        songMode = NORMAL;
      }
      else
      {
        high.timeStart = millis();
        high.times++;
      }
    }
    else
    {
      high.times++;
      high.timeStart = millis();
    }
  }

  if (high.times > 30.0 && millis() - high.timeStart < 50.0)
  {
    songMode = HIGH;
  }
  else if (millis() - high.timeStart > 200.0)
  {
    high.times = 0;
    songMode = NORMAL;
  }
}

// Compute the average of an array
// Written in assembly because I am a sick man
int computeAvg(int* avgs, int len)
{
  int sum = 0;
  unsigned int iter;

  asm("ldi r16, len \n\t"         // Clear counting register
      "ldi r30, lo8(avgs) \n\t"
      "ldi r31, hi8(avgs) \n\t"
      "lds r24, (iter) \n\t"      // 
      "add r30, r24 \n\t"         // Low Byte
      "adc r31, r1 \n\t"          // High Byte
      "ldi r25, %0"               // Load sum into register r25
      "loop: \n\t"                
      "inc r16 \n\t"              // Increase counting register
      // Add each array value to sum
      // Load array base address into register pair r31:r30
      "ld r18, Z \n\t"            // Load value at current index
      "add r25, r18 \n\t"         // Add r25 = r25 + r18, adding current index to running sum
      "ld r18, Z+ \n\t"           // Auto increment
      "cpi r16, 0 \n\t"           // Compare counting register with 0
      "brne loop \n\t"            // Repeat the loop if not equal to 0
      : "+r" (sum)                // Sum in register, write only
     );
}

// Insert a value into the array, shift it down if the first value of the array is already full
void insert(int val, int* avgs, int len)
{
  for (int iter = 0; iter < len; iter++)
  {
    if (avgs[iter] = -1)
    {
      avgs[iter] = val;
      return;
    }
  }

  for (int iter = 0; iter < len; iter++)
  {
    avgs[iter - 1] = avgs[iter];
  }

  avgs[len - 1] = val;
}

// From Arduino.cc, scaling function with curve
float scale( float origMin, float origMax, float start, float fin, float val, float curve)
{
  float origRange = 0;
  float newRange = 0;
  float zeroRefCurvVal = 0;
  float normalizedCurvVal = 0;
  float outVal;
  boolean flag = false;

  // Limit curve range
  if (curve > 10)
  {
    curve = 10;
  }
  if (curve < -10)
  {
    curve = -10;
  }

  // Scale and Invert
  curve = curve * -0.1;
  // Convert linear scale to log scale
  curve = pow(10, curve);

  // Check if val is in range
  if (val < origMin)
  {
    val = origMin;
  }
  if (val > origMax)
  {
    val = origMax;
  }

  // Zero referrence the values
  origRange = origMax - origMin;

  if (start > fin)
  {
    newRange = fin - start;
  }
  else
  {
    newRange = start - fin;
    flag = true;
  }

  zeroRefCurvVal = val - origMin;
  // Normalize to between 0 - 1
  normalizedCurvVal = zeroRefCurvVal / origRange; 

  // Check if original min is greater than original max
  if (origMin > origMax)
  {
    return 0;
  }

  if (flag == false)
  {
    outVal = (pow(normalizedCurvVal, curve) * newRange) + start;
  }
  else
  {
    // Invert the ranges
    outVal = start - (pow(normalizedCurvVal, curve) * newRange);
  }

  return outVal;
}

// Visualize the music boiiii
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

  // Check if values exceed defined threshold
  if ((avg - mapped) > (avg * DEV_THRESHOLD))
  {
    return;
  }

  // Insert the new average values
  insert(mapped, avgs, AVERAGE_LENGTH);
  insert(avg, longAvg, LONG_SECTOR);

  // Compute the average sensor value for the song
  songAvg += avg;
  iter++;

  if (iter > CYCLES)
  {
    songAvg = songAvg / iter;
    iter = 1;
  }

  longAvg = computeAvg(longAvg, LONG_SECTOR);

  // Check what the songMode should be
  checkHigh(longAvg);

  // Set the colors based on the songMode
  if (songMode == HIGH)
  {
    fadeScale = 3.0;
    
    color.r = 5;
    color.g = -1;
    color.b = 3;
  }
  else if (songMode == NORMAL)
  {
    fadeScale = 2.0;

    color.r = -1;
    color.b = 2;
    color.g = 1;
  }

  activeLEDS = scale(MIC_MIN, MIC_MAX, 0.0, (float)NUM_LEDS, (float)avg, -1);

  // Set the different LEDS, control for too high and too low
  for (int iter = 0; iter < NUM_LEDS; iter++)
  {
    if (iter < activeLEDS)
    {
      if (leds[iter].r + color.r > 255)
      {
        leds[iter].r = 255;
      }
      else if (leds[iter].r + color.r < 0)
      {
        leds[iter].r = 0;
      }
      else
      {
        leds[iter].r = leds[iter].r + color.r;
      }

      if (leds[iter].g + color.g > 255)
      {
        leds[iter].g = 255;
      }
      else if (leds[iter].g + color.g < 0)
      {
        leds[iter].g = 0;
      }
      else
      {
        leds[iter].g = leds[iter].g + color.g;
      }

      if (leds[iter].b + color.b > 255)
      {
        leds[iter].b = 255;
      }
      else if (leds[iter].b + color.b < 0)
      {
        leds[iter].b = 0;
      }
      else
      {
        leds[iter].b = leds[iter].b + color.b;
      }
    }
    else
    {
      leds[i] = CRGB(leds[iter].r / fadeScale, leds[iter].g / fadeScale, leds[iter].b / fadeScale);
    }
  }

  FastLED.show();
}

