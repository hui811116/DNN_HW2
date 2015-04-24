CC=gcc
CXX=g++
NVCC=nvcc -arch=sm_21 -w
CPPFLAGS=-std=c++11 -O2
NVCFLAGS=-Xcompiler -fPIC -std=c++11
CUDADIR=/usr/local/cuda/
LIBCUMATDIR=tool/libcumatrix/
SVMDIR=tool/svm_struct/
OBJ=obj/svmset.o obj/myAlgorithm.o 
# ================================
# = 		ADD EXE HERE         =
# ================================
EXECUTABLES=svmGen featNorm

# +==============================+
# +======== Phony Rules =========+
# +==============================+

.PHONY: debug all clean 
all:$(EXECUTABLES)
	cd svm_struct; make

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
	 -I $(CUDADIR)samples/common/inc/\
	 
LD_LIBRARY=-L$(CUDADIR)lib64 -L$(LIBCUMATDIR)lib
LIBRARY=-lcuda -lcublas -lcudart

#=============APP================================
larry: example/larryTest.cpp $(OBJ) $(LIBS)
	$(CXX) $(CPPFLAGS) -o bin/larry.app $^ $(INCLUDE) $(LIBRARY) $(LD_LIBRARY)
#ahpan: example/ahpanDatasetTest.cpp $(OBJ) $(LIBS)
#	$(CXX) $(CPPFLAGS) -o bin/ahpanDatasetTest.app $^ $(INCLUDE) $(LIBRARY) $(LD_LIBRARY)
svmGen: example/svmFeatureGen.cpp
	$(CXX) $(CPPFLAGS) -o bin/svmFeatureGen.app $^
featNorm: example/featureNorm.cpp
	$(CXX) $(CPPFLAGS) -o bin/featNorm.app $^
structSvm: svm_empty_learn svm_empty_classify
svm_empty_learn: 
	@cd tool/svm_struct; make svm_empty_learn; cd ../..
svm_empty_classify:
	@cd tool/svm_struct; make svm_empty_classify; cd ../..
#===========UTIL==========
dir:
	@cd svm_struct; make dir; cd ..
	@mkdir -p obj; mkdir -p bin

ctags:
	@cd svm_struct; make ctags; cd ..
	@rm -f src/tags tags
	@echo "Tagging src directory"
	@cd src; ctags -a *.cpp ../include/*.h; ctags -a *.cu ../include/*.h; cd ..
	@echo "Tagging main directory"
	@ctags -a src/* ; ctags -a svm_struct/*

clean:
	@echo "All objects and executables removed"
	@rm -f $(EXECUTABLES) obj/* bin/*
	@cd svm_struct; make clean; cd ..

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

