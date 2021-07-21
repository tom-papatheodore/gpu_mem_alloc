HIPCC    = hipcc
HIPFLAGS =

gpu_mem_alloc: gpu_mem_alloc.o
	$(HIPCC) $(HIPFLAGS) gpu_mem_alloc.o -o gpu_mem_alloc

gpu_mem_alloc.o: gpu_mem_alloc.cpp
	$(HIPCC) $(HIPFLAGS) -c gpu_mem_alloc.cpp

.PHONY: clean

clean:
	rm -f gpu_mem_alloc *.o
