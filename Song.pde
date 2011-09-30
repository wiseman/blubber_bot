#define c  957    // 1045 Hz
#define d  850    // 1176 Hz
#define e  759    // 1318 Hz
#define f  716    // 1397 Hz
#define g  637    // 1570 Hz
#define a  568    // 1761 Hz = 440 * 4
#define b  507    // 1972 Hz
#define C  478    // 2092 Hz
#define D  425    // 2353 Hz
#define E  379    // 2639 Hz
#define F  358    // 2793 Hz
#define G  318    // 3145 Hz
#define A  284    // 3521 Hz
#define B  253    // 3953 Hz


// notes, broken up into measures, with beats and pauses aligned
int melody[] = {  e, e,  e, E, e, e,      e,  e, e, E, D, C,      e, e, e, E, e, e,        // notes
 //               8, 8,  4, 4, 8, 10,    10, 10, 8, 8, 8, 8,      10, 8, 4, 4, 8, 10,      // note beats
 //                 8,  8, 0, 0, 0, 4,     4,  4,  0, 0, 0, 0,       4, 8, 0, 0, 0,  4,    // pause beats
                
                 g, E, a, E, b, E, C, E, D, g, E, g, F, g, G, g,
//               4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    
                  e, e, g, e, a, a,        d, d, f, d, g, g, f,         e, e, g, e, a, a, d,
//                8, 8, 4, 4, 8, 8,        8, 8, 4, 4, 8, 8, 8,         8, 8, 4, 4, 8, 8, 8,
//                  0, 8, 0, 0, 0, 16,       0, 8, 0, 0, 0, 0, 8,        0,  8, 0, 0, 0, 0, 8,
                  
                  d, d, e, d, g, d, g, f, e, e, g, e, a, a,            d, d, f, d, g, g, f,
 //               8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 4, 4, 8, 8,            8, 8, 4, 4, 8, 8, 8,
 //                 0, 8, 0, 0, 0, 0, 0, 8, 0, 8, 0, 0, 0, 16,           0, 8, 0, 0, 0, 0, 8,
                  
                  e, e, g, e, a, a,        d, d, f, d, a, g, a, g, f,   e, e, e, E, e, e,
//                8, 8, 4, 4, 8, 8,        8, 8, 4, 4, 4, 4, 8, 4, 8,   8, 8, 4, 4, 8, 10,
//                  0, 8, 0, 0, 0, 16,       0, 8, 0, 0, 0, 0, 8, 0, 4,   8, 8, 0, 0, 0, 4,

                  e, e, e, E, d, c,        e, e, e, E, e, g, c, F,      G, g, F, g, e, g, d, g, f, g, e, g, d, g, c, f, 
//               10,10, 8, 8, 8, 8,        10,10,4, 4, 8, 4, 4, 8,      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,   
//                 4, 4,  0, 0, 0, 0,       4, 8, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

                  g,  g, g};
//               16, 16, 16};
//                  0, 0,  0};
      
// how long to hold a note                            
char beats[]  = { 
               8, 8,  4, 4, 8, 10,    10, 10,  8, 8, 8, 8,      10, 8, 4, 4, 8, 10, 
               4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
               8, 8, 4, 4, 8, 8,        8, 8, 4, 4, 8, 8, 8,         8, 8, 4, 4, 8, 8, 8,
               8, 8, 4, 4, 4, 4, 8, 8,  8, 8, 4, 4, 8, 8,            8, 8, 4, 4, 8, 8, 8,
               8, 8, 4, 4, 8, 8,        8, 8, 4, 4, 4, 4, 8, 4, 8,   8, 8, 4, 4, 8, 10,
               10,10, 8, 8, 8, 8,        10,10,4, 4, 8, 4, 4, 8,      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
               16, 16, 16};

// pause beats between notes               
char pauses [] = {   
               8,  8, 0, 0, 0,  4,     4,  4,  0, 0, 0, 0,        4, 8, 0, 0, 0,  4,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 8, 0, 0, 0, 16,       0, 8, 0, 0, 0, 0, 8,        0,  8, 0, 0, 0, 0, 8,
               0, 8, 0, 0, 0, 0, 0, 8,  0, 8, 0, 0, 0, 16,           0, 8, 0, 0, 0, 0, 8,
               0, 8, 0, 0, 0, 16,       0, 8, 0, 0, 0, 0, 8, 0, 4,   8, 8, 0, 0, 0, 4,
               4, 4,  0, 0,  0, 0,       4, 8, 0, 0, 0, 0, 0, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
               0, 0,  0};
                  
// assorted globals
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.
#define tempo  8000 // Set overall tempo
//int rest_count = 100;

// Initialize
int tone = 0;
int beat = 0;
int pause = 0;

// play a note as a set of equal high and low pulses (square wave)
// version 4a: uses all int arithmetic
void playNote() {
  // bury the setup in the inter note silence
  int pulse_width = tone >> 1;
  int elapsed_time = 0;
  int duration = beat * (tempo>>3); // Set up timing
  tone = tone>>3;
  
  digitalWrite (ledPinBlue, HIGH);
  elapsed_time = 0;
    while (elapsed_time < duration) {
      digitalWrite(piezoPin,HIGH);
      delayMicroseconds(pulse_width);
      digitalWrite(piezoPin, LOW);
      delayMicroseconds(pulse_width);
      elapsed_time += tone;
    }
   digitalWrite (ledPinBlue, LOW); 
  }
  
// play a pause using the same code as a note so that the beats stay aligned
// make the minimum pause 1 beat to separate the notes slightly
void playPause() {
  long elapsed_time = 0;
  long duration = max(pause, 2) * tempo; // Set up timing
  int pulse_width = tone >> 1;
  
  digitalWrite (ledPinRed, HIGH);
  elapsed_time = 0;
    while (elapsed_time < duration) {
      digitalWrite(piezoPin,LOW);
      delayMicroseconds(pulse_width);
      digitalWrite(piezoPin, LOW);
      delayMicroseconds(pulse_width);
      elapsed_time += tone;
    }
    digitalWrite (ledPinRed, LOW);
} 

// Play the song
void playSong () {
  for (int i=0; i<MAX_COUNT; i++) {
    tone  = melody[i];
    beat  = (int)beats[i];
    pause = (int)pauses[i];

    playNote(); 
    playPause();
    }
}                           


