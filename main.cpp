#include "globales.h"
#include "jugador.h"
#include "frutas.h"
#include "interfaz.h"
#include "programa.h"

int main()
{
    InitWindow(V_ANCHO, V_ALTO, "Boombastic");
    SetTargetFPS(60);

    cargar_cuadros_intro();
    iniciar_botones_menu();
    cargar_records();
    cargar_ui();
    cargar_texturas_jugador();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Actualizar pantalla actual
        switch (pantalla_actual)
        {
        case INTRO:
            actualizar_intro(dt);
            break;
        case MENU:
            actualizar_menu();
            break;
        case RECORDS:
            actualizar_records();
            break;
        case JUEGO:
            actualizar_juego(dt);
            break;
        case FIN:
            actualizar_fin();
            break;
        default:
            break;
        }

        // Dibujar segun la pantalla
        BeginDrawing();
        ClearBackground(BLACK);

        switch (pantalla_actual)
        {
        case INTRO:
            dibujar_intro();
            break;
        case MENU:
            dibujar_menu();
            break;
        case RECORDS:
            dibujar_records();
            break;
        case JUEGO:
            dibujar_juego();
            break;
        case FIN:
            dibujar_fin();
            break;
        default:
            break;
        }

        dibujar_filtro();

        EndDrawing();
    }

    // Descargar imagenes y salir
    descargar_cuadros_intro();
    descargar_ui();
    descargar_texturas_jugador();
    CloseWindow();
    return 0;
}
