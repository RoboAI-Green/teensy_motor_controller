#include <Arduino.h>
#include <Bounce2.h>
#include <AccelStepper.h>
#include <EEPROM.h>
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

AccelStepper stepperX(AccelStepper::DRIVER, x_pulse, x_direction);
AccelStepper stepperY(AccelStepper::DRIVER, y_pulse, y_direction);
AccelStepper stepperZ(AccelStepper::DRIVER, z_pulse, z_direction);

// How many pulses it takes to move 1 mm
int stepperX_ppmm = 3200;
int stepperY_ppmm = 3200;
int stepperZ_ppmm = 320;

struct StepperValues
{
    int xSpeed;
    int xAccel;
    int ySpeed;
    int yAccel;
    int zSpeed;
    int zAccel;
};

StepperValues savedValues;

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
#define hash_save 2090715988
#define hash_load 2090478981
#define hash_map 193499011

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

int eeprom_address = 69;

void usagePrint(int cmdHash)
{
    switch (cmdHash)
    {
    case hash_posx:
        Serial.print("posx\n");
        Serial.print("\tGet and print the step count for axis X\n\n");
        break;
    case hash_posy:
        Serial.print("posy\n");
        Serial.print("\tGet and print the step count for axis Y\n\n");
        break;
    case hash_posz:
        Serial.print("posz\n");
        Serial.print("\tGet and print the step count for axis Z\n\n");
        break;
    case hash_lasertoggle:
        Serial.print("lasertoggle\n");
        Serial.print("\tToggle laser on/off\n\n");
        break;
    case hash_pulsecount:
        Serial.print("pulsecount\n");
        Serial.print("\tReturn the current pulse count received from BNC.\n\n");
        break;
    case hash_resetpulse:
        Serial.print("resetpulse\n");
        Serial.print("\tReset pulse count to zero.\n\n");
        break;
    case hash_speed:
        Serial.print("speed\n");
        Serial.print("\tSet speed\n");
        Serial.print("\tUsage:\n\t\tspeed a xxxx yyyy\n");
        Serial.print("\tWhere:\n\t\ta = axis (x,y,z)\n\t\txxxx = Acceleration\n\t\tyyyy = Max speed\n\n");
        break;
    case hash_getspeed:
        Serial.print("getspeed\n");
        Serial.print("\tGet speed and acceleration information\n\n");
        break;
    case hash_grid_blanks:
        Serial.print("grid_blanks\n");
        Serial.print("\tHow many blanks before moving the sample\n");
        Serial.print("\tUsage:\n\t\tgrid_blanks n\n");
        Serial.print("\tWhere:\n\t\tn = Blank count\n\n");
        break;
    case hash_grid_initial_move:
        Serial.print("grid_initial_move\n");
        Serial.print("\tHow much to move after blanks\n");
        Serial.print("\tUsage:\n\t\tgrid_initial_move nnnn\n");
        Serial.print("\tWhere:\n\t\tnnnn = How many millimeter's the X axis moves, after set blank count received.\n\n");
        break;
    case hash_grid_shots:
        Serial.print("grid_shots\n");
        Serial.print("\tHow many shots on a single point\n");
        Serial.print("\tUsage:\n\t\tgrid_shots nnnn\n");
        Serial.print("\tWhere:\n\t\tnnnn = How many millimeter's the X axis moves, after set blank count received.\n\n");
        break;
    case hash_grid_move_x:
        Serial.print("grid_move_x\n");
        Serial.print("\tHow many millimeter's to move X axis after shots\n\n");
        break;
    case hash_grid_move_y:
        Serial.print("grid_move_y\n");
        Serial.print("\tHow many millimeter's to move Y axis changing row\n\n");
        break;
    case hash_grid_columns:
        Serial.print("grid_columns\n");
        Serial.print("\tHow many columns (x axis)\n\n");
        break;
    case hash_grid_rows:
        Serial.print("grid_rows\n");
        Serial.print("\tHow many rows (y axis)\n\n");
        break;
    case hash_move:
        Serial.print("move\n");
        Serial.print("\tMove an axis");
        Serial.print("\tUsage:\n\t\tmove a nnnn");
        Serial.print("\tWhere:\n\t\ta = axis (x,y,z)\n\t\tnnnn = millimeters\n\n");
        break;
    case hash_optom:
        Serial.print("optom\n");
        Serial.print("\tStart outputing OptoNCDT data.\n\n");
        break;
    case hash_optos:
        Serial.print("optos\n");
        Serial.print("\tThis would stop outputing OptoNCDT data, if it was displaying.\n\n");
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
    Serial.print("\r");
}

void func_createCmd(String cmdString) { Serial.println(parser.createCmd(cmdString)); }

void laser_bnc_interrupt() { pulseCount++; }

/// @brief Move an axis
/// @param stepper
/// @param min_limit
/// @param max_limit
/// @param distance
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
            Serial.print("MEND\r");
        }
        else
        {
            stepper.run();
        }
    }

    stepper.stop();
    stepper.setCurrentPosition(stepper.currentPosition());
}

/// @brief Set the speed and acceleration of specified axis
/// @param axis
/// @param speed
/// @param acceleration
void func_speed(char axis, float speed, float acceleration)
{
    switch (axis)
    {
    case 'x':
        stepperX.setMaxSpeed(speed * stepperX_ppmm);
        stepperX.setAcceleration(acceleration * stepperX_ppmm);
        Serial.println("X_SPEEDSET");
        break;
    case 'y':
        stepperY.setMaxSpeed(speed * stepperY_ppmm);
        stepperY.setAcceleration(acceleration * stepperY_ppmm);
        Serial.println("Y_SPEEDSET");
        break;
    case 'z':
        stepperZ.setMaxSpeed(speed * stepperZ_ppmm);
        stepperZ.setAcceleration(acceleration * stepperZ_ppmm);
        Serial.println("Z_SPEEDSET");
        break;
    default:
        usagePrint(hash_speed);
        break;
        Serial.print("\r");
    }
}

void func_getspeed(char axis)
{
    switch (axis)
    {
    case 'x':
        Serial.print(stepperX.maxSpeed() / stepperX_ppmm);
        Serial.print(':');
        Serial.print(stepperX.acceleration() / stepperX_ppmm);
        break;
    case 'y':
        Serial.print(stepperY.maxSpeed() / stepperY_ppmm);
        Serial.print(':');
        Serial.print(stepperY.acceleration() / stepperY_ppmm);
        break;
    case 'z':
        Serial.print(stepperZ.maxSpeed() / stepperZ_ppmm);
        Serial.print(':');
        Serial.print(stepperZ.acceleration() / stepperZ_ppmm);
        break;
    default:
        usagePrint(hash_speed);
        break;
    }
    Serial.print("\r");
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

void func_map()
{
    int curCount, curCol;
    int linCount = grid_cols * grid_rows;
    curCount = 0;
    curCol = 0;

    while (curCount < linCount)
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

        delay(500);
        Serial.println(epsilon.optoMeas());
        delay(100);
    }

    Serial.println('MAPCOMPLETE');
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
    Serial.print(x_min_bounce.isPressed() ? "Yes\n" : "No\n");
    Serial.print("X MAX STATE: ");
    Serial.print(x_max_bounce.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Y MIN STATE: ");
    Serial.print(y_min_bounce.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Y MAX STATE: ");
    Serial.print(y_max_bounce.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Z MIN STATE: ");
    Serial.print(z_min_bounce.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Z MAX STATE: ");
    Serial.print(z_max_bounce.isPressed() ? "Yes" : "No");
    Serial.print("\r");
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
    EEPROM.get(eeprom_address, savedValues);
    stepperX.setMaxSpeed(savedValues.xSpeed);
    stepperX.setAcceleration(savedValues.xAccel);
    stepperY.setMaxSpeed(savedValues.ySpeed);
    stepperY.setAcceleration(savedValues.yAccel);
    stepperZ.setMaxSpeed(savedValues.zSpeed);
    stepperZ.setAcceleration(savedValues.zAccel);
    // func_speed('x', 30.0, 1000.0);
    // func_speed('y', 30.0, 1000.0);
    // func_speed('z', 15.0, 1.5);

    stepperX.stop();
    stepperZ.stop();
    stepperY.stop();

    // Attach laser pulse BNC as interrupt
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
            // usagePrint(hash_lasertoggle);
            func_lasertoggle();
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
                Serial.print("\r");
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
                Serial.print("\r");
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
                Serial.print("\r");
            }
            break;
        case hash_grid_initial_move:
            if (cmd.paramCount == 1)
            {
                grid_ini_move = cmd.paramArray[0].toFloat() * stepperX_ppmm;
            }
            else
            {
                usagePrint(hash_grid_initial_move);
                Serial.print("\r");
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
                Serial.print("\r");
            }
            break;
        case hash_grid_move_x:
            if (cmd.paramCount == 1)
            {
                grid_mx = cmd.paramArray[0].toFloat() * stepperX_ppmm;
            }
            else
            {
                usagePrint(hash_grid_move_x);
                Serial.print("\r");
            }
            break;
        case hash_grid_move_y:
            if (cmd.paramCount == 1)
            {
                grid_my = cmd.paramArray[0].toFloat() * stepperY_ppmm;
            }
            else
            {
                usagePrint(hash_grid_move_y);
                Serial.print("\r");
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
                Serial.print("\r");
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
                Serial.print("\r");
            }
            break;
        case hash_move:
            if (cmd.paramCount == 2)
            {
                char axis = cmd.paramArray[0].charAt(0);
                float distance = cmd.paramArray[1].toFloat();
                switch (axis)
                {
                case 'x':
                    func_move(stepperX, x_min_bounce, x_max_bounce, distance * stepperX_ppmm);
                    break;
                case 'y':
                    func_move(stepperY, y_min_bounce, y_max_bounce, distance * stepperY_ppmm);
                    break;
                case 'z':
                    func_move(stepperZ, z_min_bounce, z_max_bounce, distance * stepperZ_ppmm);
                    break;
                default:
                    break;
                }
            }
            else
            {
                usagePrint(hash_move);
                Serial.print("\r");
            }

            break;
        case hash_optom:
            Serial.println(epsilon.optoMeas());
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
        case hash_save:
            // With this, we can save the set stepper motor speeds and acceleration to the EEPROM of the Teensy 4.0
            StepperValues sv;
            sv.xSpeed = stepperX.maxSpeed();
            sv.xAccel = stepperX.acceleration();
            sv.ySpeed = stepperY.maxSpeed();
            sv.yAccel = stepperY.acceleration();
            sv.zSpeed = stepperZ.maxSpeed();
            sv.zAccel = stepperZ.acceleration();

            EEPROM.put(eeprom_address, sv);
            delay(10);
            Serial.println("EEPROM_SAVED");
            break;
        case hash_load:
            // Load the information from the EEPROM
            EEPROM.get(eeprom_address, savedValues);
            Serial.println("Saved values loaded");
            break;
        case hash_createCmd:
            func_createCmd(cmd.paramArray[0]);
            break;
        case hash_map:
            func_map();
            break;
        default:
            func_createCmd(cmd.name);
            Serial.println("Unknown command. Use help to see list of commands.");
            break;
        }
    }
}
