/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <bernhard.todd@gmail.com> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return
 *  Todd Bernhard
 * ----------------------------------------------------------------------------
 */

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/PointCloud2.h"
#include <pcl/io/pcd_io.h>	 // for savePCDFileAscii
#include <pcl/point_types.h> // for PointT

/**
 * This node simply reads a pointcloud from a PrimeSense sensor and writes it to
 * a file.
 */

class SaveCloud {

 public:
	SaveCloud(int argc, char **argv); // needs argc and argv to pass to ros::init()
	~SaveCloud();

	/**
	 * run() is essentially the main() of the class, starts everything up
	 */
	void run();
	
	// runs when we recieve a message to save the pointcloud
	void saveSignalCallback(const std_msgs::String& filename);

	// does the actual saving, runs when we've recieved a pointcloud to be saved
	void saverCallback(const sensor_msgs::PointCloud2ConstPtr& cloudMsg);


 private:
	bool save_in_progress_; // used to communicate between callbacks
	std::string filename_;

};

int main(int argc, char **argv) {

	SaveCloud saver(argc, argv);
	saver.run();

}

SaveCloud::SaveCloud(int argc, char **argv) {
	
  save_in_progress_ = false;
	filename_ = "test_pcd";

	/**
	 * ros::init() must be called before any other ros functions,
	 * sets up the node with Master
	 */
	ros::init(argc, argv, "save_pointcloud");

	ros::start(); //manually handling node's lifecycle; now unnecessary

}

SaveCloud::~SaveCloud() {
	
	// make sure we've shutdown node
	while( ros::ok() ) {
		ros::shutdown();
	}

}

void SaveCloud::run() {

	ros::NodeHandle nh;
  /**
   * subscribe to the saveNow topic to know when the user wants to save the pointcloud.
	 * queue of 1 because we don't need to get more than one at a time
	 */
	 ros::Subscriber saveListener = nh.subscribe("saveNow", 1, &SaveCloud::saveSignalCallback, this);

	ROS_INFO("SaveCloud ready, waiting for signal");

	/**
	 * loop indefinitely, pumping callbacks. Ends on Ctrl-C or on shutdown from Master
	 */
	 ros::spin();

}

void SaveCloud::saveSignalCallback(const std_msgs::String& filename) {

	// boolean allows this function to block until the saving is done
	save_in_progress_ = true;

	// set filename
	if( filename.data.compare("") == 0 )
	{
		filename_ = "test_pcd";
	}
	else
	{
		filename_ = filename.data;
	}

	// subcribes to points topic
	ros::NodeHandle nh;
	ros::Subscriber cloudListener = nh.subscribe("/camera/depth_registered/points", 1,
																									&SaveCloud::saverCallback, this);

	// blocks until the cloud has been saved
	while( save_in_progress_ )
	{
		ros::spinOnce();
	}
	// Saving is now done
	
	// shutdown subscriber
	cloudListener.shutdown();

}

void SaveCloud::saverCallback(const sensor_msgs::PointCloud2ConstPtr& cloudMsg )
{
	
	ROS_INFO("Recieved cloud; saving...");
	ROS_INFO_STREAM("Fields available in cloud: " <<  pcl::getFieldsList(*cloudMsg) );

	// convert the msg -> pcl::PointCloud<PointT>
	pcl::PointCloud<pcl::PointXYZRGB> pclCloud;
	pcl::fromROSMsg(*cloudMsg, pclCloud);

	// save it
	pcl::io::savePCDFileASCII( filename_ + ".pcd", pclCloud);
	ROS_INFO_STREAM("Saved "<< pclCloud.points.size() <<
										" points to " << filename_ << ".pcd");

	save_in_progress_ = false;

}
