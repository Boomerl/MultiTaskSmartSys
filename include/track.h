//
// Created by lrh on 2024/5/4.
//

#ifndef DEMO_SHOW_TRACK_H
#define DEMO_SHOW_TRACK_H

#include <string>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include "dbscan.h"
#include "base_model.h"
#include "utils.h"

class TrackModel {
public:
    TrackModel() = delete;

    TrackModel(std::string task, float eps, int minPts) : task_(task), eps_(eps), minPts_(minPts) {}

    ~TrackModel() {}

    void infer(Frame input);

//    void process_output(const std::vector<float> &output);

private:
    const std::string task_;
    float eps_ = 0.5; // neighborhood
    int minPts_ = 5; // min points
    // aggregation
    dbscan::DBSCAN<float> *dbscan;
};

#endif //DEMO_SHOW_TRACK_H
