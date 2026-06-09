# ====================================================================
# Makefile de Boombastic
# Solo se compila main.cpp porque los modulos viven en cabeceras .h
# que se incluyen una sola vez desde src/game.h
# ====================================================================

NOMBRE  = boombastic
FUENTES = main.cpp
CXX     = g++
CFLAGS  = -std=c++11 -Wall

# Si raylib no esta instalado en el sistema, descomenta y ajusta estas
# dos lineas con la ruta de tu copia de raylib:
# INCLUDES = -I C:/raylib/raylib/src
# RUTALIBS = -L C:/raylib/raylib/src

ifeq ($(OS),Windows_NT)
	LIBS   = $(RUTALIBS) -lraylib -lopengl32 -lgdi32 -lwinmm
	SALIDA = $(NOMBRE).exe
	BORRAR = del /Q
else
	UNAME = $(shell uname)
	ifeq ($(UNAME),Darwin)
		LIBS = $(RUTALIBS) -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	else
		LIBS = $(RUTALIBS) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	endif
	SALIDA = $(NOMBRE)
	BORRAR = rm -f
endif

all: $(SALIDA)

$(SALIDA): $(FUENTES)
	$(CXX) $(CFLAGS) $(INCLUDES) $(FUENTES) -o $(SALIDA) $(LIBS)

run: all
	./$(SALIDA)

clean:
	$(BORRAR) $(SALIDA)
