#!/bin/bash
#nvcc -c kernel.cu -O3 -D_FORCE_INLINES -dlink --ptxas-options="-v -dlcm=cg" -lcuda -lcudart -I/usr/local/cuda/samples/common/inc
nvcc -DNDEBUG -c kernel.cu -O3 -D_FORCE_INLINES -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_50,code=sm_50 --ptxas-options="-v -dlcm=cg" -I/usr/local/cuda/samples/common/inc -o kernel.o
g++ -DNDEBUG -c ./boinc_a5_btf.cpp -o boinc_a5_btf.o  -I/usr/local/include/boinc/ 
g++ ./boinc_a5_btf.o ./kernel.o /usr/local/lib/libboinc_api.a /usr/local/lib/libboinc.a -o boinc_a5_btf -pthread -L/usr/local/cuda/lib64 -lcuda -lcudart
