CUDA_DIRECTORY=/usr/local/cuda-8.0
CC=$(CUDA_DIRECTORY)/bin/nvcc
#ARCH=-gencode=arch=compute_61,code=sm_61 -gencode=arch=compute_60,code=sm_60 -gencode=arch=compute_52,code=sm_52 -gencode=arch=compute_37,code=sm_37 -gencode=arch=compute_35,code=sm_35 -gencode=arch=compute_30,code=sm_30 -gencode=arch=compute_20,code=sm_20   -gencode=arch=compute_50,code=sm_50
ARCH=-gencode=arch=compute_35,code=sm_35
CFLAGS=-c -std=c++11 $(ARCH) -O3 -I$(CUDA_DIRECTORY)/include 
LDFLAGS=-L$(CUDA_DIRECTORY)/lib64 -lcufft -lcuda
SOURCES=SCRIMP.cu
OBJECTS=SCRIMP.o
EXECUTABLE=SCRIMP

all: $(SOURCES) $(EXECUTABLE)

matlab:
	nvcc $(MATFLAGS) $(SOURCES)
    
$(EXECUTABLE): $(OBJECTS)  $(SOURCES)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	

SCRIMP.o: $(SOURCES)
	$(CC) $(CFLAGS) SCRIMP.cu -o $@
	
clean:
	rm -f *.o SCRIMP
