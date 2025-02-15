# CSI camera node for ROS 2
This was tested on Jetson Orin Nano 8GB, ROS 2 Humble.

## Usage
```
ros2 run ros2_csi csi_camera_node --ros-args -r __ns:=/camera -r __node:=csi1 -p width:=1920 -p height:=1080 -p frequency:=30 -p flip_method:=0
```

## Reference
- [JetsonHacksNano/CSI-Camera](https://github.com/JetsonHacksNano/CSI-Camera)
    - gstreamer_pipeline function in CSI-Camera/simple_camera.cpp is referenced and reused as getGstreamerPipeline in this repository.