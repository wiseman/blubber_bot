#include "WProgram.h"
void interruptTest();
void lightSeeker();
void ledSeeker();
void dreamSeeker();
void pwm(int motorPin, int dutyCycle, int repeats);
void pwm2(int motorPin1, int dutyCycle1, int motorPin2, int dutyCycle2, int repeats, boolean doBump);
void step(int stepNumber);
void playNote();
void playPause();
void playSong();
void testPins();
void testFunctionality();
int nextSelection(int song_cycle);
void playWave(int number);
/* Blimp behavior program v 0.5
5 volt power
 */

/*
Features
  All basic featuresincorporated
  Music tables slimmed down until they fit into SRAM (100+ bytes free as of not)
  todo:
  Sun triggers phone flasher - raise phoneSensitivity from 350 to 900 - slightly light sensitive
  PhoneSensitivity works at 1000; leave at 900; gives us 10% margin of safety for photocell variation.
  Speed up songs - play 39 notes
  Speed up danceseeker - play only 2 song cycles & halve delays; raise motor timing from 2*250 to 2*300
  speedFwd from 300 to 400
  Reverse speed from 600 to 800
  Blue LED on with high lightseeker; off with rest; as is (on) with dance; song pulses red and blue.
  Lowlight Blue is PWM in pause; raise duration to 500
  Change Dreaming LED to Blue; slower rate of change
  Add random dreaming modes: Waves 3-7, Vib, Coo
  Use Wave modes 0-2 for Dance
  Up vib power to full - get a resonance
  Rest counter set to 5.5 min
  Set Run time to 5.5 min
  Pulse Blue more evenly in LowLight
  Add red led on when reverse
  Cut vib times in half (8x -> 4x of "normal")
  Increased min song internote delay to 4 beats (trying to get blue led to go of)
  Turn Blue LED on during Dance motor phase
  todo:
  Serial Links
 */
char copyright [] = {'V', '1', '.', '0', 'J', 'B', 'e', 'r', 'k', 'C','2','0','0','7'};
 // motor controller left/right fwd/rev pin set
#define mcLPinFwd 11   // HW pin ?? OK 
#define mcLPinRev 8   // HW pin ?? OK 
#define mcRPinFwd 10  // HW pin ?? OK  
#define mcRPinRev 7  // HW pin ?? OK 
// cell phone vibrator
                    // unknown
// blinking LEDs
#define phonePin 1 // HW pin 24 OK analog
#define ledPinBlue 3 // HW pin 5 OK
#define ledPinRed 4 // HW pin 6 OK
#define bumpPin 5  // HW pin 11
#define vibPin 6 // HW pin 12 OK
#define piezoPin 9 // HW pin 15 OK
// photocell
#define photoCellPinLeft 3 // was 2, change for production v1.0 circuit board
#define photoCellPinRight 2 // was 3, change for production v1.0 circuit board
 
// bumpPin not in output pin list 
#define pins 8
int pin [8] = {ledPinRed, ledPinBlue, vibPin, piezoPin, mcLPinFwd, mcRPinFwd, mcLPinRev, mcRPinRev };

int pcLeftRaw = 0;
int pcRightRaw = 0;
int pcLeft = 0;
int pcRight = 0;
boolean bumpDetected = false;
boolean revDirection = false;
int fwdDirection = 0;
int pcMax = 0;
int speedFwd = 0;
int lightLevel = 0;
int currentMood = 0;
int priorMood = 0;
int nextMood = 0;
unsigned char restCounter = 0;
boolean phoneDetected = false; // not detected
boolean wasDancing = false;
boolean testMode = false;
int selection_1 = 0; // wavetable index for dance
int selection_2 = 0; // index for dreaming
// controls whether the pwm2 function can do bump detection
// enabling it during reverse may result in bobbing behavior
#define NoBump  false
#define DoBump  true
#define NoPhone  false
#define DoPhone  true
boolean phoneMode = false; // ignore phone
#define Left 1
#define Right 0
#define HighLight 1
#define LowLight 0
#define RestCounts 10

// Mood Names
#define Calibration 0
#define LightSeeker 1
#define LightSeeker1 11
#define Dancing 2
#define Singing 3
#define Resting 4
#define Dreaming 5
#define Flocking 6 // networked behavior
#define Dreaming 7 // tbd
#define Testing 8

// int moodTable [6] [6];
#define LightCounts 2
#define DanceCounts 3
#define SongCounts  2
#define phoneSensitivity 900 // analog read > this to trigger a phone detect
//
//int photoCellValueLeft = 1;
//int photoCellValueRight = 1;                             // counter pin

// Prototypes
  boolean readBump ();
  boolean readPhone ();
  void vib (int, int);
  void vibLoud (int);
  void coo ();
  void piezo (int, int);
  void pwm2 (int, int, int, int, int, boolean);
  void pwm  (int, int, int);
  void lightSeeker ();
  void danceSeeker ();
  void songSeeker  ();
  void ledSeeker   ();
  void dreamSeeker ();
  void interruptTest ();
  void testPins    ();
  void testFunctionality ();
  void pulse (int, int);
  void step (int);
  void playSong ();
  void playBeep (int);
  int nextSelection (int);
  long time = millis();
  long randNum = 0;
  
  
  void setup()
{
  for (int i = 0; i < pins; i++) pinMode(pin[i], OUTPUT);       // sets the digital pin as output
  pinMode(bumpPin, INPUT);
  for (int i = 0; i < pins; i++) digitalWrite(pin[i], LOW);     // turn output pins off
  // set up initial moods and flags
  currentMood = Resting;  // initial mood
  priorMood = currentMood;
  nextMood = currentMood;
  restCounter = 41; // must match the test just inside loop, below, to start up in rest.
  selection_1 = 0; // songs 0, 1, 2
  selection_2 = 3; // songs 3, 4, 5, 6, 7
  pcLeft = 1;
  pcRight = 1;
  bumpDetected = false;
  phoneDetected = false;
  wasDancing = false;
  pulse (ledPinRed, 1000);
  pulse (ledPinBlue, 1000);
  // detect test mode on startup if bump pin is closed
  if (readBump()) {
    bumpDetected = true;
    testMode = true;
    nextMood = Testing;
  }
  randomSeed(millis());
}
  
// Main Behavior Loop

void loop()
{
   priorMood = currentMood;
   currentMood = nextMood;
   phoneDetected = false;
   // special dispatch off of the restmode counter - rest every 41 cycles (5.5 min)
   if ((restCounter % 41 ) == 0) currentMood = Resting; // careful - RestCounter%8 == 0 catches here too
   if (restCounter == 0) currentMood = LightSeeker;    // but this is where we want it to go at reset & startup
  
  switch (currentMood) {
    case LightSeeker:
     phoneMode = NoPhone;
     for (int j = LightCounts; j > 0; j--) lightSeeker ();
     phoneDetected = false;
     
    case LightSeeker1:
     restCounter += 1;
     nextMood = LightSeeker1;
     phoneMode = DoPhone;
     lightSeeker(); 
     if (phoneDetected && !wasDancing) nextMood = Dancing;
     if (phoneDetected &&  wasDancing) nextMood = Singing;
     break; 
     
    case Dancing:
     phoneMode = NoPhone;
     restCounter = 0;
     for (int j = DanceCounts; j > 0; j--) danceSeeker(); 
     nextMood = LightSeeker1;
     wasDancing = true;
     break;
     
    // add chirp in dark mode
    case Singing:
     phoneMode = NoPhone;
     restCounter = 0;
     for (int j = SongCounts; j > 0; j--) songSeeker(); 
     nextMood = LightSeeker1;
     wasDancing = false;
     break;
     
    case Resting:
     nextMood = Resting;
     phoneMode = DoPhone;
     bumpDetected = false;
     wasDancing = false;
     restCounter += 1;
     digitalWrite (ledPinBlue, LOW);
     ledSeeker();
     if ((restCounter % 41) == 0) nextMood = Dreaming; // 8 sec times the remainder - about 5.5 min
     if (bumpDetected || phoneDetected) {
       nextMood = LightSeeker1;
       restCounter = 0; }
     break;
     
    case Dreaming:
      dreamSeeker();
      nextMood = Resting;
      break; 
      
    case Flocking:
     break;
     
    case Calibration:
     break;
     
     case Testing:
       nextMood = LightSeeker;
       while (testMode) {
         delay(1000);
         testPins ();
         delay(1000);
         testFunctionality ();
         testMode = readBump(); // allows escape if bump pin goes silent
       }
     break;
  }

}

void interruptTest ()
{
  digitalWrite (ledPinBlue, HIGH);
  delay (100);
  digitalWrite (ledPinBlue, LOW);
}





  

/*
  Behavior LightSeeker
  Behavior: Steers towards the light by proportionally activating the motor contralateral to the photocell
  Sensitivities:
    light - stimulates PWM activity of contralateral motor via the 1630 brudge chips
    bump - triggers Reversal behavior
    cell phone - switches main behaviors
  Indicators:
    Blue LED is on/blinking
    Light sensitive motor bursts
  Bright Light - fast motor speeds, LED on continuously
  Dim Light - longer, slower motot pulses, LED blinks
  Version 0_20, 2007-May-06, BDH: Baseline release
  */
  
void lightSeeker () {
  int i; 
  int blueLight = LOW;
    // read the photocells
  pcLeftRaw  = analogRead(photoCellPinLeft);
  pcRightRaw = analogRead(photoCellPinRight);
  
  // non-linear motor response to photocell (pc) input
  // Add 50 to enhance low light activity on the motors.
  pcRight = 60 + pcRightRaw/32 + (pcRightRaw/8)*(pcRightRaw/8); // quadratic power function
  pcLeft =  60 + pcLeftRaw/32  + (pcLeftRaw/8) *(pcLeftRaw/8);
  
  // change forward speed modes for low and high light levels
  pcMax = max (pcLeftRaw, pcRightRaw);
  if (pcMax > 100)                  // indor incandescent lighting baseline
    { speedFwd = 400/4;
      lightLevel = HighLight; }
    else                            // pretty dark     
    {  speedFwd = 1000/4;             // need more gain in low light levels
       lightLevel = LowLight; }
  
  // Control the LED
  if (lightLevel == HighLight)
    //pwm (ledPinBlue, 500, 500);            // low light blinks
    digitalWrite (ledPinBlue, HIGH);  // high light is on
  
  // reverse if bumped - alternate motors to help back out of corners
  if (bumpDetected) {
          digitalWrite (ledPinRed, HIGH);
          bumpDetected = false;            // reset bump flag
          revDirection = !revDirection;    // alternate direction
          if (revDirection == Left)        // and back out on one motor
            pwm2 (mcRPinRev, 1, mcLPinRev, 500, 800, DoBump);       // left motor
            else  pwm2 (mcRPinRev, 500, mcLPinRev, 1, 800, DoBump);  // right motor
            digitalWrite (ledPinRed, LOW);
           }

     // Otherwise, full speed ahead towards the light
     // pwm2 is sensitive to the bump switch - mode enabled
     // Break up the PWM into 4 segments so we can blink the lowLight LED
     else {  if (lightLevel == LowLight) digitalWrite (ledPinBlue, LOW);
             pwm2 (mcRPinFwd, pcLeft*4, mcLPinFwd, pcRight*4, speedFwd, DoBump);
             if (lightLevel == LowLight) digitalWrite (ledPinBlue, HIGH);
             pwm2 (mcRPinFwd, pcLeft*4, mcLPinFwd, pcRight*4, speedFwd, DoBump);
             if (lightLevel == LowLight) digitalWrite (ledPinBlue, LOW);
             pwm2 (mcRPinFwd, pcLeft*4, mcLPinFwd, pcRight*4, speedFwd, DoBump);
             if (lightLevel == LowLight) digitalWrite (ledPinBlue, HIGH);
             pwm2 (mcRPinFwd, pcLeft*4, mcLPinFwd, pcRight*4, speedFwd, DoBump);
             if (lightLevel == LowLight) digitalWrite (ledPinBlue, LOW);}
        
  // Between motor bursts, reset the bump flag and delay 3 sec
  // Break delay up into cycles so we can monitor the bump and cell phone
  for (i = 0; i < 200; i++) {
    delay(30); 
    if ((i % 32) == 0) {
        blueLight = !blueLight;
        if (lightLevel == LowLight) digitalWrite (ledPinBlue, blueLight); // continue lowLight level blink
        }
      
    if (bumpDetected || readBump()) {   // bump is always enabled in LightSeeker
      if (!bumpDetected) vibLoud(300); // new bump - buz for a moment
      bumpDetected = true;;             // remember the bump
      break; }
    if (phoneMode && readPhone()) {     // Phone mode must be externally enabled
      phoneDetected = true;
      break; }
  }
  if (lightLevel == LowLight) digitalWrite (ledPinBlue, LOW);
}

/*
 Behavior DanceSeeker
  Behavior: spins in a counterclockwise circle - intended for multiple synchronized blimps
  Sensitivities:
    light - no
    bump - no
    cell phone - no
  Indicators:
    LED?
    Vibrator: on between motor cycles
    Piezo - during pauses
  Version 0_20, 2007-May-06, BDH: Baseline release
*/
  void danceSeeker () {
  int i; 
  int selection = 0;
  
  digitalWrite (ledPinBlue, HIGH);
  
  // fan pulses, bracketed by buzzers
  // buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz+fan, buzz
  for (int i = 5; i> 0; i--) {
    digitalWrite (vibPin, HIGH);
    pwm2 (mcRPinFwd, 800, mcLPinRev, 800, 300, NoBump);
    digitalWrite (vibPin, LOW);
    if (i > 1) pwm2 (mcRPinFwd, 800, mcLPinRev, 800, 300, NoBump); // vibrate on all but last
  }
  digitalWrite (ledPinBlue, LOW);  
  // delay 3 sec and play a song from the wave table
    delay (500);
    playWave (nextSelection(1));
    delay (1000);
    playWave (nextSelection(1));
    delay (500);
    //playBeep (nextSelection());
    //delay (500);
  }

/*
 Behavior SongSeeker
  Behavior: Play JB's Blimp melody (c) 2007
  Activity not inserted yet
  Version 0_20, 2007-May-06, BDH: Baseline release
*/  
  void songSeeker  (){
   playSong();
   delay (1000);
  }
 
// resting behavior - for now, just blink red and wait for a bump
void ledSeeker() {
  digitalWrite (ledPinRed, HIGH);
  delay (500);
  digitalWrite (ledPinRed, LOW);
  for (int i = 0; i < 200; i++) {
    delay(30); // seemingly the only 4Mhz correction to be required
    if (bumpDetected || readBump()) {
      if (!bumpDetected) vibLoud(300); // new bump
      bumpDetected = true;; // remember the bump
      break; }
    if (phoneMode && readPhone()) {
      phoneDetected = true;
      break; }
    }
  }

// Use a random number 
void dreamSeeker () {
    // for (int p = 1; p < 10; p++){ // test mode
      randNum = random(4, 10); // songs 4 - 7
      digitalWrite (ledPinBlue, LOW);
      for (int i = 1; i < 1000; i+=2) pwm (ledPinBlue, i, 1);
      digitalWrite (ledPinBlue, HIGH);
      switch (randNum) {
        case 4:
        case 5:
        case 6:
        case 7:
          playWave (nextSelection(2));
          break;  
       case 8:
          vibLoud(500);
          break;
       case 9:
       case 10: // there is an off by bug in here, somewhere
          coo ();
          break;
      }
      for (int i = 1000; i > 1; i-=2) pwm (ledPinBlue, i, 1);
      digitalWrite (ledPinBlue, LOW);
      delay(1000);   // }  // test mode
}

// improved code - does not underflow
// may want to set a minimum pwm for moptors so they don't stall

void pwm (int motorPin, int dutyCycle, int repeats)
  {
   int i; 
      // limit dutyCycle to 1 - 100 range
   int cycle1 = max (1, min (dutyCycle, 1023)); //(dutyCycle > 1023) ? 1023 : dutyCycle;
   int cycle2 = 1024 - cycle1;         // (cycle1 < 1) ? 1 : 1024 - cycle1;
   for (i = 1; i <= repeats; i++) {
      digitalWrite ( motorPin, HIGH);
      delayMicroseconds (cycle1); 
      digitalWrite ( motorPin, LOW);
      delayMicroseconds (cycle2); 
     }
  }
  
  // run both motors in parallel using PWM
  // add a noBump arg to enable (0) or disable (1) bump detection
  // use a minimum pulst of 10 us (TBD) to get motors turning over

void pwm2 (int motorPin1, int dutyCycle1, int motorPin2, int dutyCycle2, int repeats, boolean doBump)
  {
      // limit dutyCycle to 1 - 100 range
   int i;
   int pulse1On = max (10, min (dutyCycle1, 1023)); //(dutyCycle > 1023) ? 1023 : dutyCycle;
   int pulse1Off = 1024 - dutyCycle1;         // (cycle1 < 1) ? 1 : 1024 - cycle1;
   int pulse2On = max (10, min (dutyCycle2, 1023)); //(dutyCycle > 1023) ? 1023 : dutyCycle;
   int pulse2Off = 1024 - dutyCycle2;         // (cycle1 < 1) ? 1 : 1024 - cycle1;
   for (i = 1; i <= repeats; i++) {
      if (doBump && readBump()) {
        bumpDetected = true; // remember the bump
        break; }
      digitalWrite ( motorPin1, HIGH);
      digitalWrite ( motorPin2, HIGH);
      if ( pulse1On < pulse2On ) {
          delayMicroseconds (pulse1On); 
          digitalWrite ( motorPin1, LOW); 
          delayMicroseconds ( pulse2On - pulse1On +1 );
          digitalWrite ( motorPin2, LOW); 
          delayMicroseconds ( 1025 - pulse1On); }
          else {
          delayMicroseconds (pulse2On); 
          digitalWrite ( motorPin2, LOW);  
          delayMicroseconds ( pulse1On - pulse2On +1);
          digitalWrite ( motorPin1, LOW); 
          delayMicroseconds ( 1025 - pulse2On); }
     }
  }
  
 // utility function for vibrator motor
  void vib (int power, int duration) {
    pwm (vibPin, power, duration);
  }
  
   // utility function for vibrator motor
  void vibLoud (int duration) {
    digitalWrite (vibPin, HIGH);
    delay (duration * 4);
    digitalWrite (vibPin, LOW);
  }
  
// utility pulse function
  void pulse (int pin, int time) {
    int halfPulse = time >>1;
    digitalWrite (pin, HIGH);
    delay (halfPulse);
    digitalWrite (pin, LOW);
    delay (halfPulse);
    }
  
  boolean readBump () {
    if (digitalRead(bumpPin) == 0) {
      restCounter = 0; // global
      return true;
    }
    else return false;
  }
  
  boolean readPhone () {
    if ((analogRead(phonePin) > phoneSensitivity))
    return true;
    else return false;
  }

void step (int stepNumber) {
  for (int i=0; i<stepNumber; i++) {pulse (ledPinRed, 1000);} // STEP 
}

 // Piezo Buzz 
  void piezo (int period, int duration) {
  int i; 
  // piezo 500Hz buzz test
  for (i = 0; i < duration; i++) {
      digitalWrite(piezoPin, HIGH);   // sets the LED on
      delayMicroseconds(period);
      digitalWrite(piezoPin, LOW);   // sets the LED on
      delayMicroseconds(period);
    }
  }

// 
  void coo () {
    for (int i = 200; i < 500; i += 5) piezo (i, 5);
    for (int i = 500; i > 200; i -= 5) piezo (i, 5);
    for (int i=0; i<3; i++) {
      delay (100);
      for (int i = 100; i < 500; i += 20) piezo (i, 5);
    }
  }

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



/*
  Pin test mode
  Turn each output pin on for 1 second
  ledPinRed, ledPinBlue, vibPin, piezoPin, mcLPinFwd, mcRPinFwd, mcLPinRev, mcRPinRev
  */
void testPins () {
    // pulse each defined pin
  for (int i = 0; i < pins; i++) {
   digitalWrite(pin[i], HIGH);   // sets the LED on
   delay(500) ;
   digitalWrite(pin[i], LOW);
   delay(100);
  }
}

void testFunctionality () {
  // STEP 1
  // test LED and phone detector
  // No phone: led's blink together
  // Phone: led's blink alternately
  step(1); // STEP 1
  int redLedState = LOW;
  int blueLedState = LOW;
  for (int i=1; i<20; i++) {
    redLedState = LOW;
    if (readPhone()) 
       blueLedState = !redLedState;
       else blueLedState = redLedState;
    delay(250);
    digitalWrite (ledPinRed, redLedState);
    digitalWrite (ledPinBlue, blueLedState);
    delay(250);
    redLedState = !redLedState;
    blueLedState = !blueLedState;
    digitalWrite (ledPinRed, redLedState);
    digitalWrite (ledPinBlue, blueLedState);
  }
    digitalWrite (ledPinRed, LOW);
    digitalWrite (ledPinBlue, LOW);  
    delay(1000);
    
  // STEP 2
  // Vibrator for 1 sec
  step(2); // STEP 2
  pulse (vibPin, 2000);
  delay (1000);
  
  // STEP 3
  // piezo at 3kHz
  step(3); // STEP 3
  for (int i = 0; i < 3000; i++) {
   digitalWrite(piezoPin, HIGH);   // sets the LED on
   delayMicroseconds (333);
   digitalWrite(piezoPin, LOW);
   delayMicroseconds (333);
    }
   delay (1000);
    
    // STEP 4
    // bump
    // 10 sec of bump->red pulses bracketed by blue pulses
    step(4); // STEP 4
    pulse (ledPinBlue, 1000);
    for (int i=1; i<100; i++) {
     digitalWrite(ledPinRed, readBump());
     delay (100);
    }
    digitalWrite(ledPinRed, LOW);
    pulse (ledPinBlue, 1000);
    delay(1000);
   
   // STEP 5
   // Motors
   step(5); // STEP 5
   digitalWrite (ledPinRed, HIGH);
   digitalWrite (ledPinBlue, LOW);
   pwm (mcRPinFwd, 500, 500);
   delay(500);
   pwm (mcRPinRev, 500, 500);
   delay(500);
   digitalWrite (ledPinRed, LOW);
   digitalWrite (ledPinBlue, HIGH);
   pwm (mcLPinFwd, 500, 500);
   delay(500);
   pwm (mcLPinRev, 500, 500);
   digitalWrite (ledPinRed, LOW);
   digitalWrite (ledPinBlue, LOW);
   delay(1000);
   
   // STEP6
   // photocells
   step(6); // STEP 6
   digitalWrite (ledPinRed, HIGH);
   digitalWrite (ledPinBlue, HIGH);
   bumpDetected = false;
   while (!bumpDetected) {
   pcLeftRaw  = analogRead(photoCellPinLeft);
   pcRightRaw = analogRead(photoCellPinRight);
   pcRight = 60 + pcRightRaw/32 + (pcRightRaw/8)*(pcRightRaw/8); // quadratic power function
   pcLeft =  60 + pcLeftRaw/32  + (pcLeftRaw/8) *(pcLeftRaw/8);
   pwm2 (mcRPinFwd, pcLeft*4, mcLPinFwd, pcRight*4, 500, NoBump);
   bumpDetected = readBump();
   }
}

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

