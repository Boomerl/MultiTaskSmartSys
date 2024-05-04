#ifndef DEPLOY_UTILS_H
#define DEPLOY_UTILS_H

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "point.h"

struct ConfigParameters {
    int numDopplerBins;
    float dopplerResolutionMps;
};

struct Frame {
    int frame_number;
    int number_of_points;
    std::vector<dbscan::Point<float>> points;

    explicit Frame(int f = -1, int n = 0) : frame_number(f), number_of_points(n) {}
};

int parse_data(uint8_t *buffer, int size, Frame &frame, const int &num_dopplerbins, const float &doppler_resmps);

void pre_process(const std::vector<dbscan::Point<float>> &points, std::vector<std::vector<float>> &data, int frame_idx);

std::vector<std::string> split(const std::string &s, char delim);

template<typename T>
std::vector<T> flatten2d(const std::vector<std::vector<T>> &data) {
    std::vector<T> res;
    for (const auto &row: data) {
        res.insert(res.end(), row.begin(), row.end());
    }
    return res;
}

#endif // DEPLOY_UTILS_H