//
// Created by lrh on 2024/5/3.
//

#include "trt_model.h"

TRTModel::TRTModel(string task, string model_path, string in_name, string out_name,
                   int batch_size, int in_channel, int in_height, int in_width, int num_classes)
        : task(task), model_path(model_path), in_name(in_name), out_name(out_name), BATCH_SIZE(batch_size),
          IN_CHANNEL(in_channel), IN_HEIGHT(in_height), IN_WIDTH(in_width), NUM_CLASSES(num_classes) {
    // create logger
    Logger gLogger;
    // create runtime
    runtime = createInferRuntime(gLogger);
    assert(runtime != nullptr);
    // read engines file
    ifstream engine_file(this->model_path.c_str(), ios::in | ios::binary);
    engine_file.seekg(0, ios::end);
    const size_t file_size = engine_file.tellg();
    engine_file.seekg(0, ios::beg);
    vector<char> engine_data(file_size);
    engine_file.read(engine_data.data(), file_size);
    // create engines
    engine = runtime->deserializeCudaEngine(engine_data.data(), file_size);
    // create execution context
    context = engine->createExecutionContext();
    assert(context != nullptr);
    context->setBindingDimensions(0, Dims4{BATCH_SIZE, IN_CHANNEL, IN_HEIGHT, IN_WIDTH});

    // In order to bind the buffers, we need to know the names of the input and output tensors.
    // Note that indices are guaranteed to be less than IEngine::getNbBindings()
    inputIndex = engine->getBindingIndex(this->in_name.c_str());
    assert(engine.getBindingDataType(inputIndex) == nvinfer1::DataType::kFLOAT);
    outputIndex = engine->getBindingIndex(this->out_name.c_str());
    assert(engine.getBindingDataType(outputIndex) == nvinfer1::DataType::kFLOAT);

    // create GPU buffers and stream
    CHECK(cudaStreamCreate(&stream));
    CHECK(cudaMalloc(&buffers[inputIndex], BATCH_SIZE * IN_CHANNEL * IN_HEIGHT * IN_WIDTH * sizeof(float)));
    CHECK(cudaMalloc(&buffers[outputIndex], BATCH_SIZE * NUM_CLASSES * sizeof(float)));
}

TRTModel::~TRTModel() {
    this->context->destroy();
    this->engine->destroy();
    this->runtime->destroy();

    std::cout << "free device buffers" << std::endl;
    CHECK(cudaFree(buffers[inputIndex]));
    CHECK(cudaFree(buffers[outputIndex]));
    CHECK(cudaStreamDestroy(stream));

    std::cout << "destroy TRTModel done" << std::endl;
}

void TRTModel::infer(const vector<float> &input, vector<float> &output) {
    // Check input and output size
    assert(input.size() == BATCH_SIZE * IN_CHANNEL * IN_HEIGHT * IN_WIDTH), "input size error";
    assert(output.size() == BATCH_SIZE * NUM_CLASSES), "output size error";

    CHECK(cudaMemcpyAsync(buffers[inputIndex], input.data(), input.size() * 4, cudaMemcpyHostToDevice, stream));
    context->enqueueV2(buffers, stream, nullptr);
    CHECK(cudaMemcpyAsync(output.data(), buffers[outputIndex], output.size() * 4, cudaMemcpyDeviceToHost, stream));
    cudaStreamSynchronize(stream);
}

void TRTModel::process_output(const vector<float> &output) {
    // post-process and print result
    auto max_iter = max_element(output.begin(), output.end());
    int idx = distance(output.begin(), max_iter);
    if (*max_iter >= threshold) {
        std::cout << task << ", class: " << idx << ", score: " << *max_iter << std::endl;
    }
}