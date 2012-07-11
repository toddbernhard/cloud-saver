cloud-saver
===========

Converts a ROS PointCloud2 msg to a pcl::PointCloud&lt;PointT> and saves it as a .pcd

To use it:
1.  Change the POINTS_TOPIC after the includes to the name of your ROS topic
2.  Compile and run it
3.  At a terminal, use rostopic to publish a msg on the /saveNow topic
  e.g. `$> rostopic pub /saveNow std_msgs::String "filename-without-extension" -1`
4.  On my computer, it creates the file in ~/.ros/, but you may want to run `sudo find / -name *pcd`
   to figure out where it landed.

Note: you can pass an empty string ("") and it will save it to `test_pcd.pcd`