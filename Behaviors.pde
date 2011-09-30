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
