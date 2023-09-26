#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>

#define z_pulse 0
#define z_direction 1
#define z_enable 2

#define x_pulse 3
#define x_direction 4
#define x_enable 5

#define z_max 6
#define z_min 7
#define x_max 8
#define x_min 9

#define z_max_led 14
#define z_min_led 15
#define x_max_led 16
#define x_min_led 17

#define sick_output 18
#define sick_toggle 12

#define powled 13

#define debounce_interval 25 // In milliseconds

AccelStepper stepperX(1, x_pulse, x_direction);
AccelStepper stepperZ(1, z_pulse, z_direction);

Bounce2::Button z_max_bounce = Bounce2::Button();
Bounce2::Button z_min_bounce = Bounce2::Button();
Bounce2::Button x_max_bounce = Bounce2::Button();
Bounce2::Button x_min_bounce = Bounce2::Button();
Bounce2::Button sick_bounce = Bounce2::Button();

String command;
String axisCmd;
String directionCmd;
int distanceCmd;

int powledPWM = 128;
int powledDir = 0;

void home(String axis)
{
    int distance = -1000000000;
    AccelStepper motor;

    if (axis.equals("z"))
    {
        motor = stepperZ;
        distance = distance * -1;
    }
    else if (axis.equals("x"))
    {
        motor = stepperX;
    }

    delay(500);

    Serial.print("Moving home");
    Serial.print("\n");
    motor.move(0);
    motor.stop();
    motor.setCurrentPosition(0);
    motor.runToNewPosition(0);
    while (true)
    {
        z_max_bounce.update();
        x_min_bounce.update();
        sick_bounce.update();

        if ((axis.equals('z') && (z_max_bounce.rose() || z_max_bounce.isPressed() || sick_bounce.rose() || sick_bounce.isPressed())) || (axis.equals('x') && (x_min_bounce.rose()  || x_min_bounce.isPressed())))
        {
            break;
        }
        else
        {
            motor.move(distance);
            motor.run();
        }
    }

    if (axis.equals("z"))
    {
        stepperZ.stop();
        stepperZ.setCurrentPosition(0);
    }
    else if (axis.equals("x"))
    {
        stepperX.stop();
        stepperX.setCurrentPosition(0);
    }

    // Serial.print("\n");
    Serial.print("Homing complete!\r");
}



void motion(String axis, String direction, int distance)
{
    Serial.print("Axis: ");
    Serial.print(axis);
    Serial.print("\t Direction: ");
    Serial.print(direction);
    Serial.print("\t Distance: ");
    Serial.print(distance);
    Serial.print("\n");
    // int pin;
    bool moving = true;

    if (axis.equals("z"))
    {

        if (direction.equals('m'))
        {
            distance = -distance;
        }

        stepperZ.move(distance);
        while (moving)
        {
            z_max_bounce.update();
            z_min_bounce.update();
            sick_bounce.update();
            if((direction.equals('m') && (z_min_bounce.isPressed() || z_min_bounce.rose()) ) || (direction.equals('p') && (z_max_bounce.rose() || z_max_bounce.isPressed() || sick_bounce.rose() || sick_bounce.isPressed()) ) || stepperZ.distanceToGo() == 0)
            {
                // stepperZ.stop();
                moving = false;
                break;
            }
            else
            {
                stepperZ.run();
            }
        }
        stepperZ.setCurrentPosition(stepperZ.currentPosition());
    }
    else if (axis.equals("x"))
    {

        if (direction.equals('m'))
        {
            distance = -distance;
        }

        stepperX.move(distance);
        while (moving)
        {
            x_max_bounce.update();
            x_min_bounce.update();
            if ((direction.equals('m') && (x_min_bounce.isPressed() || x_min_bounce.rose()) ) || (direction.equals('p') && (x_max_bounce.rose() || x_max_bounce.isPressed())) || stepperX.distanceToGo() == 0)
            {
                // stepperX.stop();
                moving = false;
                break;
            }
            else
            {
                stepperX.run();
            }
        }

        stepperX.setCurrentPosition(stepperX.currentPosition());
    }

    delay(100);

    Serial.print("Movement complete.\r");
}

void get_stepperx_pos()
{
    Serial.println(stepperX.currentPosition());
}

void get_stepperz_pos()
{
    Serial.println(stepperZ.currentPosition());
}

void z_max_led_int()
{
    digitalWrite(z_max_led, digitalRead(z_max));
}
void z_min_led_int()
{
    digitalWrite(z_min_led, digitalRead(z_min));
}
void x_max_led_int()
{
    digitalWrite(x_max_led, digitalRead(x_max));
}
void x_min_led_int()
{
    digitalWrite(x_min_led, digitalRead(x_min));
}

void zMove(String direction, int distance){
    bool moving = true;
    if (direction.equals('m'))
    {
        distance = -distance;
    }

    stepperZ.move(distance);
    while (moving)
    {
        z_max_bounce.update();
        z_min_bounce.update();
        sick_bounce.update();
        if((direction.equals('m') && (z_min_bounce.isPressed() || z_min_bounce.rose()) ) || (direction.equals('p') && (z_max_bounce.rose() || z_max_bounce.isPressed() || sick_bounce.rose() || sick_bounce.isPressed()) ) || stepperZ.distanceToGo() == 0)
        {
            // stepperZ.stop();
            moving = false;
            break;
        }
        else
        {
            stepperZ.run();
        }
    }
    stepperZ.setCurrentPosition(stepperZ.currentPosition());
    delay(100);
}

void z_focus(){
    // Move Z to bottom
    Serial.println("Move Z axis to lower");
    zMove('m', 20000);
    delay(500);
    Serial.println("Move Z axis to top");
    zMove('p',10000000);
    delay(500);
    Serial.println("Move Z axis down 2000 steps");
    zMove('m',2000);
    stepperZ.setMaxSpeed(1000);
    stepperZ.setAcceleration(100);
    delay(500);
    Serial.println("Move Z axis to top");
    zMove('p',10000000);
    delay(500);
    Serial.println("Move Z axis down 2000 steps");
    zMove('m',2000);
    stepperZ.setMaxSpeed(100);
    stepperZ.setAcceleration(100);
    delay(500);
    Serial.println("Move Z axis to top");
    zMove('p',10000000);
    delay(1000);
    // Serial.print("Focus completed.\r");
    Serial.println("Focus completed!");
    delay(100);
    Serial.println("KEKKONEN");
}

void laser_toggle(){
    digitalWrite(sick_toggle, !digitalRead(sick_toggle));
}

void setup()
{
    Serial.begin(9600);

    z_max_bounce.attach(z_max, INPUT_PULLUP);
    z_min_bounce.attach(z_min, INPUT_PULLUP);
    x_max_bounce.attach(x_max, INPUT_PULLUP);
    x_min_bounce.attach(x_min, INPUT_PULLUP);
    sick_bounce.attach(sick_output, INPUT_PULLUP);

    z_max_bounce.interval(debounce_interval);
    z_min_bounce.interval(debounce_interval);
    x_max_bounce.interval(debounce_interval);
    x_min_bounce.interval(debounce_interval);
    sick_bounce.interval(debounce_interval);

    attachInterrupt(digitalPinToInterrupt(z_max), z_max_led_int, CHANGE);
    attachInterrupt(digitalPinToInterrupt(z_min), z_min_led_int, CHANGE);
    attachInterrupt(digitalPinToInterrupt(x_max), x_max_led_int, CHANGE);
    attachInterrupt(digitalPinToInterrupt(x_min), x_min_led_int, CHANGE);

    pinMode(z_max_led, OUTPUT);
    pinMode(z_min_led, OUTPUT);
    pinMode(x_max_led, OUTPUT);
    pinMode(x_min_led, OUTPUT);

    pinMode(sick_toggle, OUTPUT);

    digitalWrite(sick_toggle, HIGH);

    pinMode(powled, OUTPUT);

    stepperX.setMaxSpeed(4000);
    stepperX.setAcceleration(400);

    stepperZ.setMaxSpeed(4000);
    stepperZ.setAcceleration(400);

    delay(2000);
    stepperX.stop();
    stepperZ.stop();
}

void loop()
{
    // analogWrite(powled,powledPWM);
    digitalWrite(powled, HIGH);
    
    if(Serial.available())
    {
        command = Serial.readStringUntil('\n');
        command.trim();
        // Serial.println(Serial.read());
        if (command.equals("posx"))
        {
            get_stepperx_pos();
        }
        else if (command.equals("posz"))
        {
            get_stepperz_pos();
        }
        else if (command.equals("zfocus"))
        {
            z_focus();
            delay(100);
            stepperZ.setMaxSpeed(4000);
            stepperZ.setAcceleration(400);
        }
        else if (command.equals("lasertoggle")){
            laser_toggle();
            delay(100);
        }
        else
        {
            // ? Command structure: apXXX
            axisCmd = command.substring(0, 1);
            directionCmd = command.substring(1, 2);
            distanceCmd = command.substring(2, command.length()).toInt();

            if (directionCmd.equals("h"))
            {
                home(axisCmd);
            }
            else if (directionCmd.equals("p") || directionCmd.equals("m"))
            {
                if (axisCmd.equals("x") || axisCmd.equals("z")){
                    motion(axisCmd, directionCmd, distanceCmd);
                }
            }
            else
            {
                Serial.print(command);
                Serial.print("Unknown command!\r");
            }
        }
    }

    digitalWrite(z_max_led, digitalRead(z_max));
    digitalWrite(z_min_led, digitalRead(z_min));
    digitalWrite(x_max_led, digitalRead(x_max));
    digitalWrite(x_min_led, digitalRead(x_min));

    delay(10);
}
