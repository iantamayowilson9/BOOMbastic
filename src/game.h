#ifndef GAME_H
#define GAME_H

// Cabecera central. Une todos los modulos y expone las cuatro funciones
// que usa main.cpp: inicializar, actualizar, dibujar y cerrar.

#include "definis.h"
#include "assets.h"
#include "archivos.h"
#include "fisicas.h"
#include "minijuego.h"
#include "partida.h"

void inicializar_juego()
{
    InitWindow(V_ANCHO, V_ALTO, "Boombastic");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // ESC ya no cierra: lo usamos para pausar
    InitAudioDevice();

    cargar_assets();
    cargar_records();
    iniciar_partida(); // Tambien arranca la musica de partida
}

void actualizar_juego(float dt)
{
    // ESC pausa y reanuda durante el juego (no en la pantalla de fin)
    if (IsKeyPressed(KEY_ESCAPE) && estado_juego != FIN)
        pausa = !pausa;
    if (pausa)
        return;

    switch (estado_juego)
    {
    case JUGANDO:
        actualizar_partida(dt);
        break;
    case MINIJUEGO:
        actualizar_minijuego(dt);
        break;
    case FIN:
        actualizar_fin();
        break;
    }
}

void dibujar_juego()
{
    BeginDrawing();
    ClearBackground(BLACK);

    switch (estado_juego)
    {
    case JUGANDO:
        dibujar_partida();
        break;
    case MINIJUEGO:
        dibujar_minijuego();
        break;
    case FIN:
        dibujar_fin();
        break;
    }

    // Cortina de pausa encima de todo
    if (pausa)
    {
        DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){0, 0, 0, 170});
        texto_contorno(fuente_titulo, "PAUSA", V_ANCHO / 2.0f, V_ALTO / 2.0f - 30,
                       100, 3, WHITE, BLACK, 3);
        texto_centrado(fuente_hud, "[ESC] continuar", V_ANCHO / 2.0f,
                       V_ALTO / 2.0f + 70, 30, 1, RAYWHITE);
    }

    EndDrawing();
}

void cerrar_juego()
{
    descargar_assets();
    CloseAudioDevice();
    CloseWindow();
}

#endif
