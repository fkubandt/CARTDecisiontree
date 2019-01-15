# Do not edit, changes can be made in makefile.conf
#
# File : Makefile for DecisionTree
# Author: Frederike Kubandt <kubandt@th.physik.uni-frankfurt.de>
# Date : Dec 2019
#

include Makefile.config

CVERSION = 7.3.0
CC = g++
CFLAGS = -Wall 
EXEC = start

CFILES = $(addsuffix .cpp, $(FILES))
OFILES = $(addsuffix .o, $(FILES))

$(EXEC): $(OFILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(OFILES) 

%.o: %.c
	$(CC) $(CFLAGS) -c $<
