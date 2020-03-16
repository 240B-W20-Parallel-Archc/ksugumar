default: latency

CC=g++
CFLAGS=--std=c++11 -fopenmp

latency: latency.cpp
	$(CC) $(CFLAGS) latency.cpp -o latency
