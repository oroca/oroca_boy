#include "Arduboy.h"
#include "audio.h"

const byte PROGMEM tune_pin_to_timer_PGM[] = { 3, 1 };
volatile byte *_tunes_timer1_pin_port;
volatile byte _tunes_timer1_pin_mask;
volatile int32_t timer1_toggle_count;
volatile byte *_tunes_timer3_pin_port;
volatile byte _tunes_timer3_pin_mask;
byte _tune_pins[AVAILABLE_TIMERS];
byte _tune_num_chans = 0;
volatile boolean tune_playing; // is the score still playing?
volatile unsigned wait_timer_frequency2;       /* its current frequency */
volatile boolean wait_timer_playing = false;   /* is it currently playing a note? */
volatile boolean tonePlaying = false;
volatile unsigned long wait_toggle_count;      /* countdown score waits */

// pointers to your musical score and your position in said score
volatile const byte *score_start = 0;
volatile const byte *score_cursor = 0;

// Table of midi note frequencies * 2
//   They are times 2 for greater accuracy, yet still fits in a word.
//   Generated from Excel by =ROUND(2*440/32*(2^((x-9)/12)),0) for 0<x<128
// The lowest notes might not work, depending on the Arduino clock frequency
// Ref: http://www.phy.mtu.edu/~suits/notefreqs.html
const uint8_t PROGMEM _midi_byte_note_frequencies[48] = {
16,17,18,19,21,22,23,24,26,28,29,31,33,35,37,39,41,44,46,49,52,55,58,62,65,
69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,
233,247
};
const unsigned int PROGMEM _midi_word_note_frequencies[80] = {
262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,
698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,
1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,
4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,
10548,11175,11840,12544,13290,14080,14917,15804,16744,17740,18795,19912,21096,
22351,23680,25088 };

/* AUDIO */

bool ArduboyAudio::audio_enabled = false;

void ArduboyAudio::on()
{
  audio_enabled = true;
}

bool ArduboyAudio::enabled()
{
  return audio_enabled;
}

void ArduboyAudio::off()
{
  audio_enabled = false;
}

void ArduboyAudio::saveOnOff()
{
  EEPROM.write(EEPROM_AUDIO_ON_OFF, audio_enabled);
}

void ArduboyAudio::begin()
{
  tune_playing = false;
  if (EEPROM.read(EEPROM_AUDIO_ON_OFF))
    on();
}

/* TUNES */

void ArduboyTunes::initChannel(byte pin)
{
}

void ArduboyTunes::playNote(byte chan, byte note)
{
}

void ArduboyTunes::stopNote(byte chan)
{
}

void ArduboyTunes::playScore(const byte *score)
{
  score_start = score;
  score_cursor = score_start;
  step();  /* execute initial commands */
  tune_playing = true;  /* release the interrupt routine */
}

void ArduboyTunes::stopScore (void)
{
  for (uint8_t i = 0; i < _tune_num_chans; i++)
    stopNote(i);
  tune_playing = false;
}

bool ArduboyTunes::playing()
{
  return tune_playing;
}

/* Do score commands until a "wait" is found, or the score is stopped.
This is called initially from tune_playcore, but then is called
from the interrupt routine when waits expire.
*/
/* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */
void ArduboyTunes::step()
{
}

void ArduboyTunes::closeChannels(void)
{
}

void ArduboyTunes::soundOutput()
{
}

void ArduboyTunes::tone(unsigned int frequency, unsigned long duration)
{
}


