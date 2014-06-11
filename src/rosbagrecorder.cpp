#include <ros/ros.h>
#include <iostream> 
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <rosbag/bag.h>

bool latch = true;
int count  = 0;
int limit = 20;
class camBagWriter {
	public:

		camBagWriter(ros::NodeHandle* _n)
		{
			this->n = *_n; 
			myBag.open("myOutput.bag", rosbag::bagmode::Write);
			this->pointSub = this->n.subscribe("/camera/depth_registered/points", 1, &camBagWriter::points, this);
			this->imSub = this->n.subscribe("/camera/rgb/image_color", 1, &camBagWriter::rgb, this);
		}

		camBagWriter(ros::NodeHandle* _n, std::string bagName)
		{
			this->n = *_n; 
			myBag.open(bagName, rosbag::bagmode::Write);
			this->pointSub = this->n.subscribe("/camera/depth_registered/points", 1, &camBagWriter::points, this);
			this->imSub = this->n.subscribe("/camera/rgb/image_color", 1, &camBagWriter::rgb, this);
		}

		void points(const sensor_msgs::PointCloud2Ptr& msg)
		{  
			std::cout << count << std::endl;
			if(count < limit)                                                                                   
			{
				if(latch)
				{
					myBag.write("/camera/depth_registered/points", ros::Time::now(), *msg);   // cloud
					latch = false;
					count++;
				}
			}
			else
			{
				std::cout << "Saving Bag." << std::endl;
				myBag.close();
				pointSub.shutdown();
				imSub.shutdown();
				std::cout << "CTRL + C..." << std::endl;
			}
		}

		void rgb(const sensor_msgs::ImageConstPtr& msg)
		{
			std::cout << count << std::endl;
			if(count < limit)
			{
				if(!latch)
				{
					myBag.write("/camera/rgb/image_color", ros::Time::now(), *msg);
					latch = true;
					count++;
				}
			}
			else
			{
				std::cout << "Saving Bag." << std::endl;
				myBag.close();
				pointSub.shutdown();
				imSub.shutdown();
				std::cout << "CTRL + C..." << std::endl;
			}
			//myBag.write("/camera/depth_registered/points", ros::Time::now(), *msg);   // cloud
		}
	protected:    
		rosbag::Bag myBag;
		sensor_msgs::PointCloud2Ptr x;
 		ros::Subscriber pointSub;
		ros::Subscriber imSub;
		ros::NodeHandle n; 
		ros::Publisher pub;
		
};

int main(int argc, char *argv[])
{
	ros::init(argc, argv, "rosBagRecorder");
	ros::NodeHandle n;

	std::string bagName;
	if (argc > 1){
	    bagName = argv[1];
	    camBagWriter e(&n,bagName);
	    ros::spin();
	}
	else
	{
		camBagWriter e(&n);
		ros::spin();
	}

}