#!/bin/sh
g++ -W -Wall netcvs.cpp -o netcvs -I/opt/local/include -L/opt/local/lib \
	-lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread
g++ -W -Wall netcvc.cpp -o netcvc -I/opt/local/include -L/opt/local/lib \
	-lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread
