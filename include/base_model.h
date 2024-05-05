//
// Created by lrh on 2024/4/20.
//

#ifndef DEPLOY_BASE_ENGINE_H
#define DEPLOY_BASE_ENGINE_H

#include <iostream>
#include <vector>

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


class BaseModel {
public:
    BaseModel() = default;

    virtual ~BaseModel() { std::cout << "destroy BaseModel done" << std::endl; };

    virtual void infer(const std::vector<float> &input, std::vector<float> &output) = 0;

    virtual void process_output(const std::vector<float> &output) = 0;
};

#endif //DEPLOY_BASE_ENGINE_H
