CC=gcc
CXX=g++
NVCC=nvcc -arch=sm_21 -w
CFLAGS=
EXECUTABLES=

CUDADIR=/usr/local/cuda/
LIBCUMATDIR=tool/libcumatrix/
CUMATOBJ=$(LIBCUMATDIR)obj/device_matrix.o $(LIBCUMATDIR)obj/cuda_memory_manager.o
LIBS=$(LIBCUMATDIR)lib/libcumatrix.a
HEADEROBJ=obj/algorithm.o

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
	 -I $(CUDA_DIR)include/\
	 -I $(CUDA_DIR)samples/common/inc/

LD_LIBRARY=-L$(CUDA_DIR)lib64 -L$(LIBCUMATDIR)lib
LIBRARY=-lcuda -lcublas -lcudart -lcumatrix


larry: $(LIBS) $(HEADEROBJ) 
#	$(CXX) $(CPPFLAGS) $(INCLUDE) -o $(TARGET) $^ $(LIBS) $(LIBRARY) $(LD_LIBRARY)

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

