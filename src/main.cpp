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
#define hash_setzhome 326166452
#define hash_homez 261599176
#define hash_timeit 505346961
#define hash_defocus 143439470
#define hash_defocus_offset 1608925396
#define hash_defocus_interval -260913486

/// @brief Structure that contains stepper controller, min and max limit switches, and steps per mm information.
struct StepperDriver
{
    AccelStepper motor;
    Bounce2::Button min_switch;
    Bounce2::Button max_switch;
    int spmm;
} stepperX, stepperY, stepperZ;

/// @brief Sturcture that contains speed and acceleration infromation for X,Y,Z drivers and the Z home distance.
struct EEPROMValues
{
    int xSpeed;
    int xAccel;
    int ySpeed;
    int yAccel;
    int zSpeed;
    int zAccel;
    float zHome;
} savedValues;

int eeprom_address = 42;

CmdParse parser = CmdParse();
optoNCDT epsilon = optoNCDT();

bool optoLaser = true;

unsigned long grid_blanks = 0;
float grid_ini_move = 0;
int grid_ppp = 0;
float grid_mx = 0;
float grid_my = 0;
int grid_cols = 0;
int grid_rows = 0;
float zHomeDistance = 0;

bool defocus = false;
float defocus_offset = 0.0;
int defocus_interval = 0;

/// @brief Prints information on how to use specified function
/// @param cmdHash
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
    case hash_defocus:
        Serial.print("defocus\n");
        Serial.print("\tDefocus enabled/disabled\n");
        Serial.print("\tUsage:\n\t\tdefocus [True/False]\n\n");
        break;
    case hash_defocus_interval:
        Serial.print("defocus_interval\n");
        Serial.print("\tHow many positions before defocus offset\n");
        break;
    case hash_defocus_offset:
        Serial.print("defocus_offset\n");
        Serial.print("\tSet defocus offset amount in mm\n");
        break;
    default:
        break;
    }
}

/// @brief Prints all possible commands and their usage information
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

/// @brief Move specified axis
/// @param stepper Stepper driver structure
/// @param distance Distance to move in mm
void func_move(StepperDriver &ref_stepper, float distance_mm)
{
    bool moving = true;
    long distance = distance_mm * ref_stepper.spmm;
    ref_stepper.motor.move(distance);

    while (moving)
    {
        ref_stepper.min_switch.update();
        ref_stepper.max_switch.update();

        if (
            (distance < 0 && (ref_stepper.min_switch.isPressed() || ref_stepper.min_switch.rose())) ||
            (distance > 0 && (ref_stepper.max_switch.isPressed() || ref_stepper.max_switch.rose())) ||
            ref_stepper.motor.distanceToGo() == 0)
        {
            moving = false;
            ref_stepper.motor.stop();
            Serial.print("MEND\r");
        }
        else
        {
            ref_stepper.motor.run();
        }
    }

    ref_stepper.motor.stop();
    ref_stepper.motor.setCurrentPosition(ref_stepper.motor.currentPosition());
}

/// @brief Set the speed and acceleration of specified axis
/// @param axis
/// @param speed Unit is mm/s
/// @param acceleration Unit is mm/s^2
void func_speed(char axis, float speed, float acceleration)
{
    switch (axis)
    {
    case 'x':
        stepperX.motor.setMaxSpeed(speed * stepperX.spmm);
        stepperX.motor.setAcceleration(acceleration * stepperX.spmm);
        Serial.println("X_SPEEDSET");
        break;
    case 'y':
        stepperY.motor.setMaxSpeed(speed * stepperY.spmm);
        stepperY.motor.setAcceleration(acceleration * stepperY.spmm);
        Serial.println("Y_SPEEDSET");
        break;
    case 'z':
        stepperZ.motor.setMaxSpeed(speed * stepperZ.spmm);
        stepperZ.motor.setAcceleration(acceleration * stepperZ.spmm);
        Serial.println("Z_SPEEDSET");
        break;
    default:
        usagePrint(hash_speed);
        break;
        Serial.print("\r");
    }
}

/// @brief Prints speed and acceleration of specified axis. Format is `speed:acceleration`
/// @param axis
void func_getspeed(char axis)
{
    switch (axis)
    {
    case 'x':
        Serial.print(stepperX.motor.maxSpeed() / stepperX.spmm);
        Serial.print(':');
        Serial.print(stepperX.motor.acceleration() / stepperX.spmm);
        break;
    case 'y':
        Serial.print(stepperY.motor.maxSpeed() / stepperY.spmm);
        Serial.print(':');
        Serial.print(stepperY.motor.acceleration() / stepperY.spmm);
        break;
    case 'z':
        Serial.print(stepperZ.motor.maxSpeed() / stepperZ.spmm);
        Serial.print(':');
        Serial.print(stepperZ.motor.acceleration() / stepperZ.spmm);
        break;
    default:
        usagePrint(hash_speed);
        break;
    }
    Serial.print("\r");
}

void func_posx() { Serial.println(stepperX.motor.currentPosition()); }
void func_posy() { Serial.println(stepperY.motor.currentPosition()); }
void func_posz() { Serial.println(stepperZ.motor.currentPosition()); }

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

/// @brief Home the Z axis, using sensor reading from Micro-Epsilon optoNCDT 1900 and the saved Z home distance.
/// @param ref_driver Stepper driver for Z axis
void func_homez(StepperDriver &ref_driver)
{
    
    float z_dist = epsilon.optoMeas();
    float dToGo = zHomeDistance - z_dist;
    long dToGoSteps = dToGo * ref_driver.spmm;
    bool moving = true;
    bool limit = false;

    if (z_dist < 200'000)
    {
        Serial.println(dToGo);
        // while ((dToGo >= 0.01 || dToGo <= -0.01) && moving)
        while (abs(dToGo) >= 0.01 && moving)
        {
            ref_driver.motor.move(-1 * dToGoSteps);

            ref_driver.min_switch.update();
            ref_driver.max_switch.update();

            if (ref_driver.min_switch.isPressed() || ref_driver.min_switch.rose() ||
                ref_driver.max_switch.isPressed() || ref_driver.max_switch.rose())
            {
                dToGo = 0.01;
                Serial.println("Z LIMIT PRESSED");
                // Serial.println(dToGo);
                ref_driver.motor.stop();
                moving = false;
                limit = true;
            }
            else if (ref_driver.motor.distanceToGo() == 0)
            {
                moving = false;
                ref_driver.motor.stop();
            }
            else
            {
                ref_driver.motor.run();
            }

            z_dist = epsilon.optoMeas();
            if (z_dist < 200'000 && !limit)
            {
                dToGo = zHomeDistance - z_dist;
                dToGoSteps = dToGo * ref_driver.spmm;
            }
            else
            {
                moving = false;
            }
        }
    }
    else
    {
        Serial.println(z_dist);
    }

    ref_driver.motor.stop();
    ref_driver.motor.setCurrentPosition(ref_driver.motor.currentPosition());

    Serial.println("HOMEDONE");
}

/// @brief Start grid movement.
void func_gridMove()
{
    Serial.println("GRIDSTART");
    bool warm = false;
    float curZHomeDist = zHomeDistance;
    int curMove, curCount, curRow, curCol;
    curMove = curCount = curRow = curCol = 0;
    int linCount = grid_cols * grid_rows;
    long unsigned int pco = 0;
    epsilon.optoCmd("LASERPOW OFF");

    while (curCount < linCount)
    {
        if (pulseCount != pco)
        {
            Serial.print("Pulse count: ");
            Serial.print(pulseCount);
            Serial.print("\tCur count: ");
            Serial.print(curCount);
            Serial.print("\tLin count: ");
            Serial.println(linCount);
            pco = pulseCount;
        }

        if (warm)
        {
            curMove = pulseCount - (grid_blanks + curCount * grid_ppp);
            if (curMove == grid_ppp)
            {
                epsilon.optoCmd("LASERPOW FULL");
                delay(5);
                if (curCol < (grid_cols - 1))
                {
                    Serial.println("MOVEX");
                    func_move(stepperX, grid_mx);
                    curCol++;
                }
                else
                {
                    Serial.println("MOVEY");
                    func_move(stepperY, grid_my);
                    grid_mx *= -1;
                    curCol = 0;
                }

                if (defocus && curCount > 0 && (curCount + 1) % defocus_interval == 0)
                {
                    zHomeDistance += defocus_offset;
                    Serial.println("OFFSET");
                }

                // Move the Z axis and check if correction exceeds 1 mm
                float z_dist = epsilon.optoMeas();
                float dToGo = zHomeDistance - z_dist;
                if (abs(dToGo) > 1.0)  // Check if Z correction exceeds 1 mm
                {
                    Serial.println("OVERZ");
                    zHomeDistance = curZHomeDist;
                    break;  // Stop grid movement
                }

                func_homez(stepperZ);
                epsilon.optoCmd("LASERPOW OFF");

                if (pulseCount != pco)
                {
                    Serial.println("!!PULSE COUNT OUT OF SYNC!!");
                }

                curCount++;
                Serial.println("GRIDMOVEND");
            }
        }
        else if (pulseCount == grid_blanks)
        {
            epsilon.optoCmd("LASERPOW FULL");
            warm = true;
            func_move(stepperX, grid_ini_move);
            func_homez(stepperZ);
            epsilon.optoCmd("LASERPOW OFF");
        }
    }

    //Serial.println("Last Z homedistance: " + String(zHomeDistance));
    //Serial.println("Setting Z homedistance back to: " + String(curZHomeDist));
    zHomeDistance = curZHomeDist;
    Serial.println("GRIDCOMPLETE");
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
            func_move(stepperX, grid_mx);
            curCol++;
        }
        else
        {
            func_move(stepperY, grid_my);
            grid_mx *= -1;
            curCol = 0;
        }
        curCount++;

        delay(500);
        Serial.println(epsilon.optoMeas());
        delay(100);
    }

    Serial.println("MAPCOMPLETE");
}

/// @brief Print status of limit switches
/// @param driverX Stepper driver for X axis
/// @param driverY Stepper driver for Y axis
/// @param driverZ Stepper driver for Z axis
void func_lim(StepperDriver &driverX, StepperDriver &driverY, StepperDriver &driverZ)
{

    driverX.max_switch.update();
    driverX.min_switch.update();
    driverY.max_switch.update();
    driverY.min_switch.update();
    driverZ.max_switch.update();
    driverZ.min_switch.update();

    Serial.print("X MIN STATE: ");
    Serial.print(driverX.min_switch.isPressed() ? "Yes\n" : "No\n");
    Serial.print("X MAX STATE: ");
    Serial.print(driverX.max_switch.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Y MIN STATE: ");
    Serial.print(driverY.min_switch.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Y MAX STATE: ");
    Serial.print(driverY.max_switch.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Z MIN STATE: ");
    Serial.print(driverZ.min_switch.isPressed() ? "Yes\n" : "No\n");
    Serial.print("Z MAX STATE: ");
    Serial.print(driverZ.max_switch.isPressed() ? "Yes" : "No");
    Serial.print("\r");
}

void func_timeit()
{
    elapsedMillis sincePrint;
    float totalTime = 0;
    epsilon.optoCmd("LASERPOW OFF");
    delay(500);
    float z_dist = epsilon.optoMeas();
    size_t count = 100;

    for (size_t i = 0; i < count; i++)
    {
        epsilon.optoCmd("LASERPOW OFF");
        delay(100);
        z_dist = 500'000;
        sincePrint = 0;

        epsilon.optoCmd("LASERPOW FULL");
        while (z_dist > 200'000)
        {
            z_dist = epsilon.optoMeas();
            Serial.print("Optomeas output: ");
            Serial.println(z_dist);
        }
        totalTime += sincePrint;
        Serial.print(sincePrint);
        Serial.println(" ms");
    }

    Serial.println("----------");
    Serial.print("Test count: ");
    Serial.println(count);
    Serial.print("Total time: ");
    Serial.print(totalTime);
    Serial.println(" ms");
    Serial.print("AVG TIME: ");
    Serial.println(totalTime / count);
}

void setup()
{
    Serial.begin(9600);
    epsilon.begin(921600, SERIAL_8N1);
    epsilon.setMeasuringRange(100);

    stepperX.motor = AccelStepper(AccelStepper::DRIVER, x_pulse, x_direction);
    stepperY.motor = AccelStepper(AccelStepper::DRIVER, y_pulse, y_direction);
    stepperZ.motor = AccelStepper(AccelStepper::DRIVER, z_pulse, z_direction);

    // How many pulses it takes to move 1 mm
    stepperX.spmm = 3200;
    stepperY.spmm = 3200;
    stepperZ.spmm = 320;

    stepperX.max_switch = Bounce2::Button();
    stepperX.min_switch = Bounce2::Button();
    stepperY.max_switch = Bounce2::Button();
    stepperY.min_switch = Bounce2::Button();
    stepperZ.max_switch = Bounce2::Button();
    stepperZ.min_switch = Bounce2::Button();

    // // Attach buttons
    stepperX.max_switch.attach(x_max, INPUT_PULLDOWN);
    stepperX.min_switch.attach(x_min, INPUT_PULLDOWN);
    stepperY.max_switch.attach(y_max, INPUT_PULLDOWN);
    stepperY.min_switch.attach(y_min, INPUT_PULLDOWN);
    stepperZ.max_switch.attach(z_max, INPUT_PULLDOWN);
    stepperZ.min_switch.attach(z_min, INPUT_PULLDOWN);

    // Set stepper motor max speed and acceleration
    EEPROM.get(eeprom_address, savedValues);
    stepperX.motor.setMaxSpeed(savedValues.xSpeed);
    stepperX.motor.setAcceleration(savedValues.xAccel);
    stepperY.motor.setMaxSpeed(savedValues.ySpeed);
    stepperY.motor.setAcceleration(savedValues.yAccel);
    stepperZ.motor.setMaxSpeed(savedValues.zSpeed);
    stepperZ.motor.setAcceleration(savedValues.zAccel);
    zHomeDistance = savedValues.zHome;

    stepperX.motor.stop();
    stepperZ.motor.stop();
    stepperY.motor.stop();

    // Attach laser pulse BNC as interrupt
    attachInterrupt(digitalPinToInterrupt(laser_bnc), laser_bnc_interrupt, RISING);
}

void loop()
{

    stepperX.max_switch.update();
    stepperX.min_switch.update();
    stepperY.max_switch.update();
    stepperY.min_switch.update();
    stepperZ.max_switch.update();
    stepperZ.min_switch.update();

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
            func_lasertoggle();
            break;
        case hash_pulsecount:
            func_pulsecount();
            break;
        case hash_resetpulse:
            pulseCount = 0;
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
                // This variable is used as a limit before the grid starts
                grid_blanks = cmd.paramArray[0].toInt();
                Serial.println(grid_blanks);
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
                // This variable is the distance the X axis moves after the blanking system is done.
                grid_ini_move = cmd.paramArray[0].toFloat();
                Serial.println(grid_ini_move);
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
                // This variable references to pulses per position, telling the system how many times a single point should be pulsed by the laser.
                grid_ppp = cmd.paramArray[0].toInt();
                Serial.println(grid_ppp);
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
                // General value to tell grid function, how many mm to move the X axis, between grid positions
                grid_mx = cmd.paramArray[0].toFloat();
                Serial.println(grid_mx);
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
                // General value to tell grid function, how many mm to move the Y axis, when moving to another row
                grid_my = cmd.paramArray[0].toFloat();
                Serial.println(grid_my);
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
                // This value is used for grid function, to calculate how many total pulses is needed to meet wanted points and how many times the X axis needs to move, before calling a move for the Y axis
                grid_cols = cmd.paramArray[0].toInt();
                Serial.println(grid_cols);
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
                // This value is used for grid function, to calculate how many total pulses is needed to meet wanted points and how many times the Y axis needs to move
                grid_rows = cmd.paramArray[0].toInt();
                Serial.println(grid_rows);
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
                    func_move(stepperX, distance);
                    break;
                case 'y':
                    func_move(stepperY, distance);
                    break;
                case 'z':
                    func_move(stepperZ, distance);
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

        case hash_defocus:
            if (cmd.paramCount == 1)
            {
                // This command is used to enable/disable defocus, which is used to offset the Z axis after a certain amount of positions when shooting in a grid.
                String paramValue = cmd.paramArray[0];
                if (paramValue.equalsIgnoreCase("True")) {
                    defocus = true;
                } else if (paramValue.equalsIgnoreCase("False")) {
                    defocus = false;
                }
                Serial.println(defocus);
            }
            else
            {
                usagePrint(hash_defocus);
                Serial.print("\r");
            }
            break;

        case hash_defocus_offset:
            if (cmd.paramCount == 1)
            {
                // This command is used to set the defocus offset amount in mm.
                defocus_offset = cmd.paramArray[0].toFloat();
                Serial.println(defocus_offset);
            }
            else
            {
                usagePrint(hash_defocus_offset);
                Serial.print("\r");
            }
            break;

        case hash_defocus_interval:
            if (cmd.paramCount == 1)
            {
                // This command is used to set the defocus interval, which is used to tell the system how many positions it should move before applying the defocus offset.
                defocus_interval = cmd.paramArray[0].toInt();
                Serial.println(defocus_interval);
            }
            else
            {
                usagePrint(hash_defocus_interval);
                Serial.print("\r");
            }
            break;
            
        case hash_optom:
            // Print the distance reading from the Micro-Epsilon optoNCDT 1900
            Serial.clear();
            Serial.println(epsilon.optoMeas());
            break;
        case hash_optoc:
            // Send a command to the Micro-Epsilon optoNCDT 1900 sensor
            Serial.println(epsilon.optoCmd(cmd.parameters));
            break;
        case hash_lim:
            func_lim(stepperX, stepperY, stepperZ);
            break;
        case hash_help:
            func_help();
            break;
        case hash_save:
            // With this, we can save the set stepper motor speeds and acceleration to the EEPROM of the Teensy 4.0
            EEPROMValues sv;
            sv.xSpeed = stepperX.motor.maxSpeed();
            sv.xAccel = stepperX.motor.acceleration();
            sv.ySpeed = stepperY.motor.maxSpeed();
            sv.yAccel = stepperY.motor.acceleration();
            sv.zSpeed = stepperZ.motor.maxSpeed();
            sv.zAccel = stepperZ.motor.acceleration();
            sv.zHome = zHomeDistance;

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
        case hash_setzhome:
            if (cmd.paramCount == 1)
            {
                float distance = cmd.paramArray[0].toFloat();
                zHomeDistance = distance;
            }

            Serial.println(zHomeDistance);
            break;
        case hash_homez:
            func_homez(stepperZ);
            break;
        case hash_timeit:
            func_timeit();
            break;
        default:
            Serial.println("Unknown command. Use help to see list of commands.");
            break;
        }
    }
}
