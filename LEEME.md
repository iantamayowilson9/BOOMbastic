# Boombastic

Juego educativo tipo arcade (inspirado en Smoothie Smash de Club Penguin) para
ninos de 7 a 11 anos. Hecho en C++ con raylib. El chango rebota sobre las frutas
que desfilan de derecha a izquierda y va sumando hacia un objetivo.

## Archivos de codigo

- **main.cpp** : archivo principal. SOLO tiene el ciclo del juego y las llamadas
  a las funciones segun la pantalla actual.
- **game.h**   : aqui vive TODO el funcionamiento (constantes, structs, variables
  globales y todas las funciones del juego).

## Carpetas de assets (nombres de los PNG)

- **assets/intro/**   frame_001.png ... frame_215.png   (la intro, YA funciona)
- **assets/ui/**      boton_normal.png, boton_hover.png, logo.png, filtro_lineas.png
- **assets/sprites/** chango.png, chango_clavado.png, bomba.png
- **assets/frutas/**  sandia.png, platano.png, uva.png, manzana.png
- **assets/sonido/**  (sonidos futuros)
- **datos/**          records.dat  (archivo binario de records, se crea al jugar)

## Compilar y jugar

Con make:

    make
    make run

A mano en Windows (MinGW / w64devkit):

    g++ main.cpp -o boombastic.exe -lraylib -lopengl32 -lgdi32 -lwinmm
    .\boombastic.exe

    g++ -std=c++11 -Wall main.cpp -o boombastic.exe -I C:/raylib/raylib/src -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm

    g++ -std=c++11 -Wall *.cpp -o boombastic.exe -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm

## Controles

- Flechas izquierda/derecha (o A/D): mover al chango
- ESPACIO: clavado (caer rapido)
- Flechas arriba/abajo: subir o bajar la gravedad
- R: reiniciar la gravedad
- F: prender/apagar el filtro retro
- ESC: volver al menu
