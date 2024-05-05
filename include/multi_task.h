//
// Created by lrh on 2024/4/20.
//

#ifndef DEPLOY_MULTI_TASK_H
#define DEPLOY_MULTI_TASK_H

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

#include "trt_model.h"
#include "track.h"
#include "serial_parser.h"
#include "utils.h"

using namespace std;

class MultiTask {
public:
    MultiTask(const string &config_file, const string &port, int baud_rate, vector<int> &num_classes,
              vector<BaseModel *> &models);

    virtual ~MultiTask();

    void run();

private:
    const string config_file;
    const string port;
    const int baud_rate;
    const vector<int> num_classes;
    const int window_size = 30;
    const int step_size = 1;
    SerialParser *parser;
    vector<BaseModel *> &models;
    deque<Frame> frames;
    TrackModel *track_model;

    // multi-thread
    vector<deque<vector<float>>>
            input_queues;
    vector<deque<vector<float>>>
            output_queues;
    mutex mtx_frames, mtx_infers, mtx_res;
    thread t_pre, t_track;
    vector<thread> t_infer;
    vector<thread> t_res;
    condition_variable cv_pre, cv_track;
    vector<condition_variable *> cv_infers, cv_res;
    bool track_ready = false;

    void prepare_input();

    void infer(int id);

    void track();

    void res(int id);
};

#endif //DEPLOY_MULTI_TASK_H
