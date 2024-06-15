//
// Created by lrh on 2024/5/3.
//

#ifndef DEMO_SHOW_TRT_MODEL_H
#define DEMO_SHOW_TRT_MODEL_H

#include <string>
#include <fstream>
#include <algorithm>
#include <NvInfer.h>
#include <NvOnnxParser.h>
#include "logger.h"
#include "base_model.h"

using namespace std;
using namespace nvinfer1;

class TRTModel : public BaseModel {
public:
    TRTModel(string task, string model_path, string in_name, string out_name, int batch_size,
             int in_channel, int in_height, int in_width, int num_classes, float threshold);

    ~TRTModel() override;

    void infer(const vector<float> &input, vector<float> &output) override;

    void process_output(const vector<float> &output) override;

private:
    const string task;
    const string model_path;    // model path
    const string in_name;    // input name
    const string out_name;  // output
    const int BATCH_SIZE; // batch size
    const int IN_CHANNEL; // input channel
    const int IN_HEIGHT; // input height
    const int IN_WIDTH; // input width
    const int NUM_CLASSES; // number of classes
    const float threshold; // threshold

    cudaStream_t stream;
    IRuntime *runtime; // runtime
    ICudaEngine *engine; // engines
    IExecutionContext *context; // execution context
    void *buffers[2];   // input and output device buffers
    int inputIndex;   // input index
    int outputIndex;  // output index
};

#endif //DEMO_SHOW_TRT_MODEL_H
