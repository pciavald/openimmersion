#!/bin/sh
nc -l -p 5001 | ./build/getvideo &
ssh 'pi@192.168.0.92' 'raspivid -t 0 -w 1280 -h 720 -o - | nc 192.168.0.15 5001'
