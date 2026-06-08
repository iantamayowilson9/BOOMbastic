# ===========================================================================
#  MAKEFILE - BOOMBASTIC
#  USO:  make        -> COMPILA       make run -> COMPILA Y CORRE
#        make clean  -> LIMPIA
#  NECESITA RAYLIB INSTALADO. DETECTA EL SISTEMA OPERATIVO.
# ===========================================================================
CXX     := g++
CXXFLAGS:= -std=c++11 -Wall
OBJ     := main.o
DESTINO := boombastic

ifeq ($(OS),Windows_NT)
    LIBS    := -lraylib -lopengl32 -lgdi32 -lwinmm
    DESTINO := boombastic.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIBS := -lraylib -framework OpenGL -framework Cocoa -framework IOKit
    else
        LIBS := -lraylib -lm -lpthread -ldl -lrt -lX11
    endif
endif

all: $(DESTINO)

$(DESTINO): main.cpp game.h
	$(CXX) $(CXXFLAGS) main.cpp -o $(DESTINO) $(LIBS)

run: all
	./$(DESTINO)

clean:
	rm -f $(DESTINO) $(OBJ)

.PHONY: all run clean
