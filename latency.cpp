#include<iostream>
#include<random>
#include<cstdlib>
#include<chrono>
#include<cmath>
#include<fstream>
#include<omp.h>

using namespace std;
using namespace chrono;

unsigned int uniform(unsigned int limit);

//globals
int num_threads;
int strideSize;
bool stride;
size_t memSize;
bool rw_flag;
bool one_stride = false;
bool one_memSize = false;

//Usage
void print_usage(){
	
	cout << "Usage:	[-t | --threads] <number of threads> \n"
			"	[-rw | --read-write 	Enables read-modify-write \n"
			"	[-s | --stride]  	Enables sequential accesses\n"
			"	[-ss | --strideSize] <stride size(bytes)> 	Stride size to use\n"
//			"	[-m | --memSize] <memory size to allocate(bytes)\n"
			"	[-h | --help]	Shows Usage\n"
		<<	endl;
}

//Return memory with randomly linked nodes
void** random_linkedlist(size_t size) {
	size_t len = size/sizeof(void*);
	void** buffer = new void*[len];

	size_t* idx = new size_t[len];
	for(size_t i=0; i<len; i++) {
		idx[i] = i;
	}

	//Shuffle indices
	for(size_t i=0; i<len-1; i++) {
		size_t j = i + uniform(len-i);
		if(j != i) {
			size_t temp = idx[i];
			idx[i] = idx[j];
			idx[j] = temp;
		}
	}

	//Link the shuffled nodes
	for(size_t i=0; i<len-1; i++) {
		buffer[idx[i]] = (void*) &buffer[idx[i+1]]; 
	}
	//Link last node to first node
	buffer[idx[len-1]] = (void *) &buffer[idx[0]];

	//clean-up and return randomized memory
	delete[] idx;
	return buffer;
}

//Random number generator from uniform distribution
unsigned int uniform(unsigned int limit) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0,limit-1);
	return dis(gen);
}

//Returns memory with linked nodes - "stride" bytes apart each other
void** strided_linkedlist(size_t size, int stride) {
	//cout << "stride: " << stride << ", so im here" << endl;
	size_t len = size/sizeof(void*);
	void** buffer = new void*[len];

	size_t* idx = new size_t[len];
	for(size_t i=0; i<len; i++) {
		idx[i] = i;
	}

	//Arrange indices based on stride
	if(strideSize > 0) {
		//cout << "strideSize: " << strideSize << endl;
		for(size_t i=0; i<len; i++) {
			int val = (i*stride/sizeof(void*))%len;
			idx[i] = val;
		}
	} else {
		//cout << "strideSize: " << strideSize << ", so im here" << endl;
		for(size_t i=0; i<len; i++) {
			idx[i] = i;
		}
	}	

	//Link the nodes
	for(size_t i=0; i<len-1; i++) {
		buffer[idx[i]] = (void*) &buffer[idx[i+1]]; 
	}
	buffer[idx[len-1]] = (void *) &buffer[idx[0]];
	delete[] idx;
	return buffer;
}


//volatile keyword to avoid compiler optimization
volatile void* defeat_prefetch;

double pointer_chase(void** memory, size_t count) {
	
	void** pointer = (void**) memory;
	void** next;
	void** temp;

	//Choose Read vs Read-Modify-Write
	
	auto start=system_clock::now();
	auto end=system_clock::now();

	if(rw_flag) {
		//cout << "rw_flag: " << rw_flag << ", so im here" << endl;
  		start = system_clock::now();
		while (count > 0) {
			next = (void**) *pointer;
			
			//read-modify-write
			temp = (void**) *pointer;
			temp = (void**) ((uintptr_t)temp & 0x7fffffffffffffff);
			*pointer = temp;
			
			pointer = next;
			count--;
		}
		end = system_clock::now();
	} else {
		start = system_clock::now();
		while (count > 0) {
			pointer = (void**) *pointer;
			count--;
		}
		end = system_clock::now();
	}

	//Measure and return time taken
	auto elapsed = duration<double>(end-start).count();
	defeat_prefetch = *pointer;
	return elapsed;
}	

//Default Parameters - set when before command line arguments take effect
void set_defaults() {	
	num_threads = 1;
	strideSize = 64;
	stride = false;
	memSize = 1024*1024*64;
	rw_flag = false; 
}

//Print set parameters before runs
void print_parameters() {
	cout << "Memory Size: " << memSize << endl;
	cout << "Number of Threads: " << num_threads << endl;
	cout << "Read-Write flag: " << rw_flag << endl;
	cout << "Stride flag: " << stride << endl;
	cout << "Stride Size: " << strideSize << endl;
}

//Gets filename based on parameters to write .csv
string get_csv_name(){
	
	string rw, access;	

	if(rw_flag) rw = "rw"; 
	else rw = "read";
	
	if(stride) access = "stride"; 
	else access = "random";

	return "pchase_" + rw + "_" + to_string(num_threads) + "t_" + access + ".csv";
	
}


int main(int argc, char *argv[]) {

	set_defaults();
	
	//Parse Command line Arguements
	for(int i=1; i<argc; i++) {
		string arg = argv[i];
		if ((arg == "-h") || (arg == "--help")) {
			print_usage();
			exit(0);

		} else if ((arg == "-t") || (arg == "--threads")) {
			if(i+1 < argc) {
				num_threads = atoi(argv[++i]);
			} else { print_usage(); exit(0); }

		} else if ((arg == "-rw") || (arg == "--read-write")) {
			rw_flag = true;

		} else if ((arg == "-s") || (arg == "--stride")) {
			stride = true;
	
		} else if ((arg == "-ss") || (arg == "--strideSize")) {
			if(i+1 < argc) {
				strideSize = atoi(argv[++i]);
				one_stride = true;	
				}
			else { print_usage(); exit(0); }
//		} else if ((arg == "-m") || (arg == "--memSize")) {
//			if(i+1 < argc) {
//				memSize = atoi(argv[i++]);
//		//			one_memSize = true;
//				}
//			else { print_usage(); exit(0); }
		} else {
			print_usage();	exit(0);
		}
	}

//Init conditions based on access pattern
	size_t min;
	size_t max;
	size_t step;	
	int stride_min;
	int stride_max;

if(!stride) {
	min = 64;
	max = 1024*1024*64;
	step = 1;	
	stride_min=0;
	stride_max=0;

} else {
	min=1024*1024*64;
	max=1024*1024*64;
	step = 1;
	stride_min=0;
	stride_max=1024;
	
	if(one_stride) {
		stride_min=strideSize;
		stride_max=strideSize;
	}
}

	//debug
	//min = 1024*1024*16;
	//max = 1024*1024*64;
	//stride_min=512;
	//stride_max=1024;
	
	//Print parameters set before iterating
	cout << "\n\n";
	print_parameters();
	cout << "\n\n";

	ofstream csv;
	csv.open(get_csv_name());
	//csv.open("test.csv");
	
	if(!stride) {
		csv << "size(bytes),time(ns),Threads,Stride,StrideSize,\n";
		cout << "size(bytes) \t\t time(ns)" << endl;
	} else {
		csv << "size(bytes),time(ns),Threads,Stride,StrideSize,\n";
		cout << "stride(bytes) \t\t time(ns)" << endl;
	}

	csv.close();

//Iterate over different memory sizes and stride widths
	//for(size_t m=min; m<=max; m+= (size_t{1} << ((unsigned int)log2(m)-step)) ) {
	for(size_t m=min; m<=max; m*=2) {
	for(int s=stride_min; s<=stride_max; s+= strideSize) {
		
		double time_ns;
		size_t count;
		double time_avg=0;
		
		count = std::max(m*16, size_t{1}<<30);

//Choose between random or strided access based on given arguement			
			if(!stride) { //Random
				
				//Run on a number of threads using omp
				#pragma omp parallel num_threads(num_threads)
				{			
					void** memory = random_linkedlist(m);
					
					//Pointer Chase
					double time = pointer_chase(memory,count);
						
					//Accumulation for finding average access time across all threads
					#pragma omp atomic
						time_avg += time;
					
					delete[] memory;
				}
			} else { //Strided
				
				//Run on a number of threads using omp
				#pragma omp parallel num_threads(num_threads)
				{
					void** memory = strided_linkedlist(m, s);
					
					//Pointer Chase
					double time = pointer_chase(memory,count);
					
					//Accumulation for finding average access time across all threads
					#pragma omp atomic
						time_avg += time;
					
					delete[] memory;
				}
			}

//Cleanup and report latency
		time_ns = (time_avg/num_threads) * 1000000000/count;

		//Write and display outputs
	csv.open(get_csv_name(), std::ios_base::app);
		if(!stride) {
			csv << m << "," << time_ns << "," << num_threads \
			<< "," << "no" << "," << strideSize << ",\n";
			cout << m << "\t\t\t" << time_ns << endl;
		} else {
			csv << s << "," << time_ns << "," << num_threads \
			<< "," << "yes" << "," << strideSize << ",\n";
			cout << s << "\t\t\t" << time_ns << endl;
		}

	csv.close();
	}
	}

	return 0;
}
