#include "analogWave.h"

analogWave wave(DAC);

#define NOTE_A4 69
#define FREQ_A4 440

int tonic = 65;

int melody[] = {
  1,3,5,1,
  1,3,5,1,
  5,6,8,5,6,8,
  8,10,8,6,5,1,
  8,10,8,6,5,1,
  1, -4, 1,
  1, -4, 1,
};

int rhythm[] {
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 2,
  4, 4, 2, 
  8, 8, 8, 8, 4, 4,
  8, 8, 8, 8, 4, 4,
  4, 4, 2,
  4, 4, 2
};

int noteCounter = 0;

int bpm = 120;

float beatDuration = 60.0 / bpm * 1000;

void setup(){
  wave.sine(10);
}

void loop(){
  int currentNote = melody[noteCounter] + tonic;
  float frequncy = FREQ_A4 * pow(2, ((currentNote - NOTE_A4) / 12.0));

  float noteDuration = beatDuration * (4.0 / rhythm[noteCounter]);

  wave.freq(frequncy);

  delay(noteDuration * 0.85);
  wave.stop();
  delay(noteDuration * 0.15);

  noteCounter++;

  noteCounter = noteCounter % 32;

}