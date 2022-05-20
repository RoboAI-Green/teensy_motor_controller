#include <Arduino.h>
#include <AccelStepper.h>

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

AccelStepper stepperX(1,x_pulse,x_direction);
AccelStepper stepperZ(1,z_pulse,z_direction);

String command;
String axisCmd;
String directionCmd;
int distanceCmd;

int powledPWM = 128;
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

    Serial.print("Moving home");

    while (digitalRead(limitPin)==LOW && limitPin>0 && directionPin>0 && pulsePin>-1 )
    {
        delay(10);
    }

    Serial.print("\n");
    Serial.println("\nHoming complete!");
}

void motion(String axis, String direction, int distance){
    Serial.print("Axis: ");
    Serial.print(axis);
    Serial.print("\tDirection: ");
    Serial.print(direction);
    Serial.print("\tDistance: ");
    Serial.println(distance);

    if (axis.equals("z")){
        if(direction.equals('p')){
            stepperZ.move(distance);
            while(digitalRead(z_max)==LOW && stepperZ.distanceToGo()!=0){
                
            }
        }
        else{

        }

    }else if(axis.equals("x")){

    }else{
        Serial.print("AXIS UNKNOWN");
    }

}

void setup()
{
    Serial.begin(9600);
    pinMode(z_max, INPUT_PULLUP);
    pinMode(z_min, INPUT_PULLUP);
    pinMode(x_max, INPUT_PULLUP);
    pinMode(x_min, INPUT_PULLUP);

    pinMode(z_max_led, OUTPUT);
    pinMode(z_min_led, OUTPUT);
    pinMode(x_max_led, OUTPUT);
    pinMode(x_min_led, OUTPUT);

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
        
        // ? Command structure: apXXX
        axisCmd = command.substring(0,1);
        directionCmd = command.substring(1,2);
        distanceCmd = command.substring(2,command.length()).toInt();

        if(directionCmd.equals("h")){
            home(axisCmd);
        }
        else if(directionCmd.equals("p") || directionCmd.equals("m")){
            motion(axisCmd, directionCmd, distanceCmd);
        }
        else {
            Serial.print("Unknown command!\r");
        }
    }

    digitalWrite(z_max_led, digitalRead(z_max));
    digitalWrite(z_min_led, digitalRead(z_min));
    digitalWrite(x_max_led, digitalRead(x_max));
    digitalWrite(x_min_led, digitalRead(x_min));

    delay(10);
}
