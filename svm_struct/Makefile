# Makefile for empty SVM-struct API, 03.10.06

#Call 'make' using the following line to make CYGWIN produce stand-alone Windows executables
#		make 'SFLAGS=-mno-cygwin'

#Use the following to compile under unix or cygwin
CC = gcc
LD = gcc

CFLAGS =   $(SFLAGS) -O3 -fomit-frame-pointer -ffast-math -Wall 
LDFLAGS =  $(SFLAGS) -O3 -lm -Wall
#CFLAGS =  $(SFLAGS) -pg -Wall
#LDFLAGS = $(SFLAGS) -pg -lm -Wall 
LIBS = -lm
OUTOBJDIR = ../obj/
OBJ=obj/svm_struct_api.o obj/svm_struct_learn_custom.o

.PHONY: all clean 

all: svm_empty_learn svm_empty_classify dir

clean: svm_light_clean svm_struct_clean
	@echo "removing obj files and dump files..."
	@rm -f obj/*.o *.tcov *.d core gmon.out *.stackdump 

vpath %.h include/
vpath %.c src/

INCLUDE= -I include/\
		 -I svm_struct/include/\
		 -I svm_light/

#-----------------------#
#----   SVM-light   ----#
#-----------------------#
svm_light_hideo_noexe: 
	cd svm_light; make svm_learn_hideo_noexe

svm_light_clean: 
	cd svm_light; make clean

#----------------------#
#----  STRUCT SVM  ----#
#----------------------#

svm_struct_noexe: 
	cd svm_struct; make svm_struct_noexe

svm_struct_clean: 
	cd svm_struct; make clean


#-------------------------#
#----  SVM empty API  ----#
#-------------------------#

svm_empty_classify: svm_light_hideo_noexe svm_struct_noexe $(OBJ) svm_struct/svm_struct_classify.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o $(OUTOBJDIR)myAlgorithm.o
	$(LD) $(LDFLAGS) $(OBJ) svm_struct/svm_struct_classify.o svm_light/svm_common.o svm_struct/svm_struct_common.o -o svm_empty_classify.app $(INCLUDE) $(LIBS)

svm_empty_learn: svm_light_hideo_noexe svm_struct_noexe $(OBJ) svm_struct/svm_struct_learn.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o $(OUTOBJDIR)myAlgorithm.o
	$(LD) $(LDFLAGS) svm_struct/svm_struct_learn.o $(OBJ) svm_light/svm_hideo.o svm_light/svm_learn.o svm_light/svm_common.o svm_struct/svm_struct_common.o svm_struct/svm_struct_main.o -o svm_empty_learn.app $(INCLUDE) $(LIBS)

obj/svm_struct_api.o: src/svm_struct_api.c include/svm_struct_api.h include/svm_struct_api_types.h
	$(CC) -c $(CFLAGS) src/svm_struct_api.c $(INCLUDE) -o $@

obj/svm_struct_learn_custom.o: src/svm_struct_learn_custom.c include/svm_struct_api.h
	$(CC) -c $(CFLAGS) src/svm_struct_learn_custom.c $(INCLUDE) -o $@

#=================
#==OTHER TARGETS==
#=================
$(OUTOBJDIR)myAlgorithm.o:
	@cd .. ; make obj/myAlgorithm.o; cd svm_struct

obj/%.o: src/%.c include/%.h
	@echo "compiling $@ "
	@$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

#=================
#======UTIL=======
#=================
ctags:
	@rm -f tags svm_struct/tags svm_light/tags
	@echo "Tagging svm_struct"
	@cd svm_struct; ctags -a *.c *.h; cd ..
	@echo "Tagging svm_light"
	@cd svm_light; ctags -a *.c *.h; cd ..
	@ctags -a *.c src/* svm_struct/* svm_light/* 
dir:
	@mkdir -p obj
	@mkdir -p include
	@mkdir -p src

