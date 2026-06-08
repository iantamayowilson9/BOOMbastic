/* ===========================================================================
   MAIN.CPP  -  ARCHIVO PRINCIPAL DEL JUEGO BOOMBASTIC
   AQUI SOLO ESTA EL MENU DE LLAMADAS: ABRE LA VENTANA, CORRE EL CICLO
   Y SEGUN LA PANTALLA LLAMA A LAS FUNCIONES QUE VIVEN EN game.h
=========================================================================== */
#include "game.h"

int main()
{
    // ABRO LA VENTANA Y FIJO 60 CUADROS POR SEGUNDO
    InitWindow(ANCHO, ALTO, "Boombastic");
    SetTargetFPS(60);

    // PREPARO LOS BOTONES, LEO LOS RECORDS Y CARGO LOS CUADROS DE LA INTRO
    iniciar_botones_menu();
    cargar_records();
    cargar_cuadros_intro();
    cargar_ui();

    // CICLO PRINCIPAL DEL JUEGO
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // PRIMERO ACTUALIZO SEGUN LA PANTALLA EN LA QUE ESTOY
        if (pantalla_actual == PANTALLA_INTRO)        actualizar_intro(dt);
        else if (pantalla_actual == PANTALLA_MENU)    actualizar_menu();
        else if (pantalla_actual == PANTALLA_RECORDS) actualizar_records();
        else if (pantalla_actual == PANTALLA_JUEGO)   actualizar_juego(dt);
        else if (pantalla_actual == PANTALLA_FIN)     actualizar_fin();

        // DESPUES DIBUJO SEGUN LA PANTALLA
        BeginDrawing();
        ClearBackground(BLACK);

        if (pantalla_actual == PANTALLA_INTRO)        dibujar_intro();
        else if (pantalla_actual == PANTALLA_MENU)    dibujar_menu();
        else if (pantalla_actual == PANTALLA_RECORDS) dibujar_records();
        else if (pantalla_actual == PANTALLA_JUEGO)   dibujar_juego();
        else if (pantalla_actual == PANTALLA_FIN)     dibujar_fin();

        // EL FILTRO RETRO SE PINTA ENCIMA DE TODO
        dibujar_filtro();

        EndDrawing();
    }

    // LIBERO LAS IMAGENES Y CIERRO LA VENTANA AL SALIR
    descargar_cuadros_intro();
    descargar_ui();
    CloseWindow();
    return 0;
}
