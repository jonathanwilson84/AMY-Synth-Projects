/*

AMY additive synthesis library 

SIMPLE DRONE SYNTH 

Very simple drone synth that takes input from 4 10k pots 
Two oscillators with variable frequency control
filter on one oscillator demonstrates cutoff and resonance

This is step one of my experiments with AMY, and is for documentation purposes
I have been exploring options for audio synthesis tools on esp32, and this 
is the first one that I have gotten to work properly. 

Currently, this code should work with regular esp32 wroom boards, but YMMV

Connections:
frequencyPot -- GPIO4
cutoffPot -- GPIO2
resPot -- GPIO15
freq2Pot -- GPIO34

Uses pcm5102 dac module, with connections described below


Check here for full documentation:

https://github.com/shorepine/amy/tree/main
*/


#include <AMY-Arduino.h>
#include <I2S.h>
#include <Ewma.h>

AMY amy; //CREATE AMY OBJECT

//ADC_AVERAGE_LIBRARY_SETUP (Ewma)

Ewma adcFilter1(.01);
Ewma adcFilter2(.01);
Ewma adcFilter3(.05);

//SETUP_ANALOG_INPUT_PINS 

int frequencyPot = 4;
int cutoffPot = 2;
int resPot = 15; 
int freq2Pot = 34;

void setup() {

//INITIALIZE ANALOG_INPUT_PINS as input

pinMode(frequencyPot, INPUT);
pinMode(freq2Pot, INPUT);
pinMode(cutoffPot, INPUT);
pinMode(resPot, INPUT);

//for serial debug
Serial.begin(115200);

  // Set your I2S pins. Data/SD/DIN/DOUT, SCK/BLCK, FS/WS/LRCLK. 
  //With esp32 any pins can be set to be I2S pins. The defaults are provided so we will use those

  I2S.setDataPin(27); // 27
  I2S.setSckPin(26); // 26
  I2S.setFsPin(25); // 25
  I2S.begin(I2S_PHILIPS_MODE, AMY_SAMPLE_RATE, BYTES_PER_SAMPLE*8);

//INITIALIZE AMY OBJECT - turn on reverb and/or chorus by setting 1, turn off by setting 0
amy.begin(/* cores= */ 1, /* reverb= */ 1, /* chorus= */ 0);

//OSC1 SETUP - as far as I can figure out, each oscillator has to be initialzed as a struct that passes its data set to amy.default.event(), set parameters here for each osc

//FOR FIRST TEST, KEEP IT SIMPLE

  struct event e = amy.default_event();
  e.osc = 0;
  e.wave = SAW_UP;
  e.filter_type = 1;
  e.resonance = 1;
  e.velocity = 1;
//SEND OSC1 TO BE PROCESSED
  amy.add_event(e);

//OSC 2 SETUP - here we will define a different osc as event c. same setup as above

  struct event c = amy.default_event();
  c.osc = 2;
  c.wave = SAW_DOWN; //can be a variety of wave types, check docs
  c.filter_type = 1; //filter type 1 is lowpass
  c.resonance = 1; //set res to low
  c.velocity = 1; //velocity acts as noteOn message, must be set to start wave
  amy.add_event(c);

//SETUP REVERB
  config_reverb(1.f, .85f, .5f,3000);
}

}

void loop() {
//READ AND MAP POTS TO ACCEPTABLE RANGES - frequency is direct
int freq = analogRead(frequencyPot);
float filt1 = adcFilter1.filter(freq);
int freqM = map(filt1, 0, 4095, 10, 880);
float f = freqM/10.0f;

//cutoff is direct value

int cutoff = analogRead(cutoffPot);
float filt3 = adcFilter3.filter(cutoff);
int cutoffM = map(filt3, 0, 4095, 50, 8000);

//resonance is .5 - 16, over 10 produces glitches, even 10 produces glitches sometimes, try 8
int res = analogRead(resPot);
int resM = map(res, 0, 4095, 5, 100);
float resA = resM/10.0f;

//for some reason, to set the values we have to redefine the struct in the loop, can we change this?

  struct event e = amy.default_event();
  e.osc = 0;
  e.freq_coefs[COEF_CONST] = f;
  e.filter_freq_coefs[COEF_CONST] = cutoffM;
  e.resonance = resA;

//each time our values change, we must re send the current values to our amy object
  amy.add_event(e);

//OSC c input

int freq2 = analogRead(freq2Pot);
float filt2 = adcFilter2.filter(freq2);
int freq2M = map(filt2, 0, 4095, 10, 880);

   struct event c = amy.default_event();
   c.osc = 2;
   c.freq_coefs[COEF_CONST] = freq2M;
   amy.add_event(c);


  // In your loop you have to get the buffer of samples and then play it out your device
  short * samples = amy.render_to_buffer();
  I2S.write_blocking(samples, AMY_BLOCK_SIZE*AMY_NCHANS*BYTES_PER_SAMPLE);
}

}
