#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>

#define PCUSB_SERIAL Serial
#define OPTO_SERIAL Serial1

#define TOO_MUCH_DATA   262075
#define NO_PEAK         262076
#define PEAK_FRONT      262077
#define PEAK_AFTER      262078
#define GLOBAL_ERROR    262080
#define PEAK_WIDE       262081
#define LASER_OFF       262082

#define z_pulse         0
#define z_direction     1
#define z_enable        2

#define x_pulse         3
#define x_direction     4
#define x_enable        5

#define y_pulse         10
#define y_direction     11
#define y_enable        12

#define z_max           6
#define z_min           7
#define x_max           8
#define x_min           9
#define y_max           22
#define y_min           23

#define z_max_led       14
#define z_min_led       15
#define x_max_led       19
#define x_min_led       20

// #define pulse_bnc       19

#define powled          13

#define debounce_ms     25

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
const int optom = 269944500;
const int optos = 269944506;

const int MR = 100;
int sender = 1;

String param[MR];

void resetArray() {
  for (int i = 0; i < MR; i++) {
    param[i] = ""; // Set each element to an empty string
  }
}

int splitString(String str, char delimiter) {
  int partIndex = 0; // Index to keep track of the current part
  int lastIndex = 0; // Index to keep track of the last split position
  
  // Iterate over each character in the string
  for (uint i = 0; i < str.length(); i++) {
    // If the current character is the delimiter or the end of the string
    if (str.charAt(i) == delimiter || i == str.length() - 1) {
      // Extract the part between the last split position and the current position
      param[partIndex++] = str.substring(lastIndex, i + 1);
      
      // Update the last split position
      lastIndex = i + 1;
      
      // If the maximum number of parts has been reached, break out of the loop
      if (partIndex >= MR) {
        break;
      }
    }
  }
  
  // Return the number of split parts
  return partIndex;
}


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

void opto_meas(){
    while(true)
  {
    uint8_t lByte, mByte, hByte;
    // Read the L-Byte packet
    while (true) {
      if (OPTO_SERIAL.available() >= 1) {
        lByte = OPTO_SERIAL.read();
        if ((lByte >> 6) == 0) { // Check if the flag bits are correct
          break;
        }
      }
    }

    // Read the M-Byte packet
    while (true) {
      if (OPTO_SERIAL.available() >= 1) {
        mByte = OPTO_SERIAL.read();
        if ((mByte >> 6) == 1) { // Check if the flag bits are correct
          break;
        }
      }
    }

    // Read the H-Byte packet
    while (true) {
      if (OPTO_SERIAL.available() >= 1) {
        hByte = OPTO_SERIAL.read();
        if ((hByte >> 6) == 2) { // Check if the flag bits are correct
          break;
        }
      }
    }

    // Extract the 6 bits of data from each packet
    uint32_t x = ((hByte & 0x3F) << 12) | ((mByte & 0x3F) << 6) | (lByte & 0x3F);

    if (x >=0 && x<=230604){
      float calcVal = ((float(x)-98232)/65536)*MR;
      PCUSB_SERIAL.println(calcVal);
    }else if (x == TOO_MUCH_DATA){
      PCUSB_SERIAL.println("Too much data for selected baud rate!");
    }else if(x == NO_PEAK) {
      PCUSB_SERIAL.println("No signal coming back!");
    }else if(x == PEAK_FRONT){
      PCUSB_SERIAL.println("Object is too close to the sensor!");
    }else if(x == PEAK_AFTER){
      PCUSB_SERIAL.println("Object is too far to measure!");
    }else if(x == GLOBAL_ERROR){
      PCUSB_SERIAL.println("Measurement value cannot be evaluated, global error");
    }else if(x == PEAK_WIDE){
      PCUSB_SERIAL.println("Peak is too wide");
    }else if(x == LASER_OFF){
      PCUSB_SERIAL.println("LASER IS OFF");
    }else{
      PCUSB_SERIAL.print("Unknown bit: ");
      PCUSB_SERIAL.println(x);
    }

    if (PCUSB_SERIAL.available()) {
      // Read one byte from the serial port
      String receivedString = PCUSB_SERIAL.readStringUntil('\n');
      Command cmd = parseCommand(receivedString);
      
      if (cmd.nameHash == optos) {
        PCUSB_SERIAL.println("Stopping output");
        return;
      }
    }

    OPTO_SERIAL.clear();
  }
}

/// @brief Move the specified axis, in a set distance to set direction
/// @param axis
/// @param direction
/// @param distance
// void motion(String axis, String direction, int distance)
void motion(String parameters)
{

    String axis = parameters.substring(0,1);
    String direction = parameters.substring(1,2);
    int distance = parameters.substring(2,parameters.length()).toInt();


    Serial.print("Axis: ");
    Serial.print(axis);
    Serial.print("\t Direction: ");
    Serial.print(direction);
    Serial.print("\t Distance: ");
    Serial.print(distance);
    Serial.print("\n");

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
            if ((direction.equals('m') && (z_min_bounce.isPressed() || z_min_bounce.rose())) || (direction.equals('p') && (z_max_bounce.rose() || z_max_bounce.isPressed() || sick_bounce.rose() || sick_bounce.isPressed())) || stepperZ.distanceToGo() == 0)
            {
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
            if ((direction.equals('m') && (x_min_bounce.isPressed() || x_min_bounce.rose())) || (direction.equals('p') && (x_max_bounce.rose() || x_max_bounce.isPressed())) || stepperX.distanceToGo() == 0)
            {
                moving = false;
                break;
            }
            else
            {
                stepperX.run();
            }
        }

        stepperX.setCurrentPosition(stepperX.currentPosition());
    }else if(axis.equals("y")){
        if(direction.equals('m')){distance = -distance;}

        stepperY.move(distance);
        while(moving){
            y_max_bounce.update();
            y_min_bounce.update();
            if ((direction.equals('m')&&(y_min_bounce.isPressed() || y_min_bounce.rose())) || (direction.equals('p') && (y_max_bounce.rose() ||y_max_bounce.isPressed())) || stepperY.distanceToGo() == 0){
                moving = false;
                break;
            }
            else{
                stepperY.run();
            }
        }

        stepperY.setCurrentPosition(stepperY.currentPosition());
    }


    Serial.print("Movement complete.\r");
}


void func_speed(char axis, int speed, int acceleration){
    PCUSB_SERIAL.println(axis);
    PCUSB_SERIAL.println(speed);
    PCUSB_SERIAL.println(acceleration);

}


void z_max_led_int() { digitalWrite(z_max_led, digitalRead(z_max)); }
void z_min_led_int() { digitalWrite(z_min_led, digitalRead(z_min)); }
void x_max_led_int() { digitalWrite(x_max_led, digitalRead(x_max)); }
void x_min_led_int() { digitalWrite(x_min_led, digitalRead(x_min)); }

void setup() {
    PCUSB_SERIAL.begin(9600);
    OPTO_SERIAL.begin(921600, SERIAL_8N1);
    pinMode(powled, OUTPUT);
    analogWrite(powled, 1);

    z_max_bounce.attach(z_max, INPUT_PULLUP);
    z_min_bounce.attach(z_min, INPUT_PULLUP);
    x_max_bounce.attach(x_max, INPUT_PULLUP);
    x_min_bounce.attach(x_min, INPUT_PULLUP);
    y_max_bounce.attach(y_max, INPUT_PULLUP);
    y_min_bounce.attach(y_min, INPUT_PULLUP);
    // sick_bounce.attach(sick_output, INPUT_PULLUP);
    // bnc_bounce.attach(pulse_bnc,INPUT_PULLUP);

    z_max_bounce.interval(debounce_ms);
    z_min_bounce.interval(debounce_ms);
    x_max_bounce.interval(debounce_ms);
    x_min_bounce.interval(debounce_ms);
    y_max_bounce.interval(debounce_ms);
    y_min_bounce.interval(debounce_ms);
    // sick_bounce.interval(debounce_ms);
    // bnc_bounce.interval(1);

    attachInterrupt(digitalPinToInterrupt(x_max), x_max_led_int, CHANGE);
    attachInterrupt(digitalPinToInterrupt(x_min), x_min_led_int, CHANGE);

    pinMode(x_max_led,OUTPUT);
    pinMode(x_min_led,OUTPUT);

    stepperX.setMaxSpeed(500);
    stepperX.setAcceleration(500);
    stepperX.stop();

}

void loop() {

    if(PCUSB_SERIAL.available()){
        String userInput = PCUSB_SERIAL.readStringUntil('\n');

        Command cmd = parseCommand(userInput);
        int paramNum = splitString(cmd.parameters, ' ');
        
        switch (cmd.nameHash)
        {
            case posx:
                PCUSB_SERIAL.println("Get and print the step count for axis X");
                break;
            case posy:
                PCUSB_SERIAL.println("Get and print the step count for axis Y");
                break;
            case posz:
                PCUSB_SERIAL.println("Get and print the step count for axis Z");
                break;
            case lasertoggle:
                PCUSB_SERIAL.println("Toggle laser");
                break;
            case pulsecount:
                PCUSB_SERIAL.println("Pulse count");
                break;
            case resetpulse:
                PCUSB_SERIAL.println("Reset pulses");
                break;
            case speed:
                PCUSB_SERIAL.println("Set speed");
                if(paramNum == 3){
                    func_speed(param[0].charAt(0), param[1].toInt(), param[2].toInt());
                }else{
                    PCUSB_SERIAL.println("Usage: speed axis[char] speed[int] acceleration[int]");
                }
                break;
            case getspeed:
                PCUSB_SERIAL.println("Get speed information");
                break;
            case linear_blanks:
                PCUSB_SERIAL.println("How many blanks before moving the sample");
                break;
            case linear_initial_move:
                PCUSB_SERIAL.println("How much to move after blanks");
                break;
            case linear_shots:
                PCUSB_SERIAL.println("How many shots on a single point");
                break;
            case linear_move_x:
                PCUSB_SERIAL.println("How many steps to move on X axis after shots");
                break;
            case linear_move_y:
                PCUSB_SERIAL.println("How many steps to move when changing row");
                break;
            case linear_columns:
                PCUSB_SERIAL.println("How many columns (x axis)");
                break;
            case linear_rows:
                PCUSB_SERIAL.println("How many rows (y axis)");
                break;
            case move:
                PCUSB_SERIAL.println("Move an axis");
                motion(cmd.parameters);
                break;
            case optom:
                PCUSB_SERIAL.println("Start outputing OptoNCDT data.");
                break;
            case optos:
                PCUSB_SERIAL.println("This would stop outputing OptoNCDT data, if it was displaying.");
                break;
            default:
                PCUSB_SERIAL.println("UNKNOWN COMMAND");
                PCUSB_SERIAL.println(cmd.nameHash);
                break;
        }

    }

    digitalWrite(z_max_led, digitalRead(z_max));
    digitalWrite(z_min_led, digitalRead(z_min));
    digitalWrite(x_max_led, digitalRead(x_max));
    digitalWrite(x_min_led, digitalRead(x_min));
}
