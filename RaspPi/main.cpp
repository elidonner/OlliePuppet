#include <iostream>
#include <opencv2/opencv.hpp>
#include "utils/servo.hpp"
#include "utils/serial.hpp"

// UNCOMMENT TO SEE FACE DETECTION
 #define FaceDetection

#ifdef FaceDetection
#include "MNN_UltraFace.hpp"
#else
#include "MobileNetV1.hpp"
#endif

using namespace std;

#ifdef FaceDetection
int calculate_num_faces(vector<FaceInfo> person_info)
#else
int calculate_num_faces(vector<PersonInfo> person_info)
#endif
{
    // initialize length
    int length = 0;

    for (auto person : person_info)
    {
        length++;
    }

    return length;
}

int find_closest_face(int face_sizes[], int center_poses[], int length)
{
    int largest, i, pos;
    largest = face_sizes[0];
    for (i = 1; i < length; i++)
    {
        if (face_sizes[i] > largest)
        {
            largest = face_sizes[i];
            pos = i;
        }
    }

    return center_poses[pos];
}

void print_pos_and_size(int poses[], int mags[], int length)
{

    for (int i = 0; i < length; i++)
    {
        cout << "mags: " << i << ":    " << mags[i] << endl;
        cout << "center x position is: " << i << ":    " << poses[i] << endl;
    }
}

int main(int argc, char **argv)
{
    float f;
    float FPS[16];
    int i, angle, Fcnt = 0;
    cv::Mat frame;
    int face_there = 0;

    // some timing
    chrono::steady_clock::time_point Tbegin, Tend;

    for (i = 0; i < 16; i++)
        FPS[i] = 0.0;

        // Load Model
#ifdef FaceDetection
    UltraFace deepModel("./MNN/slim-320-quant-ADMM-50.mnn", 320, 240, 4, 0.65); // config model input
#else
    UltraPerson deepModel("./TensorFlow/detect.tflite", 300, 300, 4, 0.65);
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
        //      frame=cv::imread("27.jpg");  //if you want to run just one picture need to refresh frame before class detection
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

//         int length = calculate_num_faces(face_info);
//         int mag[length];
//         int centerX[length];
        int centerX;
        bool first_face = true;
        int j = 0;

        for (auto person : person_info)
        {
            cv::Point pt1(person.x1, person.y1);
            cv::Point pt2(person.x2, person.y2);
            cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
            if (first_face)
            {
                centerX = int((person.x1 + person.x2) / 2);
            }
            // first_face = false;
            // mag[j] = int(face.y1+face.y2);
            // centerX[j] = int((face.x1+face.x2)/2);
            j++;
            // centerY = int((face.y1 + face.y2)/2);
        }

//         //print positions
//         print_pos_and_size(centerX, mag, length);
//         cout<<"center y position is: "<<centerY<<endl;

//         //experimentally x seemed to go from 40 - 600
//         //so mapping them to an angle of 30 -> 200 degrees:
//         int center = find_closest_face(mag, centerX, length);
        if (j > 1)
        {
            face_there--;
            if (face_there < 0)
            {
                face_there = 0;
            }
        }
        else
        {
            face_there++;
            if (face_there > 100)
            {
                angle = int(float((560 - centerX)) * (170.0 / 560.0));
                servo.moveToAngle(angle);
                if (face_there > 200)
                {
                    face_there = 200;
                }
            }
        }

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
