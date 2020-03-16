# compile program with necessary flags (OpenMP and C++11)
make

# run program with default parameters (random access with 1 thread --> generates csv with the latency numbers)
./latency -t 1

# run program with read-write enabled and random access with 1 thread --> generates csv with the latency numbers
./latency -t 1 -rw
