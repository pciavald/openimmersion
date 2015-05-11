#!/bin/bash
nc -l -p 5001 | ./build/face &
ssh 'pi@192.168.0.92' 'raspivid -vf -t 0 -o - | nc 192.168.0.15 5001'
#./build/face
