CC=gcc
CXX=g++
NVCC=nvcc -arch=sm_21 -w
CPPFLAGS=-g -std=c++11

CUDADIR=/usr/local/cuda/
LIBCUMATDIR=tool/libcumatrix/
#CUMATOBJ=$(LIBCUMATDIR)obj/device_matrix.o $(LIBCUMATDIR)obj/cuda_memory_manager.o
OBJ=obj/myAlgorithm.o obj/dataset.o

# +==============================+
# +======== Phony Rules =========+
# +==============================+

.PHONY: debug all clean 

LIBS=$(LIBCUMATDIR)lib/libcumatrix.a

$(LIBCUMATDIR)lib/libcumatrix.a:
	@echo "Missing library file, trying to fix it in tool/libcumatrix"
	@cd tool/libcumatrix/ ; make ; cd ../..

debug:
	@CPPFLAGS+=-g

vpath %.h include/
vpath %.cpp src/

INCLUDE= -I include/\
	 -I $(LIBCUMATDIR)include/\
	 -I $(CUDADIR)include/\
	 -I $(CUDADIR)samples/common/inc/

LD_LIBRARY=-L$(CUDADIR)lib64 -L$(LIBCUMATDIR)lib
LIBRARY=-lcuda -lcublas -lcudart

EXECUTABLES=larry
#=============APP================================
larry:  $(OBJ) 
#	$(CXX) $(CPPFLAGS) -o larry.app $^ $(INCLUDE) $(LIBRARY) $(LD_LIBRARY)

#ADD HERE*
#================================================

#===========UTIL==========
dir:
	@mkdir -p obj

ctags:
	@rm -f src/tags tags
	@echo "Tagging src directory"
	@cd src; ctags -a *.cpp ../include/*.h; ctags -a *.cu ../include/*.h; cd ..
	@echo "Tagging main directory"
	@ctags -a *.cpp src/* ; ctags -a *.cu src/*

clean:
	@echo "All objects and executables removed"
	@rm -f $(EXECUTABLES) obj/* ./*.app bin/*


# +==============================+
# +===== Other Phony Target =====+
# +==============================+
obj/%.o: src/%.cpp include/%.h
	@echo "compiling OBJ: $@ " 
	@$(CXX) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<

obj/datasetJason.o: src/datasetJason.cpp include/dataset.h 
	@echo "compiling OBJ: $@ "
	@$(CXX) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<

obj/%.o: %.cu
	@echo "compiling OBJ: $@ "
	@$(NVCC) $(NVCCFLAGS) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<

