#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "BYTETracker.h"
#include "tfliteTrack.hpp"
#include "interactions.hpp"
#include "servo.hpp"
#include "serial.hpp"
#include "person.hpp"


using namespace std;

#ifndef DEBUG
#define DEBUG
#endif


int main(int argc, char **argv)
{

#ifdef DEBUG
    // fps stuff
    float f;
    int Fcnt, i= 0;
    float FPS[16];

    // some timing
    chrono::steady_clock::time_point Tbegin, Tend;

    for (i = 0; i < 16; i++)
        FPS[i] = 0.0; //
#endif

    //frame object
    cv::Mat frame;

    // Start the gpio pins with pigpio
    // Needs to be done before Servo or Serial can be used
    if (gpioInitialise() < 0)
    {
        return 1;
    }
    // gpioSetSignalFunc(SIGINT, stop);

    // Initialize a servo
    Servo servo(4, 0, 1000, 2000, 1500);
    // Initialize Serial
//    Serial ser("/dev/ttyS0", 115200); //GPIO Serial
    Serial ser("/dev/ttyACM0", 115200); //USB Serial with Arduino Mega
    fflush(stdout);
    //interactions object to handle ollies interactions and timers
    Audio audio(ser);
    vector<Person> people;
    Interactions interactions(audio, servo);


    //load deep learning model with tracking model
    BYTETracker tracker(22, 30);
    UltraPerson deepModel(tracker, "detect.tflite", 300, 300, 4, 0.6);
//
//    cv::VideoCapture cap(-1);
    cv::VideoCapture cap("FWM2.mp4");
    if (!cap.isOpened())
    {
        cerr << "ERROR: Unable to open the camera" << endl;
        return 0;
    }
    cout << "Start grabbing, press ESC on Live window to terminate" << endl;

    while (1)
    {
        cap >> frame;
        if (frame.empty())
        {
            cerr << "ERROR: Unable to grab from the camera" << endl;
            break;
        }

#ifdef DEBUG
        Tbegin = chrono::steady_clock::now();
#endif

        vector<PersonInfo> person_info;
        //detect the people in the frame
        deepModel.detect(frame, person_info);
        //update the people vector, and the interaction case we are in from the people found in the frame
        interactions.update_people(person_info, people, frame);
        std::cout<<people.size()<<std::endl;
        if(people.size()>0)
        {
            interactions.track_w_servo(people.at(0));
        }


        // Check for serial to see if audio is done playing
//        if(audio.audio_playing)
//        {
//            if (ser.available()>1)
//            {
//                std::cout<<"serial available"<<std::endl;
//                if(ser.read())
//                {
//                    //if(serial.read()) says audio is done playing
//                    audio.audio_done();
//                }
//
//            }
//        }


#ifdef DEBUG
        Tend = chrono::steady_clock::now();

        // calculate frame rate
        f = chrono::duration_cast<chrono::milliseconds>(Tend - Tbegin).count();
        if (f > 0.0)
            FPS[((Fcnt++) & 0x0F)] = 1000.0 / f;
        for (f = 0.0, i = 0; i < 16; i++)
        {
            f += FPS[i];
        }

        cv::putText(frame, cv::format("FPS %0.2f", f / 16), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255));

        // show output
        cv::imshow("RPi 64 OS - 1,95 GHz - 2 Mb RAM", frame);
        char esc = cv::waitKey(5);
        if (esc == 27)
            break;
#endif
    }

    servo.kill();
    cout << "Closing the camera" << endl;
    cv::destroyAllWindows();
    cout << "Bye!" << endl;
    return 0;
}
