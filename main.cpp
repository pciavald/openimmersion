#include <iostream>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace cv;
using namespace std;
using namespace raspicam;

int		main(void)
{
    RaspiCam_Cv		camera;
    Mat				frame;
    Mat				edges;

    camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
    if (!camera.open())
	{
		cout << "Could not open camera" << endl;
        return (-1);
	}
    namedWindow("edges",1);
	while (42)
    {
        //Mat frame;
        //stream >> frame;
        camera.grab();
        camera.retrieve(frame);
        //cvtColor(frame, edges, CV_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        //imshow("edges", edges);
        imshow("edges", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
