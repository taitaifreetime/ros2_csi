import os

from ament_index_python.packages import get_package_share_path
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    this_pkg = get_package_share_path('ros2_csi')
    camera_info_file = os.path.join(this_pkg, 'config/csi1_info.yaml')

    return LaunchDescription([
        Node(
            package='ros2_csi',
            executable='csi_camera_node',
            output='screen',
            namespace='camera',
            name='csi1', 
            parameters=[
                os.path.join(this_pkg, 'config/csi1.yaml'), 
                {
                    'camera_info_file': "file://"+camera_info_file
                }
            ],
            emulate_tty=True,
        ),
    ])