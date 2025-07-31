# CSI camera node for ROS 2
This was tested on Jetson Orin Nano 8GB, ROS 2 Humble.

## Requirements
```
sudo apt install ros-humble-image-transport ros-humble-image-transport-plugins ros-humble-camera-info-manager
```

## Camera Configuration
```
// enable IMX219 and reboot
sudo /opt/nvidia/jetson-io/jetson-io.py 

// check camera device
v4l2-ctl --list-devices

// open camera stream
nvgstcapture-1.0 --sensor-id=0
```

## Usage
```
ros2 launch ros2_csi csi0_camera.launch.py
```
```
ros2 run ros2_csi csi_camera_node --ros-args -r __ns:=/camera -r __node:=csi0 -p cam_id:=0 -p width:=1280 -p height:=720 -p frequency:=60 -p flip_method:=0 -p camera_info_file:=file://file_path
```
- ```__node``` should match camera_name in camera info file.

## Reference
- [JetsonHacksNano/CSI-Camera](https://github.com/JetsonHacksNano/CSI-Camera)
    - gstreamer_pipeline function in CSI-Camera/simple_camera.cpp is referenced and reused as getGstreamerPipeline in this repository.