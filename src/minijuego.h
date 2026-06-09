#ifndef MINIJUEGO_H
#define MINIJUEGO_H

// Lluvia de bananas que dura 15 segundos exactos. Detiene la cinta de
// frutas, congela la columna del chango y cambia la musica.

#include "definis.h"
#include "assets.h"
#include "fisicas.h"

// Crea una banana nueva que cae desde arriba del viewport
void soltar_banana()
{
    for (int i = 0; i < MAX_BANANAS; i++)
    {
        if (bananas[i].activa)
            continue;
        bananas[i].activa = true;
        bananas[i].radio = 28.0f;
        bananas[i].pos.x = (float)GetRandomValue(VP_X + 40, VP_X + VP_ANCHO - 40);
        bananas[i].pos.y = VP_Y - 30.0f;
        bananas[i].vel = (float)GetRandomValue(320, 520);
        return;
    }
}

// Arranca el minijuego: limpia la pantalla y cambia la musica
void lanzar_minijuego_bananas()
{
    estado_juego = MINIJUEGO;
    tiempo_minijuego = TIEMPO_MINIJUEGO;
    bananas_recolectadas = 0;
    tiempo_spawn_banana = 0.0f;

    for (int i = 0; i < MAX_BANANAS; i++)
        bananas[i].activa = false;

    chango.x_congelada = chango.pos.x;
    chango.estado = ARRIBA;

    if (musica_lista(musica_partida))
        PauseMusicStream(musica_partida); // Se pausa para reanudarla luego
    if (musica_lista(musica_minijuego))
        PlayMusicStream(musica_minijuego);
}

// Regresa a la partida principal de inmediato
void terminar_minijuego()
{
    for (int i = 0; i < MAX_BANANAS; i++)
        bananas[i].activa = false;

    puntos += bananas_recolectadas * 5; // Bono por cada banana

    if (musica_lista(musica_minijuego))
        StopMusicStream(musica_minijuego);
    if (musica_lista(musica_partida))
        ResumeMusicStream(musica_partida); // Continua donde se quedo

    estado_juego = JUGANDO;
}

void actualizar_minijuego(float dt)
{
    if (musica_lista(musica_minijuego))
        UpdateMusicStream(musica_minijuego);

    tiempo_minijuego -= dt;
    if (tiempo_minijuego <= 0)
    {
        tiempo_minijuego = 0;
        terminar_minijuego();
        return;
    }

    // El chango sigue rebotando pero puede moverse para atrapar bananas
    actualizar_jugador(dt);

    // El piso del minijuego solo rebota, no quita vida
    float pies = chango.pos.y + chango.tam.y;
    if (pies >= SUELO_Y)
    {
        chango.pos.y = SUELO_Y - chango.tam.y;
        chango.vel_y = REBOTE;
        chango.clavando = false;
        chango.estado = ARRIBA;
    }

    // Lluvia constante de bananas, dos por tanda para que caigan mas
    tiempo_spawn_banana += dt;
    if (tiempo_spawn_banana >= 0.20f)
    {
        tiempo_spawn_banana = 0.0f;
        soltar_banana();
        soltar_banana();
    }

    Rectangle caja = {chango.pos.x, chango.pos.y, chango.tam.x, chango.tam.y};
    for (int i = 0; i < MAX_BANANAS; i++)
    {
        if (!bananas[i].activa)
            continue;
        bananas[i].pos.y += bananas[i].vel * dt;

        if (CheckCollisionCircleRec(bananas[i].pos, bananas[i].radio, caja))
        {
            bananas[i].activa = false;
            bananas_recolectadas++;
            puntos += 2;
            sonar(sfx_aplastar, 1.05f);
            crear_particulas(bananas[i].pos, YELLOW, 8);
        }
        else if (bananas[i].pos.y - bananas[i].radio > SUELO_Y)
        {
            bananas[i].activa = false;
        }
    }

    actualizar_particulas(dt);
}

void dibujar_bananas()
{
    for (int i = 0; i < MAX_BANANAS; i++)
    {
        if (!bananas[i].activa)
            continue;
        Rectangle origen = {0, 0, (float)tex_fruta[1].width, (float)tex_fruta[1].height};
        Rectangle destino = {bananas[i].pos.x - bananas[i].radio,
                             bananas[i].pos.y - bananas[i].radio,
                             bananas[i].radio * 2, bananas[i].radio * 2};
        DrawTexturePro(tex_fruta[1], origen, destino, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

#endif
