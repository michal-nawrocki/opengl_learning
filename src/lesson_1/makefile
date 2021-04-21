BIN = main.o
CC = g++
FLAGS = -Wall -pedantic -Wextra
INC = -I ../common/include -I/sw/include -I/usr/local/include
FRAMEWORKS =-lglfw -framework OpenGL -framework OpenAL -framework Cocoa -lglew
SRC = main.cpp

all:
	${CC} ${FLAGS} ${FRAMEWORKS} -o ${BIN} ${SRC} ${INC}