# Boombastic

Juego educativo arcade (inspirado en Smoothie Smash de Club Penguin y en la
inercia de Burrito Bison) para ninos. Hecho en C++ con raylib. El chango rebota
sin parar sobre las frutas que desfilan en fila india y va sumando hacia un
objetivo matematico. No hay menu: el juego arranca directo en la partida.

## Arquitectura modular

Solo se compila `main.cpp`. Toda la logica vive en cabeceras dentro de `src/`
que se incluyen una sola vez desde `src/game.h`.

    main.cpp        Punto de entrada limpio
    src/definis.h   Enums, structs, constantes y variables globales
    src/assets.h    Carga unica con respaldo automatico (anti-crash y anti-parpadeo)
    src/fisicas.h   Movimiento, rebote, picada, fila india, combos y particulas
    src/partida.h   Fases, marco, HUD acoplado, reloj digital y pantalla de fin
    src/minijuego.h Lluvia de bananas de 15 segundos
    src/archivos.h  Records en disco con fopen/fwrite (nombres de solo letras)
    src/game.h      Cabecera central: Inicializar, Actualizar, Dibujar, Cerrar

## Carpetas de assets

    assets/textures/interfaz/   marco_pantalla.png
    assets/textures/personaje/  chango_arriba.png chango_aplastar.png
                                chango_izquierda.png chango_derecha.png
    assets/textures/frutas/     fruta_sandia.png fruta_platano.png fruta_uva.png
                                fruta_manzana.png fruta_bomba.png
    assets/textures/ambiente/   fondo_jungla.png particula_jugo.png
    assets/fuentes/             EARTHBOUND.TTF DS-DIGIB.TTF UPHEAVAL.TTF
    assets/sfx/                 aplastar.wav sfx_rebote.wav sfx_dano.wav
                                sfx_bomba_alerta.wav sfx_victoria.wav sfx_combomax.wav
    assets/music/               music_partida.wav music_minijuego.wav
    datos/                      records.dat (se crea solo al jugar)

Si falta cualquier archivo, el juego NO crashea: usa colores planos para las
texturas, la fuente del sistema para el texto y omite el sonido faltante.

## Compilar y jugar

    make
    make run

A mano en Windows (MinGW / w64devkit), apuntando a tu copia de raylib:

    g++ -std=c++11 -Wall main.cpp -o boombastic.exe -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm

## Controles

- Flechas izquierda/derecha (o A/D): mover al chango
- ESPACIO: picada vertical pesada (fast smash). Si caes sobre una fruta correcta
  el rebote es 25 por ciento mas fuerte y la cinta se acelera
- B: lanzar el minijuego de bananas a mano (para probarlo)
- ENTER en la pantalla de fin: jugar de nuevo
