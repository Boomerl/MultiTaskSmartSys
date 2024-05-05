//
// Created by lrh on 2024/4/21.
//

#include <iostream>
#include <string>
#include "multi_task.h"
#include "trt_model.h"

int main() {
    // set configs for multi-task
    string config = "/home/zzy/workspace/mmw/smart_home/configs/8m.cfg";
    string port = "/dev/ttyACM1";   // data port
    int baud_rate = B921600; // baud rate
    vector<string> paths = {"/home/zzy/workspace/mmw/smart_home/engines/gesture.trt",
                            "/home/zzy/workspace/mmw/smart_home/engines/action.trt"};
    // models
    vector<BaseModel *> models(2);
    BaseModel *model1 = new TRTModel("gesture", paths[0], "input", "output", 1, 1, 30, 30, 5, 0.8);
    BaseModel *model2 = new TRTModel("action", paths[0], "input", "output", 1, 1, 30, 30, 4, 0.8);
    models[0] = model1;
    models[1] = model2;
    vector<int> classes = {5, 4};
    // create multi-task object
    MultiTask task(config, port, baud_rate, classes, models);
    cout << "===== Start run multi-task =====" << endl;
    task.run(); // run multi-task
    cout << "===== End run multi-task =====" << endl;

    return 0;
}
