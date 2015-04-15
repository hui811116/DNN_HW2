CC=gcc
CXX=g++
NVCC=nvcc -arch=sm_21 -w
CPPFLAGS=-std=c++11 -O2
NVCFLAGS=-Xcompiler -fPIC -std=c++11
EXECUTABLES=

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

debug:CPPFLAGS+=-g

vpath %.h include/
vpath %.cpp src/
vpath %.cu src/

INCLUDE= -I include/\
	 -I $(LIBCUMATDIR)include/\
	 -I $(CUDADIR)include/\
	 -I $(CUDADIR)samples/common/inc/

LD_LIBRARY=-L$(CUDADIR)lib64 -L$(LIBCUMATDIR)lib
LIBRARY=-lcuda -lcublas -lcudart

EXECUTABLES=larry
#=============APP================================
larry: example/larryTest.cpp $(OBJ) $(LIBS)
	$(CXX) $(CPPFLAGS) -o bin/larry.app $^ $(INCLUDE) $(LIBRARY) $(LD_LIBRARY)
ahpan: example/ahpanDatasetTest.cpp $(OBJ) $(LIBS)
	$(CXX) $(CPPFLAGS) -o bin/ahpanDatasetTest.app $^ $(INCLUDE) $(LIBRARY) $(LD_LIBRARY)
svmGen: example/svmFeatureGen.cpp
	$(CXX) $(CPPFLAGS) -o bin/svmFeatureGen.app $^



#ADD HERE*
#================================================

#===========UTIL==========
dir:
	@mkdir -p obj; mkdir -p bin

ctags:
	@rm -f src/tags tags
	@echo "Tagging src directory"
	@cd src; ctags -a *.cpp ../include/*.h; ctags -a *.cu ../include/*.h; cd ..
	@echo "Tagging main directory"
	@ctags -a *.cpp src/* ; ctags -a *.cu src/*

clean:
	@echo "All objects and executables removed"
	@rm -f $(EXECUTABLES) obj/* bin/*


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
	@$(NVCC) $(NVCCFLAGS) $(INCLUDE) -o $@ -c $<

