//
// Created by lrh on 2024/4/20.
//

#include "multi_task.h"

MultiTask::MultiTask(const string &config_file, const string &port, int baud_rate, vector<int> &num_classes,
                     vector<BaseModel *> &models)
        : config_file(config_file), port(port), baud_rate(baud_rate), num_classes(num_classes), models(models) {
    // create track model
    track_model = new TrackModel("track", 0.5, 5);
    // create serial parser
    parser = new SerialParser(config_file.c_str(), port.c_str(), baud_rate);

    // for multi-thread
    const size_t num_tasks = models.size();
    input_queues.resize(num_tasks);
    output_queues.resize(num_tasks);
    cv_infers.resize(num_tasks);
    cv_res.resize(num_tasks);
    t_infer.resize(num_tasks);
    t_res.resize(num_tasks);
    // create condition variables
    for (int i = 0; i < num_tasks; i++) {
        cv_infers[i] = new condition_variable();
        cv_res[i] = new condition_variable();
    }
    // create threads
    t_pre = thread(&MultiTask::prepare_input, this);
    t_track = thread(&MultiTask::track, this);
    for (int i = 0; i < num_tasks; i++) {
        t_infer[i] = thread(&MultiTask::infer, this, i);
        t_res[i] = thread(&MultiTask::res, this, i);
    }
    cout << "multi-task init done" << endl;
}

MultiTask::~MultiTask() {
    // destroy parser
    delete parser;
    // destroy models
    for (auto &model: models) {
        delete model;
    }
    delete track_model;
    // notify all threads
    for (auto &cv: cv_infers) {
        cv->notify_all();
    }
    for (auto &cv: cv_res) {
        cv->notify_all();
    }
    // join threads
    if (t_pre.joinable())
        t_pre.join();
    if (t_track.joinable())
        t_track.join();
    for (int i = 0; i < t_infer.size(); i++) {
        if (t_infer[i].joinable())
            t_infer[i].join();
        if (t_res[i].joinable())
            t_res[i].join();
    }
    // destroy condition variables
    for (auto &cv: cv_infers) {
        delete cv;
    }
    for (auto &cv: cv_res) {
        delete cv;
    }
}

void MultiTask::run() {
    while (true) {
        // get frame
        Frame frame = parser->parse();
        if (frame.number_of_points == 0) {
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }
        // track
        track_ready = true;
        cv_track.notify_one();
        // push frame
        unique_lock<mutex> lck(mtx_frames);
        frames.emplace_back(frame);
        lck.unlock();
        cv_pre.notify_one();
        // sleep
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void MultiTask::prepare_input() {
    while (true) {
        unique_lock<mutex> lck(mtx_frames);
        cv_pre.wait(lck, [this] { return frames.size() >= window_size; });
        // get frame and pre-process
        vector<vector<float>> data(window_size, vector<float>(window_size, 0.0));
        for (int i = 0; i < window_size; ++i) {
            Frame &frame = frames.at(i);
            pre_process(frame.points, data, i);
        }
        // push input
        for (int i = 0; i < input_queues.size(); i++) {
            unique_lock<mutex> lck_infer(mtx_infers);
            input_queues[i].push_back(flatten2d(data));
            lck_infer.unlock();
            cv_infers[i]->notify_one();
        }
        // pop frame
        for (int i = 0; i < step_size; ++i) {
            frames.pop_front();
        }
        lck.unlock();
        // sleep
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void MultiTask::infer(int id) {
    while (true) {
        unique_lock<mutex> lck(mtx_infers);
        cv_infers[id]->wait(lck, [&id, this] { return !input_queues[id].empty(); });
        // get input
        std::cout << "input_queues: " << input_queues[id].size() << std::endl;
        vector<float> input(input_queues[id].front());
        input_queues[id].pop_front();
        lck.unlock();
        // inference
        vector<float> output(num_classes[id], 0.0);
        models[id]->infer(input, output);
        // push output
        unique_lock<mutex> lck_res(mtx_res);
        output_queues[id].push_back(output);
        lck_res.unlock();
        cv_res[id]->notify_one();
        // sleep
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void MultiTask::track() {
    unique_lock<mutex> lck(mtx_frames);
    while (!track_ready) {
        cv_track.wait(lck);
    }
    track_ready = false;
    track_model->infer(frames.back());
}

void MultiTask::res(int id) {
    while (true) {
        unique_lock<mutex> lck(mtx_res);
        cv_res[id]->wait(lck, [&id, this] { return !output_queues[id].empty(); });
        // get output
        vector<float> output(output_queues[id].front());
        output_queues[id].pop_front();
        lck.unlock();
        // process output
        models[id]->process_output(output);
        // sleep
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}
