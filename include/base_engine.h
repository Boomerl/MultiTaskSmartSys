//
// Created by lrh on 2024/4/20.
//

#ifndef DEPLOY_BASE_ENGINE_H
#define DEPLOY_BASE_ENGINE_H

#include <iostream>
#include <vector>

using namespace std;

#define CHECK(status) \
    do\
    {\
        auto ret = (status);\
        if (ret != 0)\
        {\
            std::cerr << "Cuda failure: " << ret << std::endl;\
            abort();\
        }\
    } while (0)


class baseEngine {
public:
    baseEngine() = default;

    virtual ~baseEngine() { std::cout << "destroy baseEngine done" << std::endl; };

    virtual void infer(const vector<float> &input, vector<float> &output) = 0;

    virtual void process_output(const vector<float> &output) = 0;
};

#endif //DEPLOY_BASE_ENGINE_H
