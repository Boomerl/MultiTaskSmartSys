//
// Created by lrh on 2024/4/20.
//

#include "serial_parser.h"

static int numRxAnt = 4;
static int numTxAnt = 3;

SerialParser::SerialParser(const char *config_file, const char *port, int baud_rate) {
    // parse config file
    if (!parse_config(config_file)) {
        std::cerr << "parse config file failed" << std::endl;
        exit(-1);
    }
    // open serial port
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "open serial port failed" << std::endl;
        exit(-1);
    }
    // set serial port
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, baud_rate);
    cfsetospeed(&options, baud_rate);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 1;
    tcsetattr(fd, TCSANOW, &options);
    // create thread
    t_read = std::thread(&SerialParser::read_data, this);
}

SerialParser::~SerialParser() {
    // close serial port
    close(fd);
    // join thread
    if (t_read.joinable())
        t_read.join();
}

bool SerialParser::parse_config(const char *config_file) {
    std::ifstream ifs(config_file);
    if (!ifs.is_open()) {
        std::cerr << "open config file failed" << std::endl;
        return false;
    }
    //
    int start_freq, idle_time, chirp_startidx, chirp_endidx, num_loops;
    float ramp_endtime;
    std::string line;
    while (std::getline(ifs, line)) {
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.size() > 0 && tokens[0] == "profileCfg") {
            start_freq = std::stoi(tokens[2]);
            idle_time = std::stoi(tokens[3]);
            ramp_endtime = std::stof(tokens[5]);
        } else if (tokens.size() > 0 && tokens[0] == "frameCfg") {
            chirp_startidx = std::stoi(tokens[1]);
            chirp_endidx = std::stoi(tokens[2]);
            num_loops = std::stoi(tokens[3]);
        }
    }
    // compute parameters
    int num_chirps_perframe = (chirp_endidx - chirp_startidx + 1) * num_loops;
    config_params.numDopplerBins = num_chirps_perframe / numTxAnt;
    config_params.dopplerResolutionMps = (float) (3e8 / (2 * start_freq * 1e9 * (idle_time + ramp_endtime) * 1e-6 *
                                                         num_chirps_perframe));

    return true;
}

Frame SerialParser::parse() {
    std::lock_guard<std::mutex> lock(mtx_frame);
    if (frames.empty()) {
        return Frame(-1, 0);
    }
    Frame f = frames.front();
    frames.pop_front();
    return f;
}

void SerialParser::read_data() {
    while (true) {
        int len = read(fd, buf, sizeof(buf));
        if (len <= 0) continue;
//        std::cout << "read data length: " << len << std::endl;
        // parse serial data
        Frame f;
        int num_points = parse_data(buf, len, f, config_params.numDopplerBins, config_params.dopplerResolutionMps);
        if (num_points > 0) {
//            std::cout << "frame_number: " << f.frame_number << ", points: " << f.number_of_points << std::endl;
            std::lock_guard<std::mutex> lock(mtx_frame);
            frames.push_back(f);
        }
        // clear
        memset(buf, 0, sizeof(buf));
        // sleep
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}