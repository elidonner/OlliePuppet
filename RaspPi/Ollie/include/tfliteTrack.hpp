#ifndef tfliteTrack_hpp
#define tfliteTrack_hpp

#pragma once

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/core/ocl.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/model.h"
#include "BYTETracker.h"

using namespace cv;
using namespace std;

typedef struct PersonInfo
{
    float x1;
    float y1;
    float width;
    float height;
    int label;

} PersonInfo;

class UltraPerson
{
public:

    UltraPerson(BYTETracker &_tracker, const std::string &yolo_path, int input_width, int input_height, int num_thread_, float score_trheshold_);

    void detect(cv::Mat &src, std::vector<PersonInfo> &personList);


private:
    bool getFileContent(std::string fileName);

private:
    int width;
    int height;
    float score_threshold;
    int num_thread;
    std::vector<std::string> Labels;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::FlatBufferModel> model;
    BYTETracker &track;
};

#endif //tfliteTrack_hpp
