#include <Arduino.h>

#define z_pulse      0
#define z_direction  1
#define z_enable     2

#define x_pulse      3
#define x_direction  4
#define x_enable     5

#define z_max        6
#define z_min        7
#define x_max        8
#define x_min        9

#define z_max_led    17
#define z_min_led    18
#define x_max_led    19
#define x_min_led    20

#define powled       13

String command;
String axisCmd;
String directionCmd;
String distanceCmd;

int powledPWM = 0;
int powledDir = 0;

void home(String axis)
{
    Serial.print("Homing axis ");
    Serial.println(axis);
    int limitPin = -1;
    int directionPin = -1;
    int pulsePin = -1;

    if (axis.equals("z"))
    {
        limitPin = z_min;
        directionPin = z_direction;
        pulsePin = z_pulse;
    }
    else if(axis.equals("x"))
    {
        limitPin = x_min;
        directionPin = x_direction;
        pulsePin = x_pulse;
    }    

    Serial.println(digitalRead(limitPin));
    Serial.print("Moving home");
    while (digitalRead(limitPin)==LOW && limitPin>0 && directionPin>0 && pulsePin>0 )
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nHoming complete!");
}

void setup()
{
    Serial.begin(9600);
    pinMode(z_max, INPUT_PULLUP);
    pinMode(z_min, INPUT_PULLUP);
    pinMode(x_max, INPUT_PULLUP);
    pinMode(x_min, INPUT_PULLUP);

    // pinMode(z_max_led, OUTPUT);
    // pinMode(z_min_led, OUTPUT);
    // pinMode(x_max_led, OUTPUT);
    // pinMode(x_min_led, OUTPUT);

    pinMode(powled,OUTPUT);

    delay(2000);
}

void loop()
{

    analogWrite(powled,powledPWM);

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

        if(directionCmd.equals("h")){
            home(axisCmd);
        }
    }

    digitalWrite(z_max_led, digitalRead(z_max));
    digitalWrite(z_min_led, digitalRead(z_min));
    digitalWrite(x_max_led, digitalRead(x_max));
    digitalWrite(x_min_led, digitalRead(x_min));

    if (powledDir>0&&powledPWM<254){
        powledPWM += 5;
    }else{
        powledDir=-1;
    }

    if (powledDir<1 && powledPWM>0){
        powledPWM -= 5;
    }else{
        powledDir=2;
    }

    delay(100);
}
