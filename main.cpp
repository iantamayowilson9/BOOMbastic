#include "src/game.h"

// Punto de entrada. El juego arranca directo en la partida, sin menu.
int main()
{
    inicializar_juego();

    while (!WindowShouldClose())
    {
        actualizar_juego(GetFrameTime());
        dibujar_juego();
    }

    cerrar_juego();
    return 0;
}
