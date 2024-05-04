//
// Created by lrh on 2024/4/20.
//

#ifndef DEPLOY_SERIAL_PARSER_H
#define DEPLOY_SERIAL_PARSER_H

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <deque>
#include <fstream>

#include "utils.h"

class SerialParser {
public:
    SerialParser(const char *config_file, const char *port, int baud_rate);

    virtual ~SerialParser();

    bool parse_config(const char *config_file);

    Frame parse();

private:
    int fd;
    std::thread t_read;
    ConfigParameters config_params;
    // data
    uint8_t buf[2048];
    std::mutex mtx_frame;
    std::deque<Frame> frames;

    void read_data();
};

#endif //DEPLOY_SERIAL_PARSER_H
