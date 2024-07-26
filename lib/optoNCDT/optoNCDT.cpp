#include "optoNCDT.h"

#define TOO_MUCH_DATA 262075
#define NO_PEAK 262076
#define PEAK_FRONT 262077
#define PEAK_AFTER 262078
#define GLOBAL_ERROR 262080
#define PEAK_WIDE 262081
#define LASER_OFF 262082

#define TIMEOUT 1000

int MR = 0;
const int bufferSize = 2048;
unsigned long startTime = 0;

optoNCDT::optoNCDT() {}

void optoNCDT::begin(uint32_t baud, uint16_t format)
{
  Serial1.begin(baud, format);
}

void optoNCDT::setMeasuringRange(int range)
{
  MR = range;
}

float optoNCDT::optoMeas()
{
  uint8_t lByte, mByte, hByte;
  Serial1.clear();
  // Read the L-Byte packet
  while (true)
  {
    if (Serial1.available() >= 1)
    {
      lByte = Serial1.read();
      if ((lByte >> 6) == 0)
      { // Check if the flag bits are correct
        break;
      }
    }
  }

  // Read the M-Byte packet
  while (true)
  {
    if (Serial1.available() >= 1)
    {
      mByte = Serial1.read();
      if ((mByte >> 6) == 1)
      { // Check if the flag bits are correct
        break;
      }
    }
  }

  // Read the H-Byte packet
  while (true)
  {
    if (Serial1.available() >= 1)
    {
      hByte = Serial1.read();
      if ((hByte >> 6) == 2)
      { // Check if the flag bits are correct
        break;
      }
    }
  }

  Serial1.clear();
  uint32_t x = ((hByte & 0x3F) << 12) | ((mByte & 0x3F) << 6) | (lByte & 0x3F);

  if (x >= 0 && x <= 230604)
  {
    return ((float(x) - 98232) / 65536) * MR;
  }
  else
  {
    return float(x);
  }
}

String optoNCDT::optoCmd(String cmd)
{
  startTime = millis();
  String output;
  Serial1.println(cmd);
  char buffer[bufferSize];

  int bufferIndex = 0;

  while (true)
  {
    // Loop until the "->" character pair is received
    // Check if there's data available to read
    if (Serial1.available())
    {
      // Read one byte from the serial port
      char receivedChar = Serial1.read();

      // Store the received byte in the buffer
      buffer[bufferIndex++] = receivedChar;

      // Check if the buffer contains the "->" character pair
      if (bufferIndex >= 2 && buffer[bufferIndex - 2] == '-' && buffer[bufferIndex - 1] == '>')
      {
        // Terminate the buffer with a null character to convert it to a string
        buffer[bufferIndex] = '\0';

        // Reset buffer index for next stream
        bufferIndex = 0;

        // Exit the loop once the "->" character pair is found
        break;
      }

      // Check if the buffer is full to prevent overflow
      if (bufferIndex >= bufferSize - 1)
      {
        // Terminate the buffer with a null character to convert it to a string
        buffer[bufferIndex] = '\0';
        break;
      }
    }

    if (millis() - startTime > TIMEOUT)
    {
      break;
    }
  }
  output.concat(buffer);
  Serial1.clear();
  Serial1.flush();
  return output;
}