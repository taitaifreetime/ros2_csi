from launch import LaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import IncludeLaunchDescription
from ament_index_python.packages import get_package_share_path
import os

def generate_launch_description():
    this_pkg = get_package_share_path('ros2_csi')

    csi0_launch = os.path.join(this_pkg, 'launch/csi0_camera.launch.py')
    csi1_launch = os.path.join(this_pkg, 'launch/csi1_camera.launch.py')

    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(csi0_launch)
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(csi1_launch)
        )
    ])
