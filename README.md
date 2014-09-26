xframe
======

a c++ communication application framework. 

components
>kernal
>>the core of the framework
>msg
>>the message defination that the frame and task will use to communicate
>comserv
>>common classes including thread/socket and some data structure
>task
>>task defination. each task will work in a thread
>xclient
>>it is a client to communicate to xframe to control it, it supports reload and set command by now

build
=====
##get the source
```
git clone git@github.com:AresTao/xframe.git
```
##system
>linux
>>ubuntu10/11 centos6.x

##Dependencies
```
yum install openssl-devel mysql mysql-devel
```
##compile
```bash
cd xframe
make clean all install
```
the so and lib will be in ../_lib/ 
