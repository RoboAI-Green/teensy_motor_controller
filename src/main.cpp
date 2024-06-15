#include <Arduino.h>
#include <Bounce2.h>
#include <AccelStepper.h>
#include "Command.h"
#include "optoNCDT.h"

// Pins 0 and 1 reserved for Serial1, optoNCDT communication

// Setting pins
#define x_pulse 2
#define x_direction 3
#define x_min 4
#define x_max 5

#define y_pulse 6
#define y_direction 7
#define y_min 8
#define y_max 9

#define z_pulse 10
#define z_direction 11
#define z_min 12
#define z_max 13

AccelStepper stepperX(1, x_pulse, x_direction);
AccelStepper stepperY(1, y_pulse, y_direction);
AccelStepper stepperZ(1, z_pulse, z_direction);

Bounce2::Button z_max_bounce = Bounce2::Button();
Bounce2::Button z_min_bounce = Bounce2::Button();
Bounce2::Button x_max_bounce = Bounce2::Button();
Bounce2::Button x_min_bounce = Bounce2::Button();
Bounce2::Button y_max_bounce = Bounce2::Button();
Bounce2::Button y_min_bounce = Bounce2::Button();

// Laser pulse BNC connection
#define laser_bnc 14
unsigned long pulseCount = 0;

// Function hashes for switch structure.
#define hash_posx 2090623343
#define hash_posz 2090623345
#define hash_posy 2090623344
#define hash_lasertoggle -1635822754
#define hash_pulsecount 1781759127
#define hash_resetpulse -1903504655
#define hash_speed 274671510
#define hash_getspeed -2066154
#define hash_grid 2090302827
#define hash_grid_blanks -293840475
#define hash_grid_initial_move 1967809066
#define hash_grid_shots -1810979237
#define hash_grid_move_x 140951096
#define hash_grid_move_y 140951097
#define hash_grid_columns 315371883
#define hash_grid_rows -2007164075
#define hash_move 2090515612
#define hash_optom 269944500
#define hash_optos 269944506
#define hash_optoc 269944490
#define hash_help 2090324718
#define hash_createCmd 2096566157
#define hash_lim 193498183

CmdParse parser = CmdParse();
optoNCDT epsilon = optoNCDT();

bool optoLaser = true;

unsigned long grid_blanks = 0;
int grid_ini_move = 0;
int grid_ppp = 0;
int grid_mx = 0;
int grid_my = 0;
int grid_cols = 0;
int grid_rows = 0;

void usagePrint(int cmdHash)
{
    switch (cmdHash)
    {
    case hash_posx:
        Serial.println("Get and print the step count for axis X");
        break;
    case hash_posy:
        Serial.println("Get and print the step count for axis Y");
        break;
    case hash_posz:
        Serial.println("Get and print the step count for axis Z");
        break;
    case hash_lasertoggle:
        Serial.println("Toggle laser");
        break;
    case hash_pulsecount:
        Serial.println("Pulse count");
        break;
    case hash_resetpulse:
        Serial.println("Reset pulses");
        break;
    case hash_speed:
        Serial.println("Set speed");
        Serial.println("Usage:\n\tspeed a xxxx yyyy");
        Serial.println("Where:\n\ta = axis (x,y,z)\n\txxxx = Acceleration\n\tyyyy = Max speed");
        break;
    case hash_getspeed:
        Serial.println("Get speed information");
        break;
    case hash_grid_blanks:
        Serial.println("How many blanks before moving the sample");
        Serial.println("Usage:\n\tgrid_blanks n");
        Serial.println("Where:\n\tn = Blank count");
        break;
    case hash_grid_initial_move:
        Serial.println("How much to move after blanks");
        Serial.println("Usage:\n\tgrid_initial_move nnnn");
        Serial.println("Where:\n\tnnnn = Step count the X axis moves, after set blank count received.");
        break;
    case hash_grid_shots:
        Serial.println("How many shots on a single point");
        Serial.println("Usage:\n\tgrid_shots nnnn");
        Serial.println("Where:\n\tnnnn = Step count the X axis moves, after set blank count received.");
        break;
    case hash_grid_move_x:
        Serial.println("How many steps to move on X axis after shots");
        break;
    case hash_grid_move_y:
        Serial.println("How many steps to move when changing row");
        break;
    case hash_grid_columns:
        Serial.println("How many columns (x axis)");
        break;
    case hash_grid_rows:
        Serial.println("How many rows (y axis)");
        break;
    case hash_move:
        Serial.println("Move an axis");
        Serial.println("Usage:\n\tmove a nnnn");
        Serial.println("Where:\n\ta = axis (x,y,z)\n\tnnnn = stepper count");
        break;
    case hash_optom:
        Serial.println("Start outputing OptoNCDT data.");
        break;
    case hash_optos:
        Serial.println("This would stop outputing OptoNCDT data, if it was displaying.");
        break;
    default:
        break;
    }
}

void func_help()
{
    usagePrint(hash_posx);
    usagePrint(hash_posz);
    usagePrint(hash_posy);
    usagePrint(hash_lasertoggle);
    usagePrint(hash_pulsecount);
    usagePrint(hash_resetpulse);
    usagePrint(hash_speed);
    usagePrint(hash_getspeed);
    usagePrint(hash_grid_blanks);
    usagePrint(hash_grid_initial_move);
    usagePrint(hash_grid_shots);
    usagePrint(hash_grid_move_x);
    usagePrint(hash_grid_move_y);
    usagePrint(hash_grid_columns);
    usagePrint(hash_grid_rows);
    usagePrint(hash_move);
    usagePrint(hash_optom);
    usagePrint(hash_optos);
    usagePrint(hash_optoc);
}

void func_createCmd(String cmdString)
{
    Serial.println(parser.createCmd(cmdString));
}

void laser_bnc_interrupt()
{
    pulseCount++;
}

// void func_move(char axis, int distance)
void func_move(AccelStepper &stepper, Bounce2::Button &min_limit, Bounce2::Button &max_limit, int distance)
{
    bool moving = true;
    stepper.move(distance);

    while (moving)
    {
        min_limit.update();
        max_limit.update();

        if (
            (distance < 0 && (min_limit.isPressed() || min_limit.rose())) ||
            (distance > 0 && (max_limit.isPressed() || max_limit.rose())) ||
            stepper.distanceToGo() == 0)
        {
            moving = false;
            stepper.stop();
            Serial.println("MEND");
        }
        else
        {
            stepper.run();
        }
    }

    stepper.stop();
    stepper.setCurrentPosition(stepper.currentPosition());
}

void func_speed(char axis, float speed, float acceleration)
{
    switch (axis)
    {
    case 'x':
        stepperX.setMaxSpeed(speed);
        stepperX.setAcceleration(acceleration);
        Serial.println("SPEEDSET");
        break;
    case 'y':
        stepperY.setMaxSpeed(speed);
        stepperY.setAcceleration(acceleration);
        Serial.println("SPEEDSET");
        break;
    case 'z':
        stepperZ.setMaxSpeed(speed);
        stepperZ.setAcceleration(acceleration);
        Serial.println("SPEEDSET");
        break;
    default:
        usagePrint(hash_speed);
        break;
    }
}

void func_getspeed(char axis)
{
    switch (axis)
    {
    case 'x':
        Serial.print("X axis\tMax Speed ");
        Serial.print(stepperX.maxSpeed());
        Serial.print("\tAcceleration ");
        Serial.println(stepperX.acceleration());
        break;
    case 'y':
        Serial.print("Y axis\tMax Speed ");
        Serial.print(stepperY.maxSpeed());
        Serial.print("\tAcceleration ");
        Serial.println(stepperY.acceleration());
        break;
    case 'z':
        Serial.print("Z axis\tMax Speed ");
        Serial.print(stepperZ.maxSpeed());
        Serial.print("\tAcceleration ");
        Serial.println(stepperZ.acceleration());
        break;
    default:
        usagePrint(hash_speed);
        break;
    }
}

void func_posx() { Serial.println(stepperX.currentPosition()); }
void func_posy() { Serial.println(stepperY.currentPosition()); }
void func_posz() { Serial.println(stepperZ.currentPosition()); }

void func_pulsecount()
{
    Serial.print("Pulse count: ");
    Serial.println(pulseCount);
}

void func_lasertoggle()
{

    if (optoLaser)
    {
        epsilon.optoCmd("LASERPOW OFF");
        optoLaser = false;
    }
    else
    {
        epsilon.optoCmd("LASERPOW FULL");
        optoLaser = true;
    }
}

void func_gridMove()
{
    bool warm = false;
    int curMove, curCount, curRow, curCol;
    curMove = curCount = curRow = curCol = 0;
    int linCount = grid_cols * grid_rows;

    // Run the while loop, when the current measurement count is less than the calculated measurement ammount.
    while (curCount < linCount)
    {
        Serial.print("Pulse count: ");
        Serial.print(pulseCount);
        Serial.print("\tCur count: ");
        Serial.print(curCount);
        Serial.print("\tLin count: ");
        Serial.println(linCount);
        // Check if warmup shots have been fired.
        // - Pulse count is continuously increasing value, provided by every single pulse fired
        //   via the BNC connection from the Quantel Falcon controlbox
        if (warm)
        {
            // From the current pulse count, remove the calculated 3rd pulse for the current position
            curMove = pulseCount - (grid_blanks + curCount * grid_ppp);
            if (curMove == grid_ppp)
            {
                if (curCol < (grid_cols - 1))
                {
                    func_move(stepperX, x_min_bounce, x_max_bounce, grid_mx);
                    curCol++;
                }
                else
                {
                    func_move(stepperY, y_min_bounce, y_max_bounce, grid_my);
                    grid_mx *= -1;
                    curCol = 0;
                }
                curCount++;
            }
        }
        else if (pulseCount == grid_blanks)
        {
            warm = true;
            func_move(stepperX, x_min_bounce, x_max_bounce, grid_ini_move);
        }
    }
}

void func_lim()
{
    x_min_bounce.update();
    x_max_bounce.update();
    y_min_bounce.update();
    y_max_bounce.update();
    z_min_bounce.update();
    z_max_bounce.update();

    Serial.print("X MIN STATE: ");
    Serial.println(x_min_bounce.isPressed() ? "Yes" : "No");
    Serial.print("X MAX STATE: ");
    Serial.println(x_max_bounce.isPressed() ? "Yes" : "No");
    Serial.print("Y MIN STATE: ");
    Serial.println(y_min_bounce.isPressed() ? "Yes" : "No");
    Serial.print("Y MAX STATE: ");
    Serial.println(y_max_bounce.isPressed() ? "Yes" : "No");
    Serial.print("Z MIN STATE: ");
    Serial.println(z_min_bounce.isPressed() ? "Yes" : "No");
    Serial.print("Z MAX STATE: ");
    Serial.println(z_max_bounce.isPressed() ? "Yes" : "No");

    Serial.print(digitalRead(x_min));
    Serial.println(digitalRead(x_max));
    Serial.print(digitalRead(y_min));
    Serial.println(digitalRead(y_max));
    Serial.print(digitalRead(z_min));
    Serial.println(digitalRead(z_max));
}

void setup()
{
    Serial.begin(9600);
    epsilon.begin(921600, SERIAL_8N1);
    epsilon.setMeasuringRange(100);

    // Attach buttons
    z_max_bounce.attach(z_max, INPUT_PULLDOWN);
    z_min_bounce.attach(z_min, INPUT_PULLDOWN);
    x_max_bounce.attach(x_max, INPUT_PULLDOWN);
    x_min_bounce.attach(x_min, INPUT_PULLDOWN);
    y_max_bounce.attach(y_max, INPUT_PULLDOWN);
    y_min_bounce.attach(y_min, INPUT_PULLDOWN);

    // Set stepper motor max speed and acceleration
    stepperX.setMaxSpeed(100000);
    stepperX.setAcceleration(5000000);
    stepperY.setMaxSpeed(125000);
    stepperY.setAcceleration(5000000);
    stepperZ.setMaxSpeed(4000);
    stepperZ.setAcceleration(400);

    stepperX.stop();
    stepperZ.stop();
    stepperY.stop();

    // Attach laser pulse BNC as interrupt
    // pinMode(laser_bnc, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(laser_bnc), laser_bnc_interrupt, RISING);
}

void loop()
{
    x_min_bounce.update();
    x_max_bounce.update();
    y_min_bounce.update();
    y_max_bounce.update();
    z_min_bounce.update();
    z_max_bounce.update();

    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        Command cmd = parser.parseCommand(input);

        switch (cmd.nameHash)
        {
        case hash_posx:
            func_posx();
            break;
        case hash_posy:
            func_posy();
            break;
        case hash_posz:
            func_posz();
            break;
        case hash_lasertoggle:
            usagePrint(hash_lasertoggle);
            break;
        case hash_pulsecount:
            func_pulsecount();
            break;
        case hash_resetpulse:
            pulseCount = 0;
            // usagePrint(hash_resetpulse);
            break;
        case hash_speed:
            if (cmd.paramCount == 3)
            {
                char axis = cmd.paramArray[0].charAt(0);
                float speed = cmd.paramArray[1].toFloat();
                float acceleration = cmd.paramArray[2].toFloat();
                func_speed(axis, speed, acceleration);
            }
            else
            {
                usagePrint(hash_speed);
            }
            break;
        case hash_getspeed:
            if (cmd.paramCount == 1)
            {
                char axis = cmd.paramArray[0].charAt(0);
                func_getspeed(axis);
            }
            else
            {
                usagePrint(hash_getspeed);
            }
            break;
        case hash_grid:
            pulseCount = 0;
            func_gridMove();
            break;
        case hash_grid_blanks:
            if (cmd.paramCount == 1)
            {
                grid_blanks = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_blanks);
            }
            break;
        case hash_grid_initial_move:
            if (cmd.paramCount == 1)
            {
                grid_ini_move = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_initial_move);
            }
            break;
        case hash_grid_shots:
            if (cmd.paramCount == 1)
            {
                grid_ppp = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_shots);
            }
            break;
        case hash_grid_move_x:
            if (cmd.paramCount == 1)
            {
                grid_mx = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_move_x);
            }
            break;
        case hash_grid_move_y:
            if (cmd.paramCount == 1)
            {
                grid_my = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_move_y);
            }
            break;
        case hash_grid_columns:
            if (cmd.paramCount == 1)
            {
                grid_cols = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_columns);
            }
            break;
        case hash_grid_rows:
            if (cmd.paramCount == 1)
            {
                grid_rows = cmd.paramArray[0].toInt();
            }
            else
            {
                usagePrint(hash_grid_rows);
            }
            break;
        case hash_move:
            if (cmd.paramCount == 2)
            {
                char axis = cmd.paramArray[0].charAt(0);
                int distance = cmd.paramArray[1].toInt();
                switch (axis)
                {
                case 'x':
                    func_move(stepperX, x_min_bounce, x_max_bounce, distance);
                    break;
                case 'y':
                    func_move(stepperY, y_min_bounce, y_max_bounce, distance);
                    break;
                case 'z':
                    func_move(stepperZ, z_min_bounce, z_max_bounce, distance);
                    break;
                default:
                    break;
                }
                // func_move(axis, distance);
            }
            else
            {
                usagePrint(hash_move);
            }

            break;
        case hash_optom:
            while (true)
            {
                float epsiMeas = epsilon.optoMeas();
                Serial.println(epsiMeas);
                if (Serial.available())
                {
                    String input = Serial.readStringUntil('\n');
                    cmd = parser.parseCommand(input);

                    if (cmd.nameHash == hash_optos)
                    {
                        Serial.println(9876543210);
                        break;
                    }
                }
            }
            break;
        case hash_optoc:
            Serial.println(epsilon.optoCmd(cmd.parameters));
            break;
        case hash_lim:
            func_lim();
            break;
        case hash_help:
            func_help();
            break;
        case hash_createCmd:
            func_createCmd(cmd.paramArray[0]);
            break;
        default:
            func_createCmd(cmd.name);
            Serial.println("Unknown command. Use help to see list of commands.");
            break;
        }
    }
}
