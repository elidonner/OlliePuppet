/*
    UART communication on Raspberry Pi using C (pigpio Library)
*/

#include "serial.hpp"
using namespace std;

Serial::Serial(char *serialName, int baudRate)
{
    if ((serial_port = serOpen(serialName, baudRate, 0)) < 0) /* open serial port */
    {
        cout << "faliure";
    }
}

int Serial::available()
{
    return serDataAvailable(serial_port);
}

void Serial::write(char *msg)
{
    serWrite(serial_port, msg, sizeof(msg));
}

void Serial::showNewData()
{
    for (uint8_t n = 0; n < numReceived; n++)
    {
        printf("%c", receivedBytes[n]);
    }
    printf("\n");
    fflush(stdout);
}

int Serial::read()
{
    bool recvInProgress = false;
    static uint8_t ndx = 0;
    uint8_t startMarker = 0x3C; // this is the start marker <
    uint8_t endMarker = 0x3E;   // this is the start marker >
    uint8_t rb;

    while (serDataAvailable(serial_port) > 0)
    {
        rb = serReadByte(serial_port);

        if (recvInProgress == true)
        {
            //if we havent reached the end marker
            if (rb != endMarker)
            {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes)
                {
                    ndx = numBytes - 1;
                }
            }
            //once we reach the end marker, show the data
            else
            {
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx; // save the number for use when printing
//                std::cout<<receivedBytes[0]<<std::endl;
                if(receivedBytes[0]=='1')
                {
                    return 1;
                }
                else if(receivedBytes[0]=='0')
                {
                    showNewData();
                }
                return 0;

            }
        }

        else if (rb == startMarker)
        {
            recvInProgress = true;
        }
    }
    return 0;
}
