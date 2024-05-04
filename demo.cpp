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
    vector<baseEngine *> models(2);
    baseEngine *model1 = new TRTModel("gesture", paths[0], "input", "output", 1, 1, 30, 30, 5);
    baseEngine *model2 = new TRTModel("action", paths[0], "input", "output", 1, 1, 30, 30, 4);
    models[0] = model1;
    models[1] = model2;
    //
    vector<vector<int>> shapes = {{1, 1, 30, 30},
                                  {1, 1, 30, 30}};  // (N, C, H, W)
    vector<int> classes = {5, 4};
    float action_thresh = 0.90;
    float gesture_thresh = 0.10;

    // create multi-task object
    MultiTask task(config, port, baud_rate, classes, models);
    cout << "===== Start run multi-task =====" << endl;
    task.run(); // run multi-task
    cout << "===== End run multi-task =====" << endl;

    return 0;
}
