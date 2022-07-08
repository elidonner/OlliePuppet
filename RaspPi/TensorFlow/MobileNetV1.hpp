//  Created by Linzaer on 2019/11/15.
//  Copyright © 2019 Linzaer. All rights reserved.

#ifndef MobileNetV1_hpp
#define MobileNetV1_hpp

#pragma once

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <iostream>
#include <opencv2/core/ocl.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/model.h"
#include <cmath>

typedef struct PersonInfo
{
    float x1;
    float y1;
    float x2;
    float y2;
    float score;

} PersonInfo;

class UltraPerson
{
public:
    UltraPerson(const std::string &yolo_path, int input_width, int input_height, int num_thread_, float score_trheshold_);

    int detect(cv::Mat &img, std::vector<PersonInfo> &personList);

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
};

#endif /* MobileNetV1_hpp */
