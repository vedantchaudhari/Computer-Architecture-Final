//----------------------------------------------------------------------------------------------------------------
// ****TO-DO: Include pertinent headers
//            - Fast LED Library
//            - Convert average function to assembly
#include "FastLED.h"
//----------------------------------------------------------------------------------------------------------------
// Configuration Details
#define NUM_LEDS      60        // Total amount of LEDs
#define LED_STRIP_PIN 6         // Pin that controls the LED strip
#define ANALOG_PIN_   0         // Pin that we read sound sensor values from
#define MIC_MIN       0.0       // Minimum mic value
#define MIC_MAX       737.0     // Maximum mic value
//----------------------------------------------------------------------------------------------------------------
// Macro Details
#define AVERAGE_LENGTH  5               // Amount of prev sensor values effecting the operating average
#define LONG_SECTOR     20              // Amount of prev sensor values used to determine a song's peak
#define HIGH            3               // Mneumonics
#define NORMAL          2               // ^^
#define MSECS           30*100          // How long the "current average" sound is held onto
#define CYCLES          MSECS / DELAY   // ^^
#define DEV_THRESHOLD   0.8             // Amount a sound reading is allowed to deviate from the average
#define DELAY           1               // Arduino loop delay
//----------------------------------------------------------------------------------------------------------------
// Global Variables

//----------------------------------------------------------------------------------------------------------------
// TEST FUNCTION
//----------------------------------------------------------------------------------------------------------------
// Helper Functions
int avg(int* avgs, int len);
void insert(int val, int* avgs, int len);
float scale( float origMin, float origMax, float start, float fin, float val, float curve);
void visualize();
//----------------------------------------------------------------------------------------------------------------

#define audio_reader_input_pin 14
#define LED_output_pin 0
#define clap_threshold 100

CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(9600);
  pinMode(audio_reader_input_pin, INPUT);
  pinMode(LED_output_pin, OUTPUT);

  FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, NUM_LEDS); // Initialize Fast LED
}

void loop()
{
//  int audio_reading = analogRead(audio_reader_input_pin);
//  Serial.print("Sound level: " + audio_reading);
//  if (audio_reading > clap_threshold)
//  {
//    digitalWrite(LED_output_pin, HIGH);
//    Serial.print("HIGH");
//  }
//  if (audio_reading < clap_threshold)
//  {
//    digitalWrite(LED_output_pin, LOW);
//    Serial.print("LOW");
//  }
    leds[0] = CRGB::White; FastLED.show(); delay(30);
    leds[0] = CRGB::Black; FastLED.show(); delay(30);

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::White; FastLED.show(); delay(30);
      leds[i] = CRGB::Black; FastLED.show(); delay(30);
    }
}

//----------------------------------------------------------------------------------------------------------------
// Helper Functions
int avg(int* avgs, int len)
{
  int sum = 0;

  asm("ldi r16,  \n\t"  // Clear counting register
      "loop: \n\t"
      "inc r16 \n\t"    // Increase counting register
      "cpi r16, 10"     // Compare counting register with length
      ""
     );
}





