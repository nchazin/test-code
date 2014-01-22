CC=c++
LIBRARIES=zmq
FLAGS=-std=c++11

all: testcase

testcase: zmq_thread_test.cc
	$(CC) $(FLAGS) -o thread_test -lzmq zmq_thread_test.cc -Wl,-rpath,/usr/local/lib



