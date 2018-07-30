void RunSimpleColorCorrectionTest() {
  static uint8_t b = 64;
  for(int i=0; i < NUM_LEDS && i < 150; i+=15) {
    leds(i,i+3) = CRGB(b,0,0);
    leds(i+5,i+8) = CRGB(0,b,0);
    leds(i+10,i+13) = CRGB(0,0,b);
  }
  FastLED.show();

  //if(++b > 100) { if(++b > 200) b++; }
  //delay(60);
  
  Serial.println("Enter brightness value (1-255)");
  while(Serial.available() == 0) ;
  String s = Serial.readString();
  s.trim();
  b = s.toInt();
}

void RunGradientColorCorrectionGammaTest() {
  static uint8_t b = 128;
  for(int i =  0; i < 150; i++) {
    leds[i] = CHSV((2*i)%256, 255, b);
    leds[383-i] = GammaCorrect(ReverseGammaCorrect(CHSV(2*i, 255, b)));
    if(leds[i] != leds[383-i]) {
      Serial.print(String(i) + ": " + String(leds[i].r) + "," + String(leds[i].g) + "," + String(leds[i].b) + " -> ");
      Serial.println(String(leds[383-i].r) + "," + String(leds[383-i].g) + "," + String(leds[383-i].b));
    }
  }

  FastLED.show();
  
  Serial.println("Enter brightness value (1-255)");
  while(Serial.available() == 0) ;
  String s = Serial.readString();
  s.trim();
  b = s.toInt();
}

void RunGradientColorCorrectionGammaTest2() {
  const uint8_t brightness = 128;
  static uint8_t hue_starting_point = 0;
  for(int i = 0; i < NUM_LEDS; i+=3) {
    CHSV temp = CHSV((hue_starting_point + i/3) % 256,255,brightness);
    leds[i] = temp;
    leds[i+1] = GammaCorrect(ReverseGammaCorrect(temp));
    leds[i+2] = CRGB::Black;
    if(leds[i] != leds[i+1]) {
      Serial.print(String(i) + ": " + String(leds[i].r) + "," + String(leds[i].g) + "," + String(leds[i].b) + " -> ");
      Serial.println(String(leds[i+1].r) + "," + String(leds[i+1].g) + "," + String(leds[i+1].b));
    }
  }

  FastLED.show();
  
  Serial.println("Enter starting hue (0-255)");
  while(Serial.available() == 0) ;
  String s = Serial.readString();
  s.trim();
  hue_starting_point = s.toInt();
}

void RunGammaConfigTest() {
  const uint8_t ss = 50; // step size between gradients
  static uint8_t b = 64;
  static float gammaR = 1.9;//1.60;//1.75;//1.8;//1.65;//1.15;
  static float gammaG = 1.9;//1.75;//1.90;//1.6;//2.1;//1.65;
  static float gammaB = 1.8;//1.80;//2.00;//3.1;//3.1;//2.85;

  fill_gradient_RGB(leds,   0, CRGB(255,0,0),  ss, CRGB(0,255,0));
  fill_gradient_RGB(leds,  ss, CRGB(0,255,0),2*ss, CRGB(0,0,255));
  fill_gradient_RGB(leds,2*ss, CRGB(0,0,255),3*ss, CRGB(255,0,0));
  fill_gradient_RGB(leds,3*ss, CRGB(255,0,0),4*ss, CRGB(0,255,0));
  fill_gradient_RGB(leds,4*ss, CRGB(0,255,0),5*ss, CRGB(0,0,255));
  fill_gradient_RGB(leds,5*ss, CRGB(0,0,255),6*ss, CRGB(255,0,0));
  
  for(int i = 0; i <= 128; i++) {
    //leds[i] = GammaCorrect(leds[i]);
    leds[i].r = applyGamma_video(leds[i].r, gammaR);
    leds[i].g = applyGamma_video(leds[i].g, gammaG);
    leds[i].b = applyGamma_video(leds[i].b, gammaB);
  }

  leds %= b;
  FastLED.show();

  Serial.println("brightness: " + String(b) + "\tGammas: " + String(gammaR) + "\t" + String(gammaG) + "\t" + String(gammaB));
  Serial.println("Enter 'r' 'g' 'b' to edit gamma. Or a number to edit brightness (1-255).");
  while(Serial.available() == 0) ;
  String s = Serial.readString();
  s.trim();
  
  if(s == "r") {
    Serial.println("Enter new red gamma value. (Current is " + String(gammaR) + ")");
    while(Serial.available() == 0) ;
    s = Serial.readString();
    s.trim();
    gammaR = s.toFloat();
  }
  else if(s == "g") {
    Serial.println("Enter new green gamma value. (Current is " + String(gammaG) + ")");
    while(Serial.available() == 0) ;
    s = Serial.readString();
    s.trim();
    gammaG = s.toFloat();    
  }
  else if(s == "b") {
    Serial.println("Enter new blue gamma value. (Current is " + String(gammaB) + ")");
    while(Serial.available() == 0) ;
    s = Serial.readString();
    s.trim();
    gammaB = s.toFloat();
  }
  else {
    b = s.toInt();
  }
}

