#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int		main(void)
{
	cout << "begin" << endl;
    VideoCapture	stream("/dev/stdin");
    Mat				edges;

	cout << "stream open" << endl;
    if(!stream.isOpened())
        return (-1);
    namedWindow("edges",1);
	while (42)
    {
        Mat frame;
        stream >> frame;
        cvtColor(frame, edges, CV_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
