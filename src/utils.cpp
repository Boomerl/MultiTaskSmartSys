#include "utils.h"

// define magic word
static uint8_t magicWord[8] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

// mmwave
static std::vector<float> doppler_list = {-2.68, -2.34, -2.01, -1.67, -1.34, -1.0, -0.67, -0.33,
                                          0.0, 0.33, 0.67, 1.0, 1.34, 1.67, 2.01, 2.34};

static int bytesToInt(const uint8_t *bytes, int pos) {
    int value = bytes[pos] & 0xFF;
    value |= (bytes[pos + 1] << 8) & 0xFF00;
    value |= (bytes[pos + 2] << 16) & 0xFF0000;
    value |= (bytes[pos + 3] << 24) & 0xFF000000;
    return value;
}

static short bytesToShort(const uint8_t *bytes, int pos) {
    short value = bytes[pos] & 0xFF;
    value |= (bytes[pos + 1] << 8) & 0xFF00;
    return value;
}

int parse_data(uint8_t *buffer, int size, Frame &frame, const int &num_dopplerbins, const float &doppler_resmps) {
    if (size <= 16) {
        return -1;
    }

    int start_index = -1;
    for (int i = 0; i < size - 8; ++i) {
        if (memcmp(buffer + i, magicWord, 8) == 0) {
            start_index = i;
            break;
        }
    }
    if (start_index == -1) {
//        std::cout << "magic word not found" << std::endl;
        return -1;
    }
    int total_size = bytesToInt(buffer, start_index + 12);
    if (total_size > 0 && start_index + total_size > size) {
//        std::cout << "total size is too large" << std::endl;
        return -1;
    }

    // parse header
    int frame_number = bytesToInt(buffer, start_index + 20);
    int number_of_points = bytesToInt(buffer, start_index + 28);
    if (number_of_points == 0) return -1;

    int idx = 36;
    assert(bytesToInt(buffer, start_index + idx) == 1);
    idx += 8;
    int tlv_numobj = bytesToShort(buffer, start_index + idx);
    idx += 2;
    int tlv_xyzQFormat = (int) pow(2, bytesToShort(buffer, start_index + idx));
    idx += 2;
    if (tlv_numobj <= 0) return -1;

    // parse point cloud data
    frame.frame_number = frame_number;
    frame.number_of_points = number_of_points;
    for (int i = 0; i < tlv_numobj; ++i) {
        dbscan::Point<float> point;
        // doppler
        int doppler_idx = bytesToShort(buffer, start_index + idx + 2);
        if (doppler_idx > (num_dopplerbins / 2 - 1)) {
            doppler_idx -= 65535;
        }
        point.doppler = doppler_idx * doppler_resmps;
        point.intensity = bytesToShort(buffer, start_index + idx + 4);
        point.x = bytesToShort(buffer, start_index + idx + 6) / (float) tlv_xyzQFormat;
        point.y = bytesToShort(buffer, start_index + idx + 8) / (float) tlv_xyzQFormat;
        point.z = bytesToShort(buffer, start_index + idx + 10) / (float) tlv_xyzQFormat;
        frame.points.push_back(point);
        // next point
        idx += 12;
    }

    return tlv_numobj;
}

void
pre_process(const std::vector<dbscan::Point<float>> &points, std::vector<std::vector<float>> &data, int frame_idx) {
    // transform point cloud to matrix
    for (auto &point: points) {
        float v = round(point.doppler * 100) / 100;
        auto it = std::find(doppler_list.begin(), doppler_list.end(), v);
        if (it != doppler_list.end()) {
            int idx = std::distance(doppler_list.begin(), it);
            data[idx][frame_idx] = point.intensity;
        }
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}