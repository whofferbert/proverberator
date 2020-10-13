#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=213.01031494140625,374
AudioEffectReverb        reverb1;        //xy=454.01031494140625,214
AudioEffectReverb        reverb2;        //xy=455.01031494140625,258
AudioEffectReverb        reverb3;        //xy=467.01031494140625,432
AudioEffectReverb        reverb4;        //xy=472.01031494140625,479
AudioEffectFreeverb      freeverb1;      //xy=566.0103149414062,332
AudioEffectFreeverb      freeverb2;      //xy=571.0103149414062,377
AudioEffectFreeverb      freeverb4;      //xy=615.0103149414062,487
AudioEffectFreeverb      freeverb3;      //xy=621.0103149414062,439
AudioMixer4              mixer1;         //xy=851.0103988647461,233.00000381469727
AudioMixer4              mixer2;         //xy=886.0103149414062,448
AudioOutputI2S           i2s2;           //xy=1092.0103149414062,389
AudioConnection          patchCord1(i2s1, 0, reverb1, 0);
AudioConnection          patchCord2(i2s1, 0, freeverb1, 0);
AudioConnection          patchCord3(i2s1, 0, mixer1, 0);
AudioConnection          patchCord4(i2s1, 0, reverb3, 0);
AudioConnection          patchCord5(i2s1, 1, reverb2, 0);
AudioConnection          patchCord6(i2s1, 1, freeverb2, 0);
AudioConnection          patchCord7(i2s1, 1, mixer2, 0);
AudioConnection          patchCord8(i2s1, 1, reverb4, 0);
AudioConnection          patchCord9(reverb1, 0, mixer1, 1);
AudioConnection          patchCord10(reverb2, 0, mixer2, 1);
AudioConnection          patchCord11(reverb3, freeverb3);
AudioConnection          patchCord12(reverb4, freeverb4);
AudioConnection          patchCord13(freeverb1, 0, mixer1, 2);
AudioConnection          patchCord14(freeverb2, 0, mixer2, 2);
AudioConnection          patchCord15(freeverb4, 0, mixer2, 3);
AudioConnection          patchCord16(freeverb3, 0, mixer1, 3);
AudioConnection          patchCord17(mixer1, 0, i2s2, 0);
AudioConnection          patchCord18(mixer2, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=612.0103149414062,552
// GUItool: end automatically generated code

#include <ResponsiveAnalogRead.h>
ResponsiveAnalogRead timePot(A8, true);
ResponsiveAnalogRead roomSizePot(A3, true);
ResponsiveAnalogRead dampingPot(A2, true);
ResponsiveAnalogRead blendPot(A0, true);
ResponsiveAnalogRead selectSwitch(A11, true);

#include <Bounce2.h>
Bounce footSwitch = Bounce();
int footSwitchPin = 26;


// LED Pin Associations
int switchLedPin = 24;
int timeLedPin = 9;
int roomSizeLedPin = 5;
int dampingLedPin = 4;
int blendLedPin = 3;

int rgbledRedPin = 0;
int rgbledGreenPin = 1;
int rgbledBluePin = 2;

struct pedalState
{
    bool ledsOn = false;
    // can be 0, 1 or 2
    int channel = 0;
    // blend is the amount of effect to mix in vs original signal
    float blend = .5;
    bool needsChannelUpdate = false;
};

pedalState brain;

void setup() {
    // set up audio things
    // Serial.begin(9600);
    AudioMemory(40);
    //delay(2400);
    sgtl5000_1.enable();
    sgtl5000_1.volume(.8);
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    // anything else with the sgtl?

    pinMode(footSwitchPin, INPUT_PULLUP);
    pinMode(switchLedPin, OUTPUT);

    pinMode(timeLedPin, OUTPUT);
    pinMode(roomSizeLedPin, OUTPUT);
    pinMode(dampingLedPin, OUTPUT);
    pinMode(blendLedPin, OUTPUT);

    footSwitch.attach(footSwitchPin);

    initialReadSet();
}

void initialReadSet () {
    // gotta set up the initial values i think

    // checking the switch state and set the thing properly
    if (digitalRead(footSwitchPin) == LOW) {
        //digitalWrite(switchLedPin, HIGH);
        brain.ledsOn = true;
    }
    if (digitalRead(footSwitchPin) == HIGH) {
        //digitalWrite(switchLedPin, LOW);
        brain.ledsOn = false;
    }

    / set switch state on boot
    selectSwitchStateManager();

    // set leds to off just in case
    digitalWrite(timeLedPin, HIGH);
    digitalWrite(roomSizeLedPin, HIGH);
    digitalWrite(dampingLedPin, HIGH);
    digitalWrite(blendLedPin, HIGH);

}

void runUpdates() {
    footSwitch.update();
    timePot.update();
    roomSizePot.update();
    dampingPot.update();
    blendPot.update();
    selectSwitch.update();
}

void selectSwitchStateManager() {
    int val = selectSwitch.getValue();
    if (val < 30) {
        // switch is up
        brain.channel = 2;
    } else if (val < 600) {
        // low
        brain.channel = 0;
    } else {
        // mid
        brain.channel = 1;
    }
    // this line is especially some side-effecting garbage that makes
    // sure the channels get set properly on boot the first time too
    brain.needsChannelUpdate = true;
}

void footSwitchStateManager () {
    // manage the things here
    // this controls the mixer levels
}

void rgbLedPWM(int R, int G,int B) {
    // PWM it
}

void rgbLedOn(bool R, bool G, bool B) {
    digitalWrite(rgbledRedPin, not R);
    digitalWrite(rgbledGreenPin, not G);
    digitalWrite(rgbledBluePin, not B);
}

void ledOnChannelLogic (){
    if (brain.channel == 0) {
        digitalWrite(timeLedPin, LOW);
        digitalWrite(roomSizeLedPin, HIGH);
        digitalWrite(dampingLedPin, HIGH);
        rgbLedOn(true, false, false);
    } else if (brain.channel == 1) {
        digitalWrite(timeLedPin, HIGH);
        digitalWrite(roomSizeLedPin, LOW);
        digitalWrite(dampingLedPin, LOW);
        rgbLedOn(false, true, false);
    } else {
        digitalWrite(timeLedPin, LOW);
        digitalWrite(roomSizeLedPin, LOW);
        digitalWrite(dampingLedPin, LOW);
        rgbLedOn(false, false, true);
    }

}

void ledManager () {
    if (brain.ledsOn == true) {
        ledOnChannelLogic();
        digitalWrite(switchLedPin, LOW);
        digitalWrite(blendLedPin, LOW);
    } else {
        digitalWrite(switchLedPin, HIGH);
        digitalWrite(timeLedPin, HIGH);
        digitalWrite(roomSizeLedPin, HIGH);
        digitalWrite(dampingLedPin, HIGH);
        digitalWrite(blendLedPin, HIGH);
        rgbLedOn(false, false, false);
    }
}

void channelManager () {
    if (brain.ledsOn == true) {
        // mixer blends in the stuff
        float dry = 1.0 - brain.blend;
        if (brain.channel == 0) {
            mixer1.gain(0, dry);
            mixer2.gain(0, dry);
            mixer1.gain(1, brain.blend);
            mixer2.gain(1, brain.blend); 
            mixer1.gain(2, 0);
            mixer2.gain(2, 0);
            mixer1.gain(3, 0);
            mixer2.gain(3, 0);
        } else if (brain.channel == 1) {
            mixer1.gain(0, dry);
            mixer2.gain(0, dry);
            mixer1.gain(1, 0);
            mixer2.gain(1, 0);
            mixer1.gain(2, brain.blend);
            mixer2.gain(2, brain.blend);
            mixer1.gain(3, 0);
            mixer2.gain(3, 0);
        } else {
            mixer1.gain(0, dry);
            mixer2.gain(0, dry);
            mixer1.gain(2, 0);
            mixer2.gain(2, 0);
            mixer1.gain(1, 0);
            mixer2.gain(1, 0);
            mixer1.gain(3, brain.blend);
            mixer2.gain(3, brain.blend);
        }
    } else {
        // mixer mute output
        mixer1.gain(0,0);
        mixer1.gain(1,0);
        mixer1.gain(2,0);
        mixer1.gain(3,0);
        mixer2.gain(0,0);
        mixer2.gain(1,0);
        mixer2.gain(2,0);
        mixer2.gain(3,0);
    }
}


void loop () {
    // check the knobs for their positions.
    // if they change, adjust the things
    // that can always happen
    // also, check the sense switch
    // if the sense switch is off:
    //   mute sounds (mixers)
    //   leds off-ish?
    //   maybe keep the "eye" on
    // else
    //   leds allowed to be on as appropriate
    //   unmute sounds (mixers)


    // update attachments first
    runUpdates();

    //Serial.println(digitalRead(footSwitchPin));
    if (footSwitch.fell() == true){
        //brain.ledsOn = false;
        brain.ledsOn = true;
        brain.needsChannelUpdate = true;
    } else if (footSwitch.rose() == true) {
        brain.ledsOn = false;
        brain.needsChannelUpdate = true;
        //brain.ledsOn = true;
    }

    
    if (selectSwitch.hasChanged()) {
        selectSwitchStateManager();
    }

    if (timePot.hasChanged()) {
        //Serial.println(timePot.getValue());
        float val = timePot.getValue() / 1024.0;
        reverb1.reverbTime(val);
        reverb2.reverbTime(val);
        reverb3.reverbTime(val);
        reverb4.reverbTime(val);
    }

    if (roomSizePot.hasChanged()) {
        //Serial.println(roomSizePot.getValue());
        float val = roomSizePot.getValue() / 1024.0;
        freeverb1.roomsize(val);
        freeverb2.roomsize(val);
        freeverb3.roomsize(val);
        freeverb4.roomsize(val);
    }

    if (dampingPot.hasChanged()) {
        //Serial.println(dampingPot.getValue());
        float val = dampingPot.getValue() / 1024.0;
        freeverb1.damping(val);
        freeverb2.damping(val);
        freeverb3.damping(val);
        freeverb4.damping(val);
    }

    if (blendPot.hasChanged()){
        // stuff
        //Serial.println(blendPot.getValue());
        float val = blendPot.getValue() / 1024.0;
        //Serial.println(val);
        // adjust mix
        brain.blend = val;
        brain.needsChannelUpdate = true;
    }


    if (brain.needsChannelUpdate == true) {
        brain.needsChannelUpdate = false;
        channelManager();
        ledManager();
    }


    delay(10);
    
}
