#ifndef ROS_H
# define ROS_H

#include "server.hpp"
#include "ros/ros.h"
#include "geometry_msgs/Point.h"

#define FPS		90

class Ros
{
	public:
		Ros(void);
		~Ros(void);

		void	send(geometry_msgs::Point & pose);

	private:
		ros::NodeHandle		_nh;
		ros::Publisher		_pub;

		Ros(const Ros & c);
		Ros &	operator=(const Ros & c);
};

#endif
