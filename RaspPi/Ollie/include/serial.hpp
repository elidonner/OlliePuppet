#ifndef SERIAL_HPP_INCLUDED
#define SERIAL_HPP_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>

class Serial
{

public:

    /**
    * Constructor
    */
    Serial(char* serialName, int baudRate);

    /**
    * check if serial port is available
    */
    int available();

    /**
    * write given msg
    */
    void write(char* msg);

    /**
     * read msg from serial port
     */
    int read();


protected:

private:
    void showNewData();
private:
    const uint8_t numBytes = 32;
    uint8_t receivedBytes[32];
    uint8_t numReceived = 0;
    int serial_port;
};


#endif // SERIAL_HPP_INCLUDED
