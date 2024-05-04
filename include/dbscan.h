//
// Created by lrh on 2024/5/4.
//

#ifndef DEMO_SHOW_DBSCAN_H
#define DEMO_SHOW_DBSCAN_H

#include "point.h"
#include "point_data_source.h"

namespace dbscan {
    static const int NOISE = -2;    // 定义噪声点的类别
    static const int NOT_CLASSIFIED = -1;   // 定义未分类点的类别

    template<typename T>
    class DBSCAN {
    public:
        DBSCAN() = default;

        DBSCAN(float neighbourhood, int minPts, std::vector<Point<T>> &point_cloud) : Neighbourhood(neighbourhood),
                                                                                      MinPts(minPts),
                                                                                      pointCloud(point_cloud) {}

        ~DBSCAN() {}

        std::vector<std::vector<size_t>> GetClusterPointSet() {
            std::vector<std::vector<size_t>> cluster;   // 存储最终聚类的结果
            std::vector<size_t> kernelObj;  // 存储核心对象的索引
            neighbourPoints.resize(pointCloud.size());
            neighbourDistance.resize(pointCloud.size());

            // 选择核心对象并找出它们的邻居
            SelectKernelAndNeighbour(kernelObj);

            // 迭代标记同一聚类的点
            int k = -1; // 初始化聚类数
            for (int i = 0; i < kernelObj.size(); ++i) {
                if (pointCloud[kernelObj[i]].cluster != NOT_CLASSIFIED) {
                    continue;
                }
                std::vector<T> queue;
                queue.push_back(kernelObj[i]);
                pointCloud[kernelObj[i]].cluster = ++k;
                while (!queue.empty()) {
                    size_t index = queue.back();
                    queue.pop_back();
                    if (neighbourPoints[index].size() > MinPts) {
                        for (int j = 0; j < neighbourPoints[index].size(); ++j) {
                            if (k == pointCloud[neighbourPoints[index][j]].cluster) {
                                continue;
                            }
                            queue.push_back(neighbourPoints[index][j]);
                            pointCloud[neighbourPoints[index][j]].cluster = k;
                        }
                    }
                }
            }

            // 将聚类结果存储到cluster变量中
            cluster.resize(k + 1);
            for (int i = 0; i < pointCloud.size(); ++i) {
                if (pointCloud[i].cluster != NOISE) {
                    cluster[pointCloud[i].cluster].push_back(i);
                }
            }
            return cluster;
        }

    private:
        float Neighbourhood;
        int MinPts;
        PointDataSource<T> pointCloud;  // 点云数据
        std::vector<std::vector<int>> neighbourPoints;    // 存储每个点的邻居点的索引
        std::vector<std::vector<T>> neighbourDistance;    // 存储每个点到邻居点的距离

        // 选择核心对象并找出它们的邻居的函数
        void SelectKernelAndNeighbour(std::vector<size_t> &kernelObj) {
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud <pcl::PointXYZ>);
            cloud->points.resize(pointCloud.size());
            for (size_t i = 0; i < pointCloud.size(); i++) {
                cloud->points[i].x = pointCloud[i].x;
                cloud->points[i].y = pointCloud[i].y;
                cloud->points[i].z = pointCloud[i].z;
            }

            pcl::KdTreeFLANN <pcl::PointXYZ> kdtree;
            kdtree.setInputCloud(cloud);

            for (size_t i = 0; i < pointCloud.size(); i++) {
                kdtree.radiusSearch(cloud->points[i], Neighbourhood, neighbourPoints[i], neighbourDistance[i]);
                if (neighbourPoints[i].size() >= MinPts) {
                    kernelObj.push_back(i);
                } else {
                    pointCloud[i].cluster = NOISE;
                }
            }
        }
    };


#endif //DEMO_SHOW_DBSCAN_H
