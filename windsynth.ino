#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=93,201
AudioSynthWaveformSineModulated sine_fm1;       //xy=270,201
AudioEffectFade          fade1;          //xy=418,204
AudioEffectChorus        chorus1;        //xy=579,203
AudioOutputAnalog        dac1;           //xy=752,202
AudioConnection          patchCord1(waveform1, sine_fm1);
AudioConnection          patchCord2(sine_fm1, fade1);
AudioConnection          patchCord3(fade1, chorus1);
AudioConnection          patchCord4(chorus1, dac1);
// GUItool: end automatically generated code

#define CHORUS_DELAY_LENGTH (32*AUDIO_BLOCK_SAMPLES)

float analogIn = 0;
float volume = 0;
float targetVolume;
float modulation = 0;
int frequency = 0;
int targetFrequency = 0;
int freqKnobIn = 0;
float modKnobIn = 0;
int formKnobIn = 0;
int mult = 0;
int carrierMult = 0;
int modulatorMult = 0;
int form = 0;
int oldform = 1;
int overblow = 0;
bool isSounding;
short delayLine[CHORUS_DELAY_LENGTH];

void setup() {
  AudioMemory(15);
  pinMode(1, INPUT_PULLUP);//thumb
  pinMode(7, INPUT_PULLUP);//top left
  pinMode(2, INPUT_PULLUP);//middle left
  pinMode(0, INPUT_PULLUP);//left bottom
  pinMode(5, INPUT_PULLUP);//right top
  pinMode(8, INPUT_PULLUP);//right middle
  pinMode(6, INPUT_PULLUP);//right bottom
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.frequency(440);
  waveform1.amplitude(0.8);
  sine_fm1.frequency(440*2);
  sine_fm1.amplitude(1.00);
  chorus1.begin(delayLine,CHORUS_DELAY_LENGTH,1);
  //chorus1.begin(delayLine,CHORUS_DELAY_LENGTH,2);
  Serial.begin(9800);
}

void loop() {
  analogIn = analogRead(A0);
  volume = 2 * (1 - (analogIn/1023));
  Serial.println(String(volume) + " " + String(modulatorMult) + "->" + String(carrierMult));
  
  formKnobIn = analogRead(A8);
  form = map(formKnobIn,0,1023,1,6);
  if(oldform != form){
    if(form == 1){
      waveform1.begin(WAVEFORM_SINE);  
    }
    else if(form == 2){
      waveform1.begin(WAVEFORM_SQUARE);
    }
    else if(form == 3){
      waveform1.begin(WAVEFORM_PULSE);
      waveform1.pulseWidth(0.875);
    }
    else if(form == 4){
      waveform1.begin(WAVEFORM_PULSE);
      waveform1.pulseWidth(0.75);
    }
    else if(form == 5){
      waveform1.begin(WAVEFORM_PULSE);
      waveform1.pulseWidth(0.125);
    }
    else{
      waveform1.begin(WAVEFORM_SAWTOOTH);  
    }
    oldform = form;
  }

  if(digitalRead(7)){
    if(digitalRead(2)){
      //C# 554.37
      targetFrequency = 554;
    }
    else{
      if(digitalRead(5)){
        //C 523.25
        targetFrequency = 523;
      }
      else{
        //high D 587.33
        targetFrequency = 587; 
      }
    }  
  }
  else if(digitalRead(2)){
    if(digitalRead(0)){
      //B 493.88
      targetFrequency = 493;  
    }
    else{
      //A# 466.16
      targetFrequency = 466;
    }
  }
  else if(digitalRead(0)){
    if(digitalRead(5)){
      //A 440.00
      targetFrequency = 440;  
    }
    else{
      //G# 415.30
      targetFrequency = 415;
    }
  }
  else if(digitalRead(5)){
    if(digitalRead(8)){
      //G 392.00
      targetFrequency = 392; 
    }
    else{
      //F 349.23
      targetFrequency = 349;
    }
  }
  else if(digitalRead(8)){
    if(digitalRead(6)){
      //F# 369.99
      targetFrequency = 370; 
    }
    else{
      //D# 311.13
      targetFrequency = 311;
    }
  }
  else if(digitalRead(6)){
    //E 329.63
    targetFrequency = 330;
  }
  else if(!digitalRead(6)){
    //low D 293.66  
    targetFrequency = 293;
  }

  if(volume < 0.4){//if(volume < 0.52){
      //TONGUE LAND
      //volume = 0;  
      if(isSounding == 1){
        //volume = 0.39;
        fade1.fadeOut(50);//50  
      }
      isSounding = 0;
  }
  else{
    if(isSounding == 0){
      fade1.fadeIn(1);//25  
    }  
    isSounding = 1;
  } 
  
  //if(volume < 1.15){//if(volume < 1.25){
    //targetFrequency = targetFrequency * 1;
    //overblow = 0;  
  //}
  //else if(volume < 1.30){
    //Serial.println("hey");
    //if(overblow == 1){
      //targetFrequency = targetFrequency * 2;
      //overblow = 1;  
    //}
    //else{
      //overblow = 0;
    //}
  //}
  //else{
    //targetFrequency = targetFrequency * 2;
    //overblow = 1;  
  //}

  if(!digitalRead(1)){
    frequency = targetFrequency * 2;  
  }
  else{
    frequency = targetFrequency;  
  }
  /**
  if(volume < 0.9){//0.9
    overblow = 0;  
  }
  else if(volume > 1.05){//else if(volume > 1.05){
    overblow = 1;  
  }
  
  if(overblow == 1){
    targetFrequency = targetFrequency * 2;  
  }  
  Serial.println(String(volume) + " " + String(overblow));
  /**
  if(overblow == 0){
    if((digitalRead(1)) || (abs(frequency - targetFrequency) < 9)){
      frequency = targetFrequency;
    }
    else{
      if(frequency < targetFrequency){
        frequency = frequency + 8;  
      }
      else if (frequency > targetFrequency ){
        frequency = frequency - 8;  
      }
      delay(25);
      //frequency = frequency * 2;//debug  
    }  
  }
  else{
    if((digitalRead(1)) || (abs(frequency - targetFrequency) < 17)){
      frequency = targetFrequency;
    }
    else{
      if(frequency < targetFrequency){
        frequency = frequency + 16;  
      }
      else if (frequency > targetFrequency ){
        frequency = frequency - 16;  
      }
      delay(25);
      //frequency = frequency * 2;//debug  
    }
  }
  **/

  //Serial.println(String(analogRead(A1)) + " " + String(analogRead(A2)) + " " + String(analogRead(A4)));
  modKnobIn = analogRead(A7);
  //if((modKnobIn > 13) && (modKnobIn < 1000)){
  modulation = (modKnobIn/1023);
    //modulation = map(modKnobIn,0,1023,0,1);
    //Serial.println(modulation);
  waveform1.amplitude(modulation);
  //}
  freqKnobIn = analogRead(A6);
  //if((freqKnobIn > 13) && (freqKnobIn < 1000)){
  mult = map(freqKnobIn,0,1023,1,11);
  switch (mult){
  case 1:
    carrierMult = 3;
    modulatorMult = 4;
    break;
  case 2:
    carrierMult = 1;
    modulatorMult = 4;
    break;
  case 3:
    carrierMult = 2;
    modulatorMult = 3;
    break;
  case 4:
    carrierMult = 1;
    modulatorMult = 3;
    break;
  case 5:
    carrierMult = 1;
    modulatorMult = 2;
    break;
  case 6:
    carrierMult = 1;
    modulatorMult = 1;
    break;
  case 7:
    carrierMult = 2;
    modulatorMult = 1;
    break;
  case 8:
    carrierMult = 3;
    modulatorMult = 1;
    break;
  case 9:
    carrierMult = 3;
    modulatorMult = 2;
    break;
  case 10:
    carrierMult = 4;
    modulatorMult = 1;
    break;
  case 11:
    carrierMult = 4;
    modulatorMult = 3;
    break;
  default:
    carrierMult = 1;
    modulatorMult = 1;
    break;
  }
  sine_fm1.frequency(frequency * carrierMult);
  waveform1.frequency(frequency * modulatorMult);
  if(overblow == 1){
    sine_fm1.amplitude(volume);//sine_fm1.amplitude(volume-0.5);
  }
  else{
    sine_fm1.amplitude(volume); 
  }
  //Serial.println("MOD VOL: " + String(modulation)+ "/" + String(modKnobIn) + " MULT: " + String(mult) + "//" + String(freqKnobIn) + " BREATH: " + String(volume));
}