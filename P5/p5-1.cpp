//Computer Orginization 2122
//Kevin Gillmore

#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <iomanip>
#include <cassert>
#include <arm_neon.h>
#include <vector>
#include <thread>
#include <sys/time.h>
#include <cstring>
#include <stdlib.h> 

using namespace std;

void FillArrays(float * array, int size);
double SumOfSums(float * p, int size);
void SingleCore(float * a, float * b, float * c, int size);
void Intrinsic(float * a, float * b, float * c, int size);
void Reset(float * c, int size);

int main(int argc, char * argv[]) {
    srand(time(0));                                 //no repeating rand
    int size = 128;                                 //# of floats
    int iter = 1;                                   //iterations

    //Command line options
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i],"-h") == 0){              //strcmp bc comparing arguments to "strings" 
            cout << "help:" << endl <<              //with == aprently isn't allowed
            "-h       prints this" << endl <<
            "-s size  sets size (default is 128 - will be made divisible by 16)" << endl <<
            "-i iter  sets iterations (default is 1)" << endl;
            return 0;
        }
        if(strcmp(argv[i],"-s") == 0){
            if((atoi(argv[i+1])%16) == 0){
                size = atoi(argv[i+1]);
            }
            else{
                //Gurantees only multiple of 16 for size
                size = atoi(argv[i+1]) + (16 - (atoi(argv[i+1])%16));
                cout << "size input rounded up to nearest multiple of 16" << endl;
            }
        }
        if(strcmp(argv[i],"-i") == 0){
            iter = atoi(argv[i+1]);
        }
    }

    //Dynamic Memory Allocation
    float * a = (float *) aligned_alloc(16, size * sizeof(float));
    float * b = (float *) aligned_alloc(16, size * sizeof(float));
    float * c = (float *) aligned_alloc(16, size * sizeof(float));

    for (int i = 0;i < size; i++){                  //Fill Arrays to be added
        FillArrays(a, size);
        FillArrays(b, size);
    }

    //MB numBytes/1048576
    cout << "Array Size: " << size << " total size in MB: " << (sizeof(float)*size*3)/1048576 << endl;
	cout << "Iterations: " << iter << endl;
    cout << "Available cores: " << thread::hardware_concurrency() << endl;
	cout << "Single core timings..." << endl;

    //NATIVE
    timeval start;
    timeval end;
    timeval elapsed;

    Reset(c, size);                                 //clear c for each time test
    gettimeofday(&start, NULL);
    for (int i = 0; i < iter; i++){
	    SingleCore(a, b, c, size);
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &elapsed);               //subtract beg from end for total
    //calculate the average time for all iterations full seconds coverted + remaining microseconds
    float avgTime = (float)elapsed.tv_sec + (float)elapsed.tv_usec/1000000.0;
    cout << "Native: " << avgTime << " Check: " << SumOfSums(c, size) << endl;

    //NEON
    Reset(c, size);
    gettimeofday(&start, NULL);
    for (int i = 0; i < iter; i++){
	    Intrinsic(a, b, c, size);
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &elapsed);
    avgTime = (float)elapsed.tv_sec + (float)elapsed.tv_usec/1000000.0;
    cout << "NEON: " << avgTime << " Check: " << SumOfSums(c, size) << endl;

    //THREADING                                     //Offset not working for splitting work between cores
    Reset(c, size);
    cout << "Threaded timing..." << endl;
    int numThreads = 0;
    numThreads = thread::hardware_concurrency();    //number of Cores available
    int offset = size/numThreads;                   //divide work
    vector <thread> threads;

    //NATIVE
    gettimeofday(&start, NULL);
    for (int i = 0; i < iter; i++){
        for (int i = 0;i < numThreads; i++){
        //Divide Array into blocks of work for each thread and check for extra at end
            if(size % numThreads != 0 && i == numThreads -  1){
                //Very fincicky arguments and only one core works then breaks
                threads.push_back(thread (SingleCore, a+(i*offset*sizeof(float)), b+(i*offset*sizeof(float)),
                c+(i*offset*sizeof(float)), (offset)+(size % numThreads)));
            }
            else{
                threads.push_back(thread (SingleCore, a+(i*offset*sizeof(float)), b+(i*offset*sizeof(float)),
                c+(i*offset*sizeof(float)), offset));
            }
            threads[i].join();                      //Bring thread back to the universe
        }
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &elapsed);
    avgTime = (float)elapsed.tv_sec + (float)elapsed.tv_usec/1000000.0;
    cout << "Native: " << avgTime << " Check: " << SumOfSums(c, size) << endl;
    threads.clear();

    //NEON
    Reset(c, size);
    gettimeofday(&start, NULL);
    for (int i = 0; i < iter; i++){
        for (int i = 0;i < numThreads; i++){
            if(size % numThreads != 0 && i == numThreads -  1){
                threads.push_back(thread (Intrinsic, a+(i*offset*sizeof(float)), b+(i*offset*sizeof(float)),
                c+(i*offset*sizeof(float)), (offset)+(size % numThreads)));
            }
            else{
                threads.push_back(thread (Intrinsic, a+(i*offset*sizeof(float)), b+(i*offset*sizeof(float)),
                c+(i*offset*sizeof(float)), offset));
            }
            threads[i].join();
        }
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &elapsed);
    avgTime = (float)elapsed.tv_sec + (float)elapsed.tv_usec/1000000.0;
    cout << "NEON: " << avgTime << " Check: " << SumOfSums(c, size) << endl;
    threads.clear();

    return 0;
}

void FillArrays(float * array, int size){           //Fills an array wth random floats between 0 and 1 inclusive
    for(int i = 0;i < size; i++){
        //rand() returns int
        array[i] = (float)(rand()) / (float)(RAND_MAX);   
    }
}

void Reset(float * c, int size){
    memset(c, 0, size * sizeof(float));             //initialize whole c array to zero before each test
}

double SumOfSums(float * p, int size) {             //Add up c array
    double retVal = 0;
    for (int i = 0; i < size; i++){
        retVal += (p[i]);
    }
    return retVal;
}

//Adds corresponding entries a and b to c
void SingleCore(float * a, float * b, float * c, int size) {
	//cout << __FUNCTION__ << " " << hex << size << dec << " " << size << endl;
	// Ensure compatibility with a 16-times unrolled loop.
	assert((size & 0xF) == 0);
	size = size / 16;

	for (int i = 0; i < size; i++) {
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
		*(c++) = *(a++) + *(b++);
	}
}
//Same as above but float32x4_t is a vector of 4 and adds at the same time
void Intrinsic(float * a, float * b, float * c, int size){
	for (int i = 0; i < size; i+=4) {
        float32x4_t aArr = {a[i], a[i+1], a[i+2], a[i+3]}, bArr = {b[i], b[i+1], b[i+2], b[i+3]};
        float32x4_t cArr = vaddq_f32(aArr, bArr);   //Add simultaneously into array
        c[i] = cArr[0];                             //Results into c 1 at a time, not sure iff adding all at once is possible
        c[i+1] = cArr[1];
        c[i+2] = cArr[2];
        c[i+3] = cArr[3];
	}
}