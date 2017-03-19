PROJECT_NAME = kiwi
BUILDDIR = build
BIN = $(BUILDDIR)/$(PROJECT_NAME).bin
CC = g++
CFLAGS = -Wall -g

build:
	$(CC) kiwi.cpp $(CFLAGS) -o kiwi


all: build
