#include <Arduino.h>

String command;
String axisCmd;
String directionCmd;
String distanceCmd;

const byte z_pulse      = 0;
const byte z_direction  = 1;
const byte z_enable     = 2;

const byte x_pulse      = 3;
const byte x_direction  = 4;
const byte x_enable     = 5;

const byte z_max        = 6;
const byte z_min        = 7;
const byte x_max        = 8;
const byte x_min        = 9;

const byte z_max_led    = 17;
const byte z_min_led    = 18;
const byte x_max_led    = 19;
const byte x_min_led    = 20;

// void z_max_change();
// void z_min_change();
// void x_max_change();
// void x_min_change();

volatile byte z_max_state = LOW;
volatile byte z_min_state = LOW;
volatile byte x_max_state = LOW;
volatile byte x_min_state = LOW;

// int i = 0;

void setup()
{
    Serial.begin(9600);
//     pinMode(z_max, INPUT_PULLUP);
//     pinMode(z_min, INPUT_PULLUP);
//     pinMode(x_max, INPUT_PULLUP);
//     pinMode(x_min, INPUT_PULLUP);

    pinMode(z_max_led, OUTPUT);
    pinMode(z_min_led, OUTPUT);
    pinMode(x_max_led, OUTPUT);
    pinMode(x_min_led, OUTPUT);

    delay(2000);
    Serial.println("Type PIN");

//     attachInterrupt(digitalPinToInterrupt(z_max), z_max_change, CHANGE);
//     attachInterrupt(digitalPinToInterrupt(z_min), z_min_change, CHANGE);
//     attachInterrupt(digitalPinToInterrupt(x_max), x_max_change, CHANGE);
//     attachInterrupt(digitalPinToInterrupt(x_min), x_min_change, CHANGE);
}

void loop()
{
    if(Serial.available()){
        command = Serial.readStringUntil('\n');
        command.trim();
        Serial.println(command);
        
        axisCmd = command.substring(0,1);
        directionCmd = command.substring(1,2);
        distanceCmd = command.substring(2,command.length());
        Serial.print("Axis: ");
        Serial.print(axisCmd);
        Serial.print("\tDirection: ");
        Serial.print(directionCmd);
        Serial.print("\tDistance: ");
        Serial.println(distanceCmd);
        Serial.println("----");
        delay(100);
    }
}

// void z_max_change() { z_max_state = !z_max_state; digitalWrite(z_max_led, z_max_state);}
// void z_min_change() { z_min_state = !z_min_state; digitalWrite(z_min_led, z_min_state);}
// void x_max_change() { x_max_state = !x_max_state; digitalWrite(x_max_led, x_max_state);}
// void x_min_change() { x_min_state = !x_min_state; digitalWrite(x_min_led, x_min_state);}
