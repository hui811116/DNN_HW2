# Makefile for SVM-struct, 03.10.06

#Use the following to compile under unix or cygwin
CC = gcc
LD = gcc

#Call 'make' using the following line to make CYGWIN produce stand-alone Windows executables
#		make 'SFLAGS=-mno-cygwin'

CFLAGS =    $(SFLAGS) -O3 -fomit-frame-pointer -ffast-math -Wall 
LDFLAGS =   $(SFLAGS) -O3 -lm -Wall
#CFLAGS =   $(SFLAGS) -pg -Wall
#LDFLAGS =  $(SFLAGS) -pg -Wall

all: svm_struct_noexe

svm_struct_noexe: svm_struct_learn.o svm_struct_classify.o svm_struct_common.o svm_struct_main.o 

.PHONY: clean
clean:
	rm -f *.o *.tcov *.d core gmon.out *.stackdump

INCLUDE= -I include/\
		 -I ../svm_light/\
		 -I ../include/

#----------------------#
#----  STRUCT SVM  ----#
#----------------------#

svm_struct_common.o: svm_struct_common.c include/svm_struct_common.h
	$(CC) -c $(CFLAGS) svm_struct_common.c $(INCLUDE) -o svm_struct_common.o

svm_struct_learn.o: svm_struct_learn.c include/svm_struct_common.h
	$(CC) -c $(CFLAGS) svm_struct_learn.c $(INCLUDE) -o svm_struct_learn.o

svm_struct_main.o: svm_struct_main.c include/svm_struct_learn.h include/svm_struct_common.h
	$(LD) -c $(CFLAGS) svm_struct_main.c $(INCLUDE) -o svm_struct_main.o

svm_struct_classify.o: svm_struct_classify.c include/svm_struct_common.h 
	$(LD) -c $(CFLAGS) svm_struct_classify.c $(INCLUDE) -o svm_struct_classify.o
