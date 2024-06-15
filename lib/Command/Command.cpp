#include "Command.h"

CmdParse::CmdParse(int defaultPara, char defaultDelim) : para(defaultPara), delim(defaultDelim)
{
    param_limit = defaultPara;
    delimiter = defaultDelim;
}

int CmdParse::hash(String str)
{
    int hash = 5381;
    int c;

    const char *cstr = str.c_str();

    while ((c = *cstr++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

Command CmdParse::parseCommand(String &input)
{
    Command cmd;

    for (int i = 0; i < param_limit; i++)
    {
        cmd.paramArray[i] = ""; // Set each element to an empty string
    }

    cmd.paramCount = 0;
    cmd.name = "";
    cmd.nameHash = 0;

    input.replace("\r", "");
    input.replace("\n", "");

    // Find the position of the first space character
    int spacePos = input.indexOf(delimiter);
    // If space is found
    if (spacePos != -1)
    {
        // Extract the command name
        cmd.name = input.substring(0, spacePos);
        cmd.nameHash = hash(input.substring(0, spacePos));
        // Extract the parameters (if any)
        cmd.parameters = input.substring(spacePos + 1);

        int partIndex = 0; // Index to keep track of the current part
        int lastIndex = 0; // Index to keep track of the last split position
        String str = cmd.parameters;
        // Iterate over each character in the string
        for (uint i = 0; i < str.length(); i++)
        {
            // If the current character is the delimiter or the end of the string
            if (str.charAt(i) == delimiter || i == str.length() - 1)
            {
                // Extract the part between the last split position and the current position
                cmd.paramArray[partIndex++] = str.substring(lastIndex, i + 1);

                // Update the last split position
                lastIndex = i + 1;

                // If the maximum number of parts has been reached, break out of the loop
                if (partIndex >= param_limit)
                {
                    break;
                }
            }
        }

        cmd.paramCount = partIndex;
    }
    else
    {
        // If no space is found, the entire input is considered as the command name
        cmd.name = input;
        cmd.nameHash = hash(input);
        cmd.parameters = "";
        cmd.paramCount = 0;
    }

    return cmd;
}

String CmdParse::createCmd(String cmdName)
{
    // int hash = parser.hash(cmdString);
    // Serial.print("#define hash_");
    // Serial.print(cmdString);
    // Serial.print(" ");
    // Serial.print(hash);
    int cmdHash = hash(cmdName);
    String command = "#define hash_";
    command.concat(cmdName);
    command.concat(" ");
    command.concat(String(cmdHash));
    return command;
}