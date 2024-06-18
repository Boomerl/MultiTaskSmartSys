## 说明

本项目是一个基于毫米波雷达的手势识别和动作识别项目，主要包括以下几个模块：手势识别、动作识别、点云跟踪、串口数据解析、多任务处理等。

## 环境依赖

- cuda
- TensorRT
- 点云库 pcl

## 编译代码

```shell
# 在项目根目录下执行，可以写个shell脚本
mkdir build
cd build
cmake ..
make
```

## 运行代码

参考网站：https://blog.csdn.net/c417469898/article/details/117510172

```shell
# 授权串口读写权限
sudo chmod 777 /dev/ttyACM1
# 或者
sudo usermod -a -G dialout $USER
# 在build目录下执行
./demo
```

## 代码结构

```shell
.
├── convert.sh  # 用于将onnx模型转换为tensorrt模型的脚本
├── CMakeLists.txt  # cmake编译文件
├── README.md
├── demo.cpp    # 主程序
└── configs
    └── 8m.cfg  # 毫米波雷达配置文件
└── engines
    └── gesture.trt  # 手势识别tensorrt模型
    └── action.trt  # 动作识别tensorrt模型
└── include
    └── base_model.h
    └── serial_parser.h
    └── trt_model.h
    └── track.h
    └── multi_task.h
    └── logger.h
    └── logging.h
    └── utils.h
    └── point.h  # 点云数据结构
    └── point_data_source.h # 点云数据包装类
    └── dbscan.h # dbscan聚类算法
    ...
└── src
    └── trt_model.cpp   # 动作和手势识别模型
    └── track.cpp   # 跟踪模块
    └── serial_parser.cpp   # 串口解析数据模块
    └── logger.cpp   # tensorrt日志模块
    └── utils.cpp   # 工具函数模块
    └── multi_task.cpp   # 多任务模块(主要是多线程)
    ...
```
