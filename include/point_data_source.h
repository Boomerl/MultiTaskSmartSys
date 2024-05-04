//
// Created by 25082 on 2024/5/4.
//

#ifndef DEMO_SHOW_POINT_DATA_SOURCE_H
#define DEMO_SHOW_POINT_DATA_SOURCE_H

#include <vector>
#include "point.h"

namespace dbscan {
    template<typename T>
    class PointDataSource {
    public:
        PointDataSource(Point<T> *ptr, size_t count) : m_ptr(ptr), m_count(count) {}

        PointDataSource(std::vector<Point<T>> &point_cloud) : m_ptr(&point_cloud[0]), m_count(point_cloud.size()) {}

        // 默认构造函数
        PointDataSource() : m_ptr(nullptr), m_count(0) {}

        ~PointDataSource() {}

        PointDataSource &operator=(const PointDataSource &other) = default;

        size_t size() { return m_count; }

        Point<T> &operator[](size_t index) { return m_ptr[index]; }

        const Point<T> *begin() { return m_ptr; }

        const Point<T> *end() { return m_ptr + m_count; }

    private:
        Point<T> *m_ptr;
        size_t m_count;
    };
}

#endif //DEMO_SHOW_POINT_DATA_SOURCE_H
