#pragma once

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <camera_info_manager/camera_info_manager.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <memory>

class CSICamera : public rclcpp::Node
{


public:
    CSICamera(rclcpp::NodeOptions options);
    ~CSICamera();

private:
    std::string getGstreamerPipeline(const int width, const int height, const int freq, const int flip_method);
    void publishImage();
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr img_pub_;
    rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr compimg_pub_;
    std::shared_ptr<camera_info_manager::CameraInfoManager> camera_info_manager_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr caminfo_pub_;
    cv::VideoCapture cap_;

    int width_, height_;
    int freq_;
    int flip_method_;
    sensor_msgs::msg::CameraInfo camera_info_msg_;
    rclcpp::Time stamp_;
    std::string node_name_;
};