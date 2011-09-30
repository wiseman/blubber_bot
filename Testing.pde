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
