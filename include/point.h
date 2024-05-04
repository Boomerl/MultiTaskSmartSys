//
// Created by 25082 on 2024/5/4.
//

#ifndef DEMO_SHOW_POINT_H
#define DEMO_SHOW_POINT_H

static const int NOT_CLASSIFIED = -1;

namespace dbscan {
    template<typename T>
    struct Point {
        T x, y, z, doppler;
        short intensity;

        // for dbscan
        int cluster = NOT_CLASSIFIED;

        Point() = default;

        Point(T x, T y, T z) : x(x), y(y), z(z) {}

        ~Point() {}

        Point operator-(const Point &other) {
            return Point(x - other.x, y - other.y, z - other.z);
        }

        Point operator+(const Point &other) {
            return Point(x + other.x, y + other.y, z + other.z);
        }

        T getSquareDistanceTo(const Point &other) {
            T dx = x - other.x;
            T dy = y - other.y;
            T dz = z - other.z;
            return dx * dx + dy * dy + dz * dz;
        }
    };
}

#endif //DEMO_SHOW_POINT_H
