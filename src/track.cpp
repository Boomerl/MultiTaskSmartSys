//
// Created by lrh on 2024/5/4.
//

#include "track.h"

void TrackModel::infer(Frame input) {
    dbscan = new dbscan::DBSCAN<float>(eps_, minPts_, input.points);
    std::vector<std::vector<size_t>> clusters = dbscan->GetClusterPointSet();
    // 找到clusters中元素最多的vector
    if (clusters.size() == 0) {
        std::cout << "No object detected" << std::endl;
        return;
    }
    int index = 0;
    for (int i = 0; i < clusters.size(); ++i) {
        if (clusters[i].size() > clusters[index].size()) {
            index = i;
        }
    }
    // 找到cluster[index]的中心点
    dbscan::Point<float> center;
    for (int i = 0; i < clusters[index].size(); ++i) {
        center.x += input.points[clusters[index][i]].x;
        center.y += input.points[clusters[index][i]].y;
        center.z += input.points[clusters[index][i]].z;
    }
    center.x /= clusters[index].size();
    center.y /= clusters[index].size();
    center.z /= clusters[index].size();
    // 找到离中心点最近的点
    float min_distance = 1e6;
    int min_index = 0;
    for (int i = 0; i < clusters[index].size(); ++i) {
        float d = center.getSquareDistanceTo(input.points[clusters[index][i]]);
        if (d < min_distance) {
            min_distance = d;
            min_index = i;
        }
    }
    // 找到最近点的坐标
    center = input.points[clusters[index][min_index]];
    std::cout << "Object detected at: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
}