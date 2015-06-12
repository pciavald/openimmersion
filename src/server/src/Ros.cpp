#include "Ros.hpp"

void	Ros::send(geometry_msgs::Point & pose)
{
	_pub.publish(pose);
	ros::spinOnce();
	//_rate.sleep();
}

Ros::Ros(void)
{
	_pub = _nh.advertise<geometry_msgs::Point>("pose", 10);
}

Ros::~Ros(void)
{
}
