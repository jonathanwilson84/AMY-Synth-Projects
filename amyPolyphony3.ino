#include <AMY-Arduino.h>
#include <I2S.h>
#include <Ewma.h>
#include <MIDI.h>

#define NUM_VOICES 6

struct VoiceStatus {
    bool active;
    byte note;
    unsigned long lastUsed;
} voiceStatus[NUM_VOICES];

AMY amy;
MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte note, byte velocity);
void handleNoteOff(byte channel, byte note, byte velocity);


int pot1 = 15;
int pot2 = 2; 
int pot3 = 4;
int pot4 = 34;

void setup() {
    Serial.begin(115200);
    MIDI.begin(1);
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    I2S.setDataPin(27);
    I2S.setSckPin(26);
    I2S.setFsPin(25);
    I2S.begin(I2S_PHILIPS_MODE, AMY_SAMPLE_RATE, BYTES_PER_SAMPLE*8);

pinMode(pot1, INPUT);

    amy.begin(1, 1, 1);

    // Initialize all voices as inactive
    for (int i = 0; i < NUM_VOICES; i++) {
        voiceStatus[i].active = false;
        voiceStatus[i].note = 0;
        voiceStatus[i].lastUsed = 0;
    }
}

void loop() {
    MIDI.read();

int attack = 30;
float attackvel = .7f;
int decay = 200; 
float sustain = .5f;
int release = 1000;
float releaseval = 0.f;


int val1 = analogRead(pot1);
int cutoff = map(val1, 0, 4095, 50, 8000);

int val2 = analogRead(pot2);
int waveform = map(val2, 0, 4095, 0, 6);



Serial.println(val1);

struct event e = amy.default_event();
    for (int i = 0; i < NUM_VOICES; ++i) {
            e.osc = i; 
            e.wave = waveform; 
            e.filter_type = 1;
            e.filter_freq_coefs[COEF_CONST] = val1;
            e.resonance = 3.f;
            sprintf(e.bp0, "%d,%.1f,%d,%.1f,%d,%.1f", attack, attackvel, decay, sustain, release, releaseval);
            amy.add_event(e);
    }
    
    short* samples = amy.render_to_buffer();
    I2S.write_blocking(samples, AMY_BLOCK_SIZE * AMY_NCHANS * BYTES_PER_SAMPLE);
}

void handleNoteOn(byte channel, byte note, byte velocity) {
    for (int i = 0; i < NUM_VOICES; ++i) {
        if (!voiceStatus[i].active) {
            // Activate the voice
            voiceStatus[i].active = true;
            voiceStatus[i].note = note;
            voiceStatus[i].lastUsed = millis();

            struct event e = amy.default_event();
            e.osc = i;  // Assign this voice to a specific oscillator
            e.midi_note = note;
            e.velocity = velocity / 127.0f;
            e.wave = SAW_UP;  // Example waveform
            e.filter_type = 1;  // Example filter type
            amy.add_event(e);
            break;
        }
    }
    Serial.println(note);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
    for (int i = 0; i < NUM_VOICES; ++i) {
        if (voiceStatus[i].note == note && voiceStatus[i].active) {
            // Deactivate the voice
            voiceStatus[i].active = false;

            struct event e = amy.default_event();
            e.osc = i;  // Target the same oscillator used for this voice
            e.midi_note = note;
            e.velocity = 0;
            amy.add_event(e);
        }
    }
}
