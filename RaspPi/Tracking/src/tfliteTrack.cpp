#include "tfliteTrack.hpp"

using namespace cv;
using namespace std;

UltraPerson::UltraPerson(const std::string &yolo_path, int input_width = 300, int input_height = 300, int num_thread_ = 4, float score_threshold_ = 0.5)
{
    width = input_width;
    height = input_height;
    score_threshold = score_threshold_;
    num_thread = num_thread_;

    // Load model
    model = tflite::FlatBufferModel::BuildFromFile(yolo_path.c_str());
    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model.get(), resolver)(&interpreter);

    interpreter->AllocateTensors();

    //     Get the names
    bool result = getFileContent("./TensorFlow/COCO_labels.txt");
    if (!result)
    {
        cout << "loading labels failed";
        exit(-1);
    }
}

bool UltraPerson::getFileContent(std::string fileName)
{
    // Open the File
    std::ifstream in(fileName.c_str());
    // Check if object is valid
    if (!in.is_open())
        return false;

    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(in, str))
    {
        // Line contains string of length > 0 then save it in vector
        if (str.size() > 0)
            Labels.push_back(str);
    }
    // Close The File
    in.close();
    return true;
}

int UltraPerson::detect(Mat &src, std::vector<PersonInfo> &personList)
{
    Mat image;
    int cam_width = src.cols;
    int cam_height = src.rows;

    // copy image to input as input tensor
    cv::resize(src, image, Size(width, height));
    memcpy(interpreter->typed_input_tensor<uchar>(0), image.data, image.total() * image.elemSize());

    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(num_thread); // quad core

    interpreter->Invoke(); // run your model

    const float *detection_locations = interpreter->tensor(interpreter->outputs()[0])->data.f;
    const float *detection_classes = interpreter->tensor(interpreter->outputs()[1])->data.f;
    const float *detection_scores = interpreter->tensor(interpreter->outputs()[2])->data.f;
    const int num_detections = *interpreter->tensor(interpreter->outputs()[3])->data.f;

    std::vector<Object> objects;

    for (int i = 0; i < num_detections; i++)
    {
        // FIXME: just trying out:
        int det_index = (int)detection_classes[i] + 1;
        if (det_index == 1) // check that it's a person, that is all we care about
        {
            if (detection_scores[i] > score_threshold)
            {
                Object Obj;
                Obj.label = det_index;
                Obj.prob = detection_scores[i];
                float y1 = detection_locations[4 * i] * cam_height;
                float x1 = detection_locations[4 * i + 1] * cam_width;
                float y2 = detection_locations[4 * i + 2] * cam_height;
                float x2 = detection_locations[4 * i + 3] * cam_width;

                Obj.rect.x = (int)x1;
                Obj.rect.y = (int)y1;
                Obj.rect.width = (int)(x2 - x1);
                Obj.rect.height = (int)(y2 - y1);

                objects.push_back(Obj);
            }
        }
    }

    vector<STrack> output_stracks = track.update(objects);

    for (size_t i = 0; i < output_stracks.size(); i++)
    {
        vector<float> tlwh = output_stracks[i].tlwh;
        bool vertical = tlwh[2] / tlwh[3] > 1.6;
        if (tlwh[2] * tlwh[3] > 20 && !vertical)
        {
            PersonInfo person;
            // FIXME: Does this effect it?
            //  Scalar s = track.get_color(output_stracks[i].track_id);
            

            person.x1 = tlwh[0];
            person.y1 = tlwh[1];
            person.x2 = tlwh[2];
            person.xy = tlwh[3];
            person.label = output_strack[i].track_id;

            // create person info
            personList.push_back(person);
        }
    }

    // create person vector for return
}