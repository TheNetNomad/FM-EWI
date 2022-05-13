#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Smoothed.h>

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
Smoothed <float> volumeSensor;
float modulation = 0;
float frequency = 0;
float targetFrequency = 0;
float oldTargetFrequency = 0;
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
int timer = 0;
int vibratoDelay = 0;
int vibratoTable[24] = {0,1,2,3,4,5,6,5,4,3,2,1,0, -1, -2, -3, -4, -5, -6, -5, -4, -3, -2, -1};
int vibratoPhase = 0;

#define BUTTON_THUMB 0
#define BUTTON_ONE 1
#define BUTTON_TWO 2
#define BUTTON_THREE 3
#define BUTTON_FOUR 4
#define BUTTON_FIVE 5
#define BUTTON_SIX 6
#define SWITCH_ONE 7
#define PEDAL_ONE 8
#define LED 13

void setup() {
  AudioMemory(15);
  pinMode(BUTTON_THUMB, INPUT_PULLUP);
  pinMode(BUTTON_ONE, INPUT_PULLUP);
  pinMode(BUTTON_TWO, INPUT_PULLUP);
  pinMode(BUTTON_THREE, INPUT_PULLUP);
  pinMode(BUTTON_FOUR, INPUT_PULLUP);
  pinMode(BUTTON_FIVE, INPUT_PULLUP);
  pinMode(BUTTON_SIX, INPUT_PULLUP);
  pinMode(SWITCH_ONE, INPUT_PULLUP);
  pinMode(PEDAL_ONE, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.frequency(440);
  waveform1.amplitude(0.8);
  sine_fm1.frequency(440*2);
  sine_fm1.amplitude(1.00);
  chorus1.begin(delayLine,CHORUS_DELAY_LENGTH,1);
  //chorus1.begin(delayLine,CHORUS_DELAY_LENGTH,2);
  volumeSensor.begin(SMOOTHED_AVERAGE, 60);
  Serial.begin(9800);
}

void loop() {
  if(digitalRead(SWITCH_ONE)){
    volumeSensor.add(analogRead(A0));
    volume = 2 * (1 - (volumeSensor.get()/1023));
  }  
  else{
    volume = 0.75;
  }
  
  //Serial.println(String(volume) + " " + String(modulatorMult) + "->" + String(carrierMult) + digitalRead(PEDAL_ONE));
  
  formKnobIn = analogRead(A8);
  form = map(formKnobIn,0,1023,1,7);
  if(oldform != form){
    if(form == 1){
      waveform1.begin(WAVEFORM_SINE);  
    }
    else if(form == 2){
      waveform1.begin(WAVEFORM_TRIANGLE);
    }
    else if(form == 3){
      waveform1.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    }
    else if(form == 4){
      waveform1.begin(WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);
    }
    else if(form == 5){
      waveform1.begin(WAVEFORM_SAWTOOTH);
    }
    else if(form == 6){
      waveform1.begin(WAVEFORM_SAWTOOTH_REVERSE);
    }
    else if(form == 7){
      waveform1.begin(WAVEFORM_SQUARE);
    }
    oldform = form;
  }

  if(digitalRead(BUTTON_ONE)){
    if(digitalRead(BUTTON_TWO)){
      //C# 554.37
      targetFrequency = 554.37;
    }
    else{
      if(digitalRead(BUTTON_FOUR)){
        if(digitalRead(BUTTON_FIVE)){
          //C 523.25
          targetFrequency = 523.25;
        }
        else{
          if(digitalRead(BUTTON_SIX)){
            //high F# 739.99
            targetFrequency = 739.99;  
          }
          else{
            //high D# 622.25
            targetFrequency = 622.25; 
          }
        }
      }
      else{
        if(digitalRead(BUTTON_FIVE)){
          if(digitalRead(BUTTON_SIX)){
            //high F 698.46
            targetFrequency = 698.46; 
          }
          else{
           //high D# 622.25
           targetFrequency = 622.25; 
          }
        }
        else if(digitalRead(BUTTON_SIX)){
          //high E 659.25
          targetFrequency = 659.25;
        }
        else{
          //high D 587.33
          targetFrequency = 587.33;
        } 
      }
    }  
  }
  else if(digitalRead(BUTTON_TWO)){
    if(digitalRead(BUTTON_THREE)){
      //B 493.88
      targetFrequency = 493.88;  
    }
    else{
      //A# 466.16
      targetFrequency = 466.16;
    }
  }
  else if(digitalRead(BUTTON_THREE)){
    if(digitalRead(BUTTON_FOUR)){
      //A 440.00
      targetFrequency = 440.00;  
    }
    else{
      //G# 415.30
      targetFrequency = 415.30;
    }
  }
  else if(digitalRead(BUTTON_FOUR)){
    if(digitalRead(BUTTON_FIVE)){
      //G 392.00
      targetFrequency = 392.00; 
    }
    else{
      //F 349.23
      targetFrequency = 349.23;
    }
  }
  else if(digitalRead(BUTTON_FIVE)){
    if(digitalRead(BUTTON_SIX)){
      if(digitalRead(BUTTON_SIX)){
        //F# 369.99
        targetFrequency = 369.99; 
      }
      else{
        //D# 311.13
        targetFrequency = 311.13; 
      }
    }
    else{
      //D# 311.13
      targetFrequency = 311.13;
    }
  }
  else if(digitalRead(BUTTON_SIX)){
    //E 329.63
    targetFrequency = 329.63;
  }
  else if(!digitalRead(BUTTON_SIX)){
    //low D 293.66  
    targetFrequency = 293.66;
  }

  if(targetFrequency != oldTargetFrequency){
    vibratoDelay = 0;
    vibratoPhase = 0;
    oldTargetFrequency = targetFrequency;  
  }
  

  if(volume < 0.35){
      if(isSounding == 1){
        fade1.fadeOut(50);
        digitalWrite(LED, LOW);
      }
      isSounding = 0;
  }
  else{
    if(isSounding == 0){
      fade1.fadeIn(50);//25
      digitalWrite(LED,HIGH); 
      vibratoDelay = 0; 
      vibratoPhase = 0;
    }  
    isSounding = 1;
  } 
  
  if(!digitalRead(BUTTON_THUMB)){
    targetFrequency = targetFrequency * 2;  
  }

  timer++;
  if(timer > 20){
    timer = 0;  
  }
  
  if(timer == 0){
    if(vibratoDelay < 75){
      vibratoDelay++;  
    }
    else{
      vibratoPhase++;
      if(vibratoPhase == 24){
        vibratoPhase = 0;  
      }  
    }
  }
  
  if(digitalRead(PEDAL_ONE)){
    if((timer == 0) || (timer == 10)){
      if(frequency < targetFrequency){
        frequency += 1;
      }
      else if(frequency > targetFrequency){
        frequency -= 1;  
      }
    }
  }
  else{
    frequency = targetFrequency + (vibratoTable[vibratoPhase] * (2 - digitalRead(BUTTON_THUMB)));
    Serial.println(vibratoDelay);
  }

  modKnobIn = analogRead(A7);
  modulation = (modKnobIn/1023);
  waveform1.amplitude(modulation);
  freqKnobIn = analogRead(A6);
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
  sine_fm1.amplitude(volume / 2);
  //Serial.println("MOD VOL: " + String(modulation)+ "/" + String(modKnobIn) + " MULT: " + String(mult) + "//" + String(freqKnobIn) + " BREATH: " + String(volume));
}
