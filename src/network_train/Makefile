ELF = bin/cif_network_train.elf

NN_LIBS_PATH=$(HOME)/programming/nn_lib
CELL_LIBS_PATH=$(HOME)/programming/cell_in_fluid/libs

INC+= -I.
INC+= -I$(NN_LIBS_PATH)/libs_common
INC+= -I$(NN_LIBS_PATH)/libs_dataset
INC+= -I$(NN_LIBS_PATH)/libs_nn
INC+= -I$(NN_LIBS_PATH)/libs_experiment
INC+= -I$(CELL_LIBS_PATH)



LIBS+= $(CELL_LIBS_PATH)/cell_libs.a
LIBS+= $(NN_LIBS_PATH)/libs_dataset/libs_dataset.a

LIBS+= $(NN_LIBS_PATH)/libs_experiment/libs_experiment.a
LIBS+= $(NN_LIBS_PATH)/libs_nn/libs_nn.a

LIBS+= $(NN_LIBS_PATH)/libs_common/libs_common.a


#LIBS+= $(NN_LIBS_PATH)/libs_common/libs_common.a
#LIBS+= $(CELL_LIBS_PATH)/cell_libs.a
#LIBS+= $(NN_LIBS_PATH)/libs_dataset/libs_dataset.a
#LIBS+= $(NN_LIBS_PATH)/libs_nn/libs_nn.a

CC = g++
CU = nvcc
LD = g++
OBJCOPY = objcopy
OBJDUMP = objdump
SIZE = size

# DEGUBFLAGS = -g -pg

CFLAGS = $(DEGUBFLAGS) -O2 -Os -s -Wall -Wextra -pedantic -std=c++11 $(INC)
CUDAFLAGS = -O3 -D_FORCE_INLINES -ccbin=$(CXX) -Xcompiler -fPIC -std=c++11 $(INC)

LDFLAGS = $(DEGUBFLAGS) -lstdc++fs -lm -lpthread -pthread -std=c++11 -lX11 -ljsoncpp -L/usr/local/cuda/lib64 -lcuda -lcudart


SRCS+= $(shell find -name '*.cpp')
SRCS_CUDA+= $(shell find -name '*.cu')


OBJS = $(SRCS:.cpp=.o)
OBJS_CUDA = $(SRCS_CUDA:.cu=.oc)

all: $(ELF)
	$(SIZE) $(ELF)

$(ELF): $(LIBS) $(OBJS) $(OBJS_CUDA)
	$(LD) -o $@ $(OBJS) $(OBJS_CUDA) $(LIBS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.oc: %.cu
	$(CU) -c $(CUDAFLAGS) $< -o $@

clean:
	rm $(OBJS) -rf
	rm $(OBJS_CUDA) -rf
	rm $(ELF) -rf
