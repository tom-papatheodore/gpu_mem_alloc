NVCC    = nvcc
CUFLAGS = -std=c++11

gpu_mem_alloc: gpu_mem_alloc.o
	$(NVCC) $(CUFLAGS) gpu_mem_alloc.o -o gpu_mem_alloc

gpu_mem_alloc.o: gpu_mem_alloc.cu
	$(NVCC) $(CUFLAGS) -c gpu_mem_alloc.cu

.PHONY: clean

clean:
	rm -f gpu_mem_alloc *.o
