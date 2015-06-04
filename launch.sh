#!/bin/sh
./build/server
`which xfce4-terminal` -e '\
	ssh pi@192.168.1.109 "/home/pi/openimmersion/build/client"'
