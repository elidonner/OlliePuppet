#include <iostream>
#include "utils/serial.hpp"


using namespace std;


int main(int argc, char **argv)
{

    //Start the gpio pins with pigpio
    //Needs to be done before Servo or Serial can be used
    if (gpioInitialise() < 0)
    {
        return 1;
    }
    //gpioSetSignalFunc(SIGINT, stop);

    //Initialize Serial
    Serial serial("/dev/ttyS0", 115200);

    //Check for serial
    if(serial.available()){
        serial.read();
    }
    char[] msg = "message";
    serial.write(msg);
    return 0;
}
