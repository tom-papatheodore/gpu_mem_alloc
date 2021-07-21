#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <getopt.h>
#include <hip/hip_runtime.h>

// Macro for checking errors in HIP API calls
#define hipErrorCheck(call)                                                               \
do{                                                                                       \
    hipError_t hipErr = call;                                                             \
    if(hipSuccess != hipErr){                                                             \
      printf("HIP Error - %s:%d: '%s'\n", __FILE__, __LINE__, hipGetErrorString(hipErr)); \
      exit(0);                                                                            \
    }                                                                                     \
}while(0)

/* =================================================================================
Parse command line arguments
==================================================================================*/
int N      = 1;
int width  = 1024;
int height = 1024;
int depth  = 64;

void print_help(){
    printf(
        "-----------------------------------------------------------\n"
        "Usage: ./gpu_mem_alloc [OPTIONS]                         \n\n"
        "--num_iter=<value>, -N <value> :  Number of array elements \n"
        "                                  (default is 1)           \n"
        "                                                           \n"
        "--width=<value>,    -W <value> :  width in elements        \n"
        "                                  (default is 1024)        \n"
        "                                                           \n"
        "--height=<value>,   -H <value> :  height in elements       \n"
        "                                  (default is 1024)        \n"
        "                                                           \n"
        "--depth=<value>,    -D <value> :  depth in elements        \n"
        "                                  (default is 64)          \n"
        "                                                           \n"
        "--help,             -h         :  show help                \n"
        "-----------------------------------------------------------\n"
    );
    exit(1);
}

void process_arguments(int argc, char *argv[]){
    const char* const short_options = "N:W:H:D:h";
    const option long_options[] = {
        {"num_iter", optional_argument, nullptr, 'N'},
        {"width",    optional_argument, nullptr, 'W'},
        {"height",   optional_argument, nullptr, 'H'},
        {"depth",    optional_argument, nullptr, 'D'},
        {"help",     no_argument,       nullptr, 'h'},
        {nullptr,    no_argument,       nullptr,   0}
    };

    while(true){
        const auto opts = getopt_long(argc, argv, short_options, long_options, nullptr);
        if(-1 == opts){ break; }
        switch(opts){
            case 'N':
                N = std::stoi(optarg);
                break;
            case 'W':
                width = std::stoi(optarg);
                break;
            case 'H':
                height = std::stoi(optarg);
                break;
            case 'D':
                depth = std::stoi(optarg);
                break;
            case 'h':
            default:
                print_help();
                break;
        }
    }
}

/* =================================================================================
Main program
==================================================================================*/
int main(int argc, char *argv[])
{
    process_arguments(argc, argv);

    size_t mem_per_iter = width * height * depth * sizeof(float);
    size_t tryMem = mem_per_iter * N;

    hipPitchedPtr pptr[256];

    size_t freeInternal(0u);
    size_t totalInternal(0u);

    hipErrorCheck( hipMemGetInfo(&freeInternal, &totalInternal) );   

    std::cout << "================================================="                        << std::endl;
    std::cout << "Total GPU Mem                   (B): " << totalInternal                   << std::endl;
    std::cout << "Total GPU Mem Avail             (B): " << freeInternal                    << std::endl;
    std::cout << "Total Requested Mem             (B): " << tryMem                          << std::endl;
    std::cout << "Avail Mem - Total Requested Mem (B): " << (long long int)freeInternal - (long long int)tryMem << std::endl;
    std::cout << "================================================="                        << std::endl;

    if(tryMem > freeInternal){
        std::cout << "Error - Not enough free memory! Exiting..." << std::endl;
        exit(2);
    }

    size_t requested_memory_sum = 0.0;
    size_t actual_memory_sum    = 0.0;

    for(int i=0; i<N; i++){

        hipExtent extent = make_hipExtent(width * sizeof(float), height, depth);
        hipErrorCheck( hipMalloc3D(&pptr[i], extent) );

        requested_memory_sum = requested_memory_sum + mem_per_iter;
        actual_memory_sum    = actual_memory_sum + pptr[i].pitch * height * depth;

        hipErrorCheck( hipMemGetInfo(&freeInternal, &totalInternal) );

        std::cout << "----- "                << "Iteration " << i     << std::endl;
        std::cout << "Width requested (B): " << width * sizeof(float) << std::endl;
        std::cout << "Pitch returned  (B): " << pptr[i].pitch         << std::endl;
        std::cout << "Requested mem   (B): " << mem_per_iter          << std::endl;
        std::cout << "Request sum     (B): " << requested_memory_sum  << std::endl;
        std::cout << "Actual sum      (B): " << actual_memory_sum     << std::endl;
        std::cout << "Expected free   (B): " << (long long int)totalInternal - (long long int)requested_memory_sum << std::endl;
        std::cout << "Free internal   (B): " << freeInternal          << std::endl;
        std::cout                                                     << std::endl;

    }

    return 0;
}
