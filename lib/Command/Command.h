#ifndef Command_h
#define Command_h

#include "Arduino.h"

struct Command
{
    String name;
    int nameHash;
    String parameters;
    int paramCount;
    String paramArray[100];
};

class CmdParse
{
public:
    CmdParse(int defaultPara = 10, char defaultDelim = ' ');
    Command parseCommand(String &input);
    int hash(String str);
    String createCmd(String str);

private:
    int para;
    char delim;
    int param_limit;
    char delimiter;
};

#endif