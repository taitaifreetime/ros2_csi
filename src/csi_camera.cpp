#include "ros2_csi/csi_camera.hpp"
#include <ros2_cpp_utils/utils.hpp>

CSICamera::CSICamera(rclcpp::NodeOptions options) : Node("csi_img_publisher", options)
{
    cam_id_ = ros2_cpp_utils::utils::getRosParam<int>(this, "cam_id", 0);
    width_ = ros2_cpp_utils::utils::getRosParam<int>(this, "width", 1280);
    height_ = ros2_cpp_utils::utils::getRosParam<int>(this, "height", 720);
    freq_ = ros2_cpp_utils::utils::getRosParam<int>(this, "frequency", 60);
    flip_method_ = ros2_cpp_utils::utils::getRosParam<int>(this, "flip_method", 0);
    std::string camera_info_file = ros2_cpp_utils::utils::getRosParam<std::string>(this, "camera_info_file", "camera_info.yaml");

    // camera stream configuration
    std::string pipeline = getGstreamerPipeline(cam_id_, width_, height_, freq_, flip_method_);
    cap_.open(pipeline, cv::CAP_GSTREAMER);
    
    // get node name and namespace
    std::string node_namespace = this->get_namespace();
    node_name_ = this->get_name();
    std::string ns_name = node_namespace + (node_namespace == "/" ? node_name_ : std::string("/") + node_name_);

    // publishers
    img_pub_ = create_publisher<sensor_msgs::msg::Image>(
        ns_name + "/image_raw", 
        rclcpp::QoS(rclcpp::SensorDataQoS())
    );
    compimg_pub_ = create_publisher<sensor_msgs::msg::CompressedImage>(
        ns_name + "/image_raw/compressed", // "/image_compressed" does not work.
        10
    );
    camera_info_pub_ = create_publisher<sensor_msgs::msg::CameraInfo>(
        ns_name + "/camera_info", 
        rclcpp::QoS(1).transient_local()
    );

    // camera info manager
    camera_info_manager_ = std::make_shared<camera_info_manager::CameraInfoManager>(
        this, 
        node_name_, // this should match camera_name in calibration file
        camera_info_file // claibration parameter file path started with file://
    );
    camera_info_msg_ = camera_info_manager_->getCameraInfo();
    stamp_ = this->get_clock()->now();
    camera_info_msg_.header.frame_id = node_name_;
    camera_info_msg_.header.stamp.sec = stamp_.seconds();
    camera_info_msg_.header.stamp.nanosec = stamp_.nanoseconds();
    camera_info_pub_->publish(camera_info_msg_);

    auto logger = this->get_logger();
    RCLCPP_INFO(logger, " ");
    RCLCPP_INFO(logger, "cam_id: %d", cam_id_);
    RCLCPP_INFO(logger, "width: %d", width_);
    RCLCPP_INFO(logger, "height: %d", height_);
    RCLCPP_INFO(logger, "frequency: %d", freq_);
    RCLCPP_INFO(logger, "flip_method: %d", flip_method_);
    RCLCPP_INFO(logger, "camera_info_file: %s", camera_info_file.c_str());
    RCLCPP_INFO(logger, "Published Topic");
    RCLCPP_INFO(logger, "  %s", img_pub_->get_topic_name());
    RCLCPP_INFO(logger, "  %s", compimg_pub_->get_topic_name());
    RCLCPP_INFO(logger, "  %s", camera_info_pub_->get_topic_name());
    
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(1000/freq_),
        std::bind(&CSICamera::publishImage, this)
    );
}

CSICamera::~CSICamera(){}

std::string CSICamera::getGstreamerPipeline(const int cam_id, const int width, const int height, const int freq, const int flip_method)
{
    return "nvarguscamerasrc sensor-id=" + std::to_string(cam_id) + " ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(width) + ", height=(int)" +
           std::to_string(height) + ", framerate=(fraction)" + std::to_string(freq) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(width) + ", height=(int)" +
           std::to_string(height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

void CSICamera::publishImage()
{
    cv::Mat img;

    // RCLCPP_INFO(this->get_logger(), "%lf", (this->get_clock()->now()).nanoseconds() / 1000000000.0-stamp_.nanoseconds() / 1000000000.0);
    stamp_ = this->get_clock()->now();
    if (!cap_.read(img)) RCLCPP_ERROR(this->get_logger(), "Capture read error");
    else
    {
        cv_bridge::CvImage cv_img;
        cv_img.encoding = "bgr8";
        cv_img.image = img;
        cv_img.header.stamp.sec = stamp_.seconds();
        cv_img.header.stamp.nanosec = stamp_.nanoseconds();
        cv_img.header.frame_id = node_name_;

        sensor_msgs::msg::Image img_msg;
        cv_img.toImageMsg(img_msg);
        img_pub_->publish(std::move(img_msg));

        sensor_msgs::msg::CompressedImage compimg_msg;
        cv_img.toCompressedImageMsg(compimg_msg);
        compimg_pub_->publish(std::move(compimg_msg));
    }

    return;
}