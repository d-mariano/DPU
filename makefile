###################################
#	Author:		Dave Mariano
#	Makefile:	makefile for DPU
#################################
dpu:	main.o dpu.o
		cc main.o dpu.o -o dpu

main.o:	main.c dpu.h
		cc -c main.c

dpu.o:	dpu.c dpu.h
		cc -c dpu.c
