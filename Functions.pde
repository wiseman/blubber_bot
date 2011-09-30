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
