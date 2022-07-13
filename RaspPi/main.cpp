#include <iostream>
#include <opencv2/opencv.hpp>
#include "utils/servo.hpp"
#include "utils/serial.hpp"
#include "utils/person.hpp"

// UNCOMMENT TO SEE FACE DETECTION
// #define FaceDetection

#ifdef FaceDetection
#include "MNN/MNN_UltraFace.hpp"
#else
#include "TensorFlow/MobileNetV1.hpp"
#endif

using namespace std;


int fast_distance(int x1, int y1, int x2, int y2)
{
    int dx = (x1-x2);
    int dy = (y1-y2);
    int dist = dx*dx + dy*dy;
    cout << "distance" << dist << endl;
    return dist;
}

void handle_person(std::vector<Person> &people, int centerX, int centerY)
{
    for (Person &person : people)
    {
        // < # is experimentally found
        if(fast_distance(centerX, centerY, person.x, person.y) < 1000)
        {
            //same_person
            person.increment_cnt(); //increment our confidence count
            person.update_pose(centerX, centerY); //update the persons location
            person.found = true; //store that we found the person in this frame
            return;
        }
    }
    //otherwise this center is a new person
    Person person(centerX, centerY);
    people.push_back(person);
}

int handle_people(std::vector<Person> &people)
{
    int num_people = 0;
    int i = 0;
    for (Person &person : people)
    {
        if(person.found)
        {
            person.found = false;
        }
        else
        {
            person.decerement_cnt();
        }
        if(person.cnt >= 10)
        {
            num_people++;
        }
        else if (person.cnt <= 0)
        {
            //delete the person
            people.erase(people.begin()+i);
            continue;
        }
        i++;
    }
    cout << "Number of people"<< num_people << endl;
    return num_people;
}

int main(int argc, char **argv)
{
    float f;
    float FPS[16];
    int i, angle, Fcnt, face_there, num_people= 0;
    std::vector<Person> people;
    cv::Mat frame;

    // some timing
    chrono::steady_clock::time_point Tbegin, Tend;

    for (i = 0; i < 16; i++)
        FPS[i] = 0.0;//    UltraPerson deepModel("./TensorFlow/detect.tflite", 300, 300, 4, 0.65);


        // Load Model
#ifdef FaceDetection
    UltraFace deepModel("./MNN/slim-320-quant-ADMM-50.mnn", 320, 240, 4, 0.65); // config model input
#else
    UltraPerson deepModel("./TensorFlow/detect.tflite", 300, 300, 4, 0.5);
#endif

    //Start the gpio pins with pigpio
    //Needs to be done before Servo or Serial can be used
    if (gpioInitialise() < 0)
    {
        return 1;
    }
    //gpioSetSignalFunc(SIGINT, stop);

    //Initialize a servo
    Servo servo(4, 0, 1000, 2000, 1500);
    //Initialize Serial
    Serial serial("/dev/ttyS0", 115200);

    cv::VideoCapture cap(-1);
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

        Tbegin = chrono::steady_clock::now();

#ifdef FaceDetection
        vector<FaceInfo> person_info;
        deepModel.detect(frame, person_info);
#else
        vector<PersonInfo> person_info;
        deepModel.detect(frame, person_info);
#endif

        Tend = chrono::steady_clock::now();

        int centerX,centerY;

        for (auto person : person_info)
        {
//            cv::Point pt1(person.x1, person.y1);
//            cv::Point pt2(person.x2, person.y2);
//            cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
            centerX = int((person.x1 + person.x2))>>1;//find center x, bit shift by 1 for divide by 2
            centerY = int(person.y1+person.y2)>>1;
            cv::Point center(centerX,centerY);
            cv::circle(frame,center,10,(0,0,255),-1); //(image,(x,y),radius,col,thickness)
            handle_person(people, centerX, centerY);
        }
        num_people = handle_people(people);


//         //experimentally x seemed to go from 40 - 600
//         //so mapping them to an angle of 30 -> 200 degrees:
//        if (j > 1)
//        {
//            face_there--;
//            if (face_there < 0)
//            {
//                face_there = 0;
//            }
//        }
//        else
//        {
//            face_there++;
//            if (face_there > 100)
//            {
//                angle = int(float((560 - centerX)) * (170.0 / 560.0));
//                servo.moveToAngle(angle);
//                if (face_there > 200)
//                {
//                    face_there = 200;
//                }
//            }
//        }

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

        //Check for serial
        if(serial.available()){
            serial.read();
        }
    }

    servo.kill();
    cout << "Closing the camera" << endl;
    cv::destroyAllWindows();
    cout << "Bye!" << endl;
    return 0;
}
