// Note: we have had to severly reduce the number of samples because we have run out of SRAM
// Analog amplitudes for a guitar string
char waveTable3 [] = { 
94, 116, 122, 112, 91, 66, 45, 32, 30, 39, 54, 72, 84, 86, 78, 64, 
49, 41, 43, 55, 73, 88, 97, 95, 82, 61, 36, 15, 5, 11, 33, 63, 94, 
116, 122, 112, 91, 66};

// Knocked off the end of the samples - ws still behaving odly.
//45, 32, 30, 39, 54, 72, 84, 86, 78, 64, 49, 
//41, 43, 55, 73, 88, 97, 95, 82, 61, 36, 15, 5, 11, 33, 63};

// frequency stepping increments
unsigned char increments [] = {
  13,
  0x1b+1,   // 42
  0x22+3,   // 54
  0x24+5,   // 57
  0x26+7,   // 60
  0x1b,   // 85
  0x1e   // 96 
};
/*
  0x22,   // 108
  0x24,   // 114
  0x26,   // 121
  0x29,   // 128
  0x2b,   // 136
  0x2e,   // 144
  0x30,   // 152
  0x33,   // 161
}; 
 */
 
#define NUM_SELECTIONS 7 // sizeof(increments)
#define NUM_SELECTIONS_1 3 // sizeof(increments)
#define NUM_SELECTIONS_2 4 // sizeof(increments)

// trick to parse out the bytes of a long
union {
  unsigned long L;
  unsigned char Q [4]; } waveSampler;
  
// step through all 22 selections
int nextSelection (int song_cycle) {
  switch (song_cycle) {
    case 1:
      if (selection_1 < NUM_SELECTIONS_1)
        return selection_1++;
        else return (selection_1 = 0);
        break;
    case 2:
      if (selection_2 < NUM_SELECTIONS_1 + NUM_SELECTIONS_2)
        return selection_2++ ;
        else return (selection_2 = NUM_SELECTIONS_1);
        break;
  }
}

// beep number from 0 to 7
void playWave (int number) {
  unsigned int increment;       
  unsigned char index;
  unsigned int frequency;
  unsigned int frequency1;
  unsigned int frequency2;
  
    increment = increments [number] * 4;
    waveSampler.L = 0;
    for (int j = 0; j < 30; j++) {
      waveSampler.L += increment;
      index = waveSampler.Q[1]; // byte 3
      frequency = waveTable3[index] * 2;
      frequency1 = frequency;
      frequency2 = frequency;
     
      for (int i = 0; i < 4; i++) {
        for (int h = 0; i < 37; i++) { // was 50
          digitalWrite (piezoPin, HIGH);
          delayMicroseconds (frequency1);
          digitalWrite (piezoPin, LOW);
          delayMicroseconds (frequency2);
        }
        frequency1 = frequency1 / 2; // crude decay mechanism
      }
      delay (50);
    }
}

