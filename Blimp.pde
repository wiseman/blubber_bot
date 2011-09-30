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





  
