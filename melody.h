#ifndef MELODY_H
#define MELODY_H

enum PianoNoteFrequency {
  NOTE_G3 = 392,
  NOTE_A4 = 440,
  NOTE_B4 = 493,
  NOTE_C4 = 523,
};

int melody[] = {
  NOTE_C4,
  NOTE_G3,
  NOTE_A4,
  NOTE_B4,
};

bool play_melody(long interval, bool restart = false);

#endif /* MELODY_H */
