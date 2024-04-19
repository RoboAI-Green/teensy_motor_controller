#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>

#define z_pulse 0
#define z_direction 1
#define z_enable 2

#define x_pulse 3
#define x_direction 4
#define x_enable 5

#define y_pulse 10
#define y_direction 11
#define y_enable 12

#define z_max 6
#define z_min 7
#define x_max 8
#define x_min 9
#define y_max 22
#define y_min 23

#define z_max_led 14
#define z_min_led 15
#define x_max_led 16
#define x_min_led 17

#define sick_output 18
#define sick_toggle 12

#define pulse_bnc 19

#define powled 13

#define debounce_interval 25 // In milliseconds

AccelStepper stepperX(1, x_pulse, x_direction);
AccelStepper stepperZ(1, z_pulse, z_direction);
AccelStepper stepperY(1,y_pulse, y_direction);

Bounce2::Button z_max_bounce = Bounce2::Button();
Bounce2::Button z_min_bounce = Bounce2::Button();
Bounce2::Button x_max_bounce = Bounce2::Button();
Bounce2::Button x_min_bounce = Bounce2::Button();
Bounce2::Button y_max_bounce = Bounce2::Button();
Bounce2::Button y_min_bounce = Bounce2::Button();
Bounce2::Button sick_bounce = Bounce2::Button();
Bounce2::Button bnc_bounce = Bounce2::Button();

const int posx = 2090623343;
const int posz = 2090623345;
const int posy = 2090623344;
const int lasertoggle = -1635822754;
const int pulsecount = 1781759127;
const int resetpulse = -1903504655;
const int speed = 274671510;
const int getspeed = -2066154;
const int linear_blanks = -1196627846;
const int linear_initial_move = -43909953;
const int linear_shots = 374222480;
const int linear_move_x = -761836275;
const int linear_move_y = -761836274;
const int linear_columns = 588159712;
const int linear_rows = 1442967786;
const int move = 2090515612;

struct Command {
    String name;
    int nameHash;
    String parameters;
};

int hash(String str){
    int hash = 5381;
    int c;

    const char *cstr = str.c_str();

    while ((c=*cstr++)){
        hash = ((hash << 5) + hash) +c;
    }

    return hash;
}

void removeNewlines(String& str) {
    str.replace("\r", ""); // Remove carriage return characters
    str.replace("\n", ""); // Remove newline characters
}

Command parseCommand(String& input) {
    Command cmd;

    // Find the position of the first space character
    int spacePos = input.indexOf(' ');
    removeNewlines(input);
    // If space is found
    if (spacePos != -1) {
        // Extract the command name
        cmd.name = input.substring(0, spacePos);
        cmd.nameHash = hash(input.substring(0, spacePos));
        // Extract the parameters (if any)
        cmd.parameters = input.substring(spacePos + 1);
    } else {
        // If no space is found, the entire input is considered as the command name
        cmd.name = input;
        cmd.nameHash = hash(input);
    }


    return cmd;
}

// void hashPrint(String command){
//     command.replace("-","_");
//     Serial.print("const int ");
//     Serial.print(command);
//     Serial.print(" = ");
//     Serial.print(hash(command));
//     Serial.println(";");
// }

// void hashCommands() {
//     hashPrint("posx");
//     hashPrint("posz");
//     hashPrint("posy");
//     hashPrint("lasertoggle");
//     hashPrint("pulsecount");
//     hashPrint("resetpulse");
//     hashPrint("speed");
//     hashPrint("getspeed");
//     hashPrint("linear-blanks");
//     hashPrint("linear-initial-move");
//     hashPrint("linear-shots");
//     hashPrint("linear-move-x");
//     hashPrint("linear-move-y");
//     hashPrint("linear-columns");
//     hashPrint("linear-rows");
//     // hashPrint("linear");
//     hashPrint("move");
// }


void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    analogWrite(LED_BUILTIN, 1);
}

void loop() {

    if(Serial.available()){
        String userInput = Serial.readStringUntil('\n');

        Command cmd = parseCommand(userInput);

        switch (cmd.nameHash)
        {
            case posx:
                Serial.println("Get and print the step count for axis X");
                break;
            case posy:
                Serial.println("Get and print the step count for axis Y");
                break;
            case posz:
                Serial.println("Get and print the step count for axis Z");
                break;
            case lasertoggle:
                Serial.println("Toggle laser");
                break;
            case pulsecount:
                Serial.println("Pulse count");
                break;
            case resetpulse:
                Serial.println("Reset pulses");
                break;
            case speed:
                Serial.println("Set speed");
                break;
            case getspeed:
                Serial.println("Get speed information");
                break;
            case linear_blanks:
                Serial.println("How many blanks before moving the sample");
                break;
            case linear_initial_move:
                Serial.println("How much to move after blanks");
                break;
            case linear_shots:
                Serial.println("How many shots on a single point");
                break;
            case linear_move_x:
                Serial.println("How many steps to move on X axis after shots");
                break;
            case linear_move_y:
                Serial.println("How many steps to move when changing row");
                break;
            case linear_columns:
                Serial.println("How many columns (x axis)");
                break;
            case linear_rows:
                Serial.println("How many rows (y axis)");
                break;
            case move:
                Serial.println("Move an axis");
            default:
                Serial.println("UNKNOWN COMMAND");
                // hashCommands();
                break;
        }

    }
}
