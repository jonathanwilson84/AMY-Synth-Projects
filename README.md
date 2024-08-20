# AMY-Synth-Projects
My experiments and projects using the AMY synthesis library

As I have been experimenting with mcu based synthesizer projects, I have been trying to get outside of the teensy audio library world.

I have been interested in ESP32, and have struggled to have success with mozzi audio library using I2S to an outboard dac, namely
the pcm5102 that I have become accustomed to using. I found the AMY synthesizer library, and have been working on getting it working
with the ESP32. Using the example ESP32 in the arduino IDE, I have been looking at the source code to figure out how it all works. 

As things go along, I will post new advancements as they come along, and I will do my best to make this repository organized enough to make sense
to those just starting out with this stuff, as I have had to learn alot over the last year or so to get to this point. 

Ill start at the beginning:

0.0  AMY DRONE SYNTH V1

Video link:  https://youtu.be/HmVCcdBEqVo?si=871RxQAUBjIRqKXD

My first working project with AMY. A simple 4 pot drone synth with 2 oscillators, one with a resonant lowpass filter. Very straight forward. Find the code in 
the folder marked with the title above. Made in Arduino IDE. You will have to install the AMY library to compile and run this. Wiring is in the sketch. 

the next step is midi input. This is something I have done before with teensy and mozzi audio libraries, so I am assuming I can translate that over, but 
we will see how it goes!


1.0 AMY POLYPHONIC SYNTH

So I've been working through getting midi going with AMY, and it ended up being pretty straightforward. I finally got a 6 voice polyphonic synth example, you can find it under 
amyPolyphony3.ino. It has a couple of controls, one for filter cutoff, one for waveform type, super simple. Adding more soon. 

My main perogotive now is to figure out what the end result of this will look like. Im thinking a unit with a two octave keyboard, small screen, and 4-6 rotary encoders. Maybe have
several different modes that have different control schemes based on different waveforms. We will see! I still have a lot of experimenting to do. Specifically, transitioning
to vscode and the esp-idf to access both cores of the esp32. Im thinking I can run most of the synth stuff on one core, and all of the periferals on the other core, so that
there is no interference, but Ive never worked with multiple cores before so I need to do some serious experimentation. 
