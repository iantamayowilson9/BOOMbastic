#ifndef PARTIDA_H
#define PARTIDA_H

// Gestiona las fases pedagogicas, el marco arcade, el HUD acoplado a los
// recuadros negros, el reloj digital de la tnt y la pantalla de fin.

#include "definis.h"
#include "assets.h"
#include "archivos.h"
#include "fisicas.h"
#include "minijuego.h"
#include <stdio.h>
#include <string.h>

// ==================================================== CONFIGURACION

ConfigNivel cargar_config_nivel(int nivel)
{
    static const ConfigNivel fases[] = {
        {0, ENTEROS, 20, 230.0f, 0.40f, false},
        {0, MINUTOS, 45, 320.0f, 0.35f, true},
        {0, HORAS, 60, 420.0f, 0.32f, true}};

    int i = 0;
    if (nivel <= 3)
        i = 0;
    else if (nivel <= 6)
        i = 1;
    else
        i = 2;

    ConfigNivel c = fases[i];
    c.nivel = nivel;
    prob_bomba = 20; // Por defecto

    if (i == 0)
    {
        c.meta = 15 + nivel * 5;
        c.vel_frutas = 230.0f + nivel * 30.0f;
    }
    else if (i == 1)
    {
        int rel = nivel - 3;
        c.meta = 30 + rel * 15;
        c.vel_frutas = 320.0f + rel * 35.0f;
    }
    else
    {
        int horas = (nivel - 6) + 1; // 2, 3, 4 en los niveles 7, 8, 9
        if (horas <= 4)
        {
            c.fase = HORAS;
            c.meta = horas * 60;
            c.vel_frutas = 420.0f + (nivel - 6) * 30.0f;
        }
        else
        {
            // Ya pasamos las 4 horas: fases aleatorias, mas bombas y mas rapido
            c.fase = (Fase)GetRandomValue(0, 2);
            c.permite_negativos = true;
            prob_bomba = 40;
            if (c.fase == ENTEROS)
                c.meta = 30 + GetRandomValue(0, 5) * 5;   // 30 a 55
            else if (c.fase == MINUTOS)
                c.meta = 45 + GetRandomValue(0, 4) * 15;  // 45 a 105
            else
                c.meta = 4 * 60;                          // 4 horas
            c.vel_frutas = 520.0f + (nivel - 10) * 25.0f;
            c.intervalo = 0.26f;
        }
    }
    return c;
}

// ==================================================== INICIO Y FIN

// Llena el suelo con frutas pegadas de izquierda a derecha
void sembrar_piso()
{
    float x = VP_X + 50.0f;
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        int idx = hueco_fruta();
        if (idx == -1)
            break;
        frutas[idx].activa = true;
        frutas[idx].radio = 42.0f;
        frutas[idx].vel = config.vel_frutas;
        frutas[idx].pos.x = x + frutas[idx].radio;
        frutas[idx].pos.y = SUELO_Y - frutas[idx].radio;
        valores_fruta(idx);
        x += frutas[idx].radio * 2.0f + 4.0f; // Bien pegadas
    }
}

void iniciar_partida()
{
    nivel_actual = 1;
    valor_actual = 0;
    puntos = 0;
    combo = 0;
    metas_logradas = 0;
    pausa = false;
    tiempo_partida = TIEMPO_PARTIDA;
    tiempo_aparicion = 0.0f;
    tiempo_gracia = TIEMPO_GRACIA;
    reloj_juego = 0.0f;
    tiempo_ultimo_combo = 0.0f;
    parpadeo_puntaje = 0.0f;
    velocidad_extra = 0.0f;
    scroll_fondo = 0.0f;
    config = cargar_config_nivel(nivel_actual);

    chango.tam = (Vector2){90, 90};
    chango.pos = (Vector2){VP_X + VP_ANCHO / 2.0f - 45.0f, VP_Y + 20.0f};
    chango.vel_y = 0;
    chango.vida = 3;
    chango.clavando = false;
    chango.invulnerable = 0;
    chango.estado = ARRIBA;

    for (int i = 0; i < MAX_FRUTAS; i++)
        frutas[i].activa = false;
    for (int i = 0; i < MAX_PARTICULAS; i++)
        particulas[i].activa = false;
    for (int i = 0; i < MAX_TEXTOS; i++)
        textos[i].activa = false;

    // El piso arranca lleno para que el chango no caiga en vacio
    sembrar_piso();

    // Reiniciar la musica desde el principio (tambien al morir y reiniciar)
    if (musica_lista(musica_minijuego))
        StopMusicStream(musica_minijuego);
    if (musica_lista(musica_partida))
    {
        StopMusicStream(musica_partida);
        PlayMusicStream(musica_partida);
    }

    estado_juego = JUGANDO;
}

void iniciar_fin_juego(bool por_tiempo)
{
    gano_por_tiempo = por_tiempo;
    nombre_jugador[0] = '\0';
    largo_nombre = 0;
    record_guardado = false;
    sonar(sfx_dano, 0.8f);
    estado_juego = FIN;
}

// ==================================================== LOGICA DE PARTIDA

void actualizar_partida(float dt)
{
    if (musica_lista(musica_partida))
        UpdateMusicStream(musica_partida);

    // Periodo de gracia: el chango flota mientras se llena el piso de frutas
    if (tiempo_gracia > 0)
    {
        tiempo_gracia -= dt;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            chango.pos.x -= VEL_LATERAL * dt;
            chango.estado = IZQUIERDA;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            chango.pos.x += VEL_LATERAL * dt;
            chango.estado = DERECHA;
        }
        if (chango.pos.x < VP_X)
            chango.pos.x = VP_X;
        if (chango.pos.x > VP_X + VP_ANCHO - chango.tam.x)
            chango.pos.x = VP_X + VP_ANCHO - chango.tam.x;
        chango.vel_y = 0;
        chango.clavando = false;
        actualizar_textos(dt);
        return;
    }

    reloj_juego += dt;
    scroll_fondo += (config.vel_frutas + velocidad_extra) * dt * 0.4f;

    tiempo_partida -= dt;
    if (tiempo_partida <= 0)
    {
        tiempo_partida = 0;
        iniciar_fin_juego(true);
        return;
    }

    actualizar_jugador(dt);

    // Nacen frutas con el tiempo
    tiempo_aparicion += dt;
    if (tiempo_aparicion >= config.intervalo)
    {
        tiempo_aparicion = 0.0f;
        crear_fruta();
    }
    actualizar_frutas(dt);
    revisar_colisiones();

    // Revisar si ya llegue a la meta del nivel
    if (valor_actual < 0)
        valor_actual = 0;
    if (valor_actual >= config.meta)
    {
        puntos += 100;
        metas_logradas++;
        nivel_actual++;
        valor_actual = 0;
        combo = 0;
        config = cargar_config_nivel(nivel_actual);
        sonar(sfx_victoria, 1.0f);
        crear_texto_flotante((Vector2){VP_X + VP_ANCHO / 2.0f, VP_Y + 80}, "NIVEL!", GOLD);
        // El minijuego sale cada ocho metas correctas
        if (metas_logradas % 8 == 0)
        {
            lanzar_minijuego_bananas();
            return;
        }
    }

    if (chango.vida <= 0)
    {
        iniciar_fin_juego(false);
        return;
    }

    if (parpadeo_puntaje > 0)
        parpadeo_puntaje -= dt;
    actualizar_particulas(dt);
    actualizar_textos(dt);

    // Tecla extra para probar el minijuego a mano
    if (IsKeyPressed(KEY_B))
        lanzar_minijuego_bananas();
}

// ==================================================== DIBUJO

// Posiciones del hud sobre el marco. Ajusta estos numeros si tu marco
// tiene los recuadros negros en otro lugar.
#define TIMER_X 670.0f
#define TIMER_Y 95.0f
#define INF_Y 694.0f   // Renglon del nivel al centro (un poco mas arriba)
#define VIDAS_X 250.0f
#define VIDAS_Y 695.0f
#define NIVEL_X 668.0f
#define COMBO_X 1050.0f
#define COMBO_Y 695.0f

// Fondo de jungla que cubre toda la pantalla y se desplaza horizontalmente
void dibujar_fondo()
{
    if (tex_fondo.width <= 1)
    {
        DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){200, 235, 200, 255});
        return;
    }
    float ancho = (float)tex_fondo.width;
    float desfase = scroll_fondo;
    while (desfase >= ancho)
        desfase -= ancho;
    for (float x = -desfase; x < V_ANCHO; x += ancho)
    {
        Rectangle origen = {0, 0, ancho, (float)tex_fondo.height};
        Rectangle destino = {x, 0, ancho, (float)V_ALTO};
        DrawTexturePro(tex_fondo, origen, destino, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

// Objetivo gigante al centro, blanco con borde negro grueso
void dibujar_objetivo_gigante()
{
    char objetivo[64];
    if (config.fase == ENTEROS)
        sprintf(objetivo, "OBJETIVO: %d", config.meta);
    else if (config.fase == MINUTOS)
        sprintf(objetivo, "OBJETIVO: %d MIN", config.meta);
    else
        sprintf(objetivo, "OBJETIVO: %d HRS", config.meta / 60);

    texto_contorno(fuente_titulo, objetivo, VP_X + VP_ANCHO / 2.0f,
                   VP_Y + VP_ALTO / 2.0f, 84, 3, WHITE, BLACK, 3);
}

void dibujar_frutas()
{
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;

        // Anillo blanco si me acerca a la meta, dorado si me deja justo
        if (frutas[i].valor > 0 && (valor_actual + frutas[i].valor) <= config.meta)
        {
            for (int k = 3; k <= 8; k++)
                DrawCircleLines((int)frutas[i].pos.x, (int)frutas[i].pos.y,
                                frutas[i].radio + k, RAYWHITE);
            if ((valor_actual + frutas[i].valor) == config.meta)
                for (int k = 9; k <= 16; k++)
                    DrawCircleLines((int)frutas[i].pos.x, (int)frutas[i].pos.y,
                                    frutas[i].radio + k, GOLD);
        }

        Texture2D t = tex_fruta[frutas[i].tipo];
        Rectangle origen = {0, 0, (float)t.width, (float)t.height};
        Rectangle destino = {frutas[i].pos.x - frutas[i].radio,
                             frutas[i].pos.y - frutas[i].radio,
                             frutas[i].radio * 2, frutas[i].radio * 2};
        DrawTexturePro(t, origen, destino, (Vector2){0, 0}, 0.0f, WHITE);

        // El numero de la fruta encima
        char etiqueta[16];
        if (frutas[i].valor >= 0)
            sprintf(etiqueta, "+%d", frutas[i].valor);
        else
            sprintf(etiqueta, "%d", frutas[i].valor);
        texto_contorno(fuente_hud, etiqueta, frutas[i].pos.x,
                       frutas[i].pos.y - frutas[i].radio - 18, 28, 1,
                       (frutas[i].valor >= 0) ? WHITE : RED, BLACK, 1);
    }
}

// Hud flotante vinculado al chango: cuanto lleva
void dibujar_llevas()
{
    char texto[32];
    if (config.fase == ENTEROS)
        sprintf(texto, "LLEVAS: %d", valor_actual);
    else if (config.fase == MINUTOS)
        sprintf(texto, "LLEVAS: %d MIN", valor_actual);
    else
        sprintf(texto, "LLEVAS: %dh %02dm", valor_actual / 60, valor_actual % 60);

    float cx = chango.pos.x + chango.tam.x / 2.0f;
    float cy = chango.pos.y - 32.0f;
    Color color = (parpadeo_puntaje > 0) ? GREEN : RAYWHITE;
    texto_contorno(fuente_hud, texto, cx, cy, 34, 1, color, BLACK, 2);
}

// Dibuja el marco fisico, o barras de respaldo si falta la textura
void dibujar_marco()
{
    if (marco_ok)
    {
        Rectangle origen = {0, 0, (float)tex_marco.width, (float)tex_marco.height};
        Rectangle destino = {0, 0, (float)V_ANCHO, (float)V_ALTO};
        DrawTexturePro(tex_marco, origen, destino, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        // Marco de respaldo: barras negras arriba y abajo con recuadros
        DrawRectangle(0, 0, V_ANCHO, VP_Y, BLACK);
        DrawRectangle(0, VP_Y + VP_ALTO, V_ANCHO, V_ALTO - (VP_Y + VP_ALTO), BLACK);
        DrawRectangle(0, 0, VP_X, V_ALTO, BLACK);
        DrawRectangle(VP_X + VP_ANCHO, 0, V_ANCHO - (VP_X + VP_ANCHO), V_ALTO, BLACK);
        // Cuadro de la tnt
        DrawRectangle(V_ANCHO / 2 - 110, 35, 220, 70, (Color){25, 0, 0, 255});
        DrawRectangleLines(V_ANCHO / 2 - 110, 35, 220, 70, RED);
        DrawText("TNT", V_ANCHO / 2 - 28, 8, 24, RED);
    }
}

// Reloj digital de la tnt y los textos del hud inferior
void dibujar_hud_acoplado()
{
    // Reloj digital MM:SS dentro de la tnt alta
    int total = (int)tiempo_partida;
    char reloj[8];
    sprintf(reloj, "%02d:%02d", total / 60, total % 60);
    Color color_reloj = (tiempo_partida < 15.0f) ? RED : (Color){255, 80, 60, 255};
    texto_centrado(fuente_digital, reloj, TIMER_X, TIMER_Y, 50, 2, color_reloj);

    // Etiquetas de respaldo si no hay marco real
    if (!marco_ok)
    {
        texto_centrado(fuente_hud, "NIVEL:", NIVEL_X - 110, INF_Y, 24, 1, RAYWHITE);
        texto_centrado(fuente_hud, "COMBO:", COMBO_X - 120, COMBO_Y, 24, 1, RAYWHITE);
    }

    // Vidas a la izquierda
    char vidas[20];
    sprintf(vidas, "Vidas: x%d", chango.vida);
    texto_centrado(fuente_hud, vidas, VIDAS_X, VIDAS_Y, 26, 1, RED);

    // Nivel al centro
    char nivel[16];
    sprintf(nivel, "%d", nivel_actual);
    texto_centrado(fuente_hud, nivel, NIVEL_X, INF_Y, 30, 1, GOLD);

    // Combo a la derecha
    char texto_combo[8];
    sprintf(texto_combo, "x%d", combo);
    texto_centrado(fuente_hud, texto_combo, COMBO_X, COMBO_Y, 30, 1,
                   (combo >= COMBO_MAX) ? GOLD : RAYWHITE);
}

void dibujar_partida()
{
    dibujar_fondo(); // Pantalla completa, detras del marco

    BeginScissorMode(VP_X, VP_Y, VP_ANCHO, VP_ALTO);
    dibujar_objetivo_gigante();
    dibujar_frutas();
    dibujar_chango();
    dibujar_llevas();
    dibujar_particulas();
    dibujar_textos();

    // Aviso del periodo de gracia con cuenta regresiva
    if (tiempo_gracia > 0)
    {
        char cuenta[8];
        sprintf(cuenta, "%d", (int)tiempo_gracia + 1);
        texto_contorno(fuente_titulo, cuenta, VP_X + VP_ANCHO / 2.0f,
                       VP_Y + 140, 90, 2, YELLOW, BLACK, 3);
        texto_contorno(fuente_hud, "PREPARATE", VP_X + VP_ANCHO / 2.0f,
                       VP_Y + 220, 30, 1, WHITE, BLACK, 1);
    }
    EndScissorMode();

    dibujar_marco();
    dibujar_hud_acoplado();
}

void dibujar_minijuego()
{
    dibujar_fondo(); // Pantalla completa, detras del marco

    BeginScissorMode(VP_X, VP_Y, VP_ANCHO, VP_ALTO);
    texto_contorno(fuente_titulo, "BANANAS!", VP_X + VP_ANCHO / 2.0f,
                   VP_Y + 70, 70, 2, YELLOW, BLACK, 2);
    dibujar_bananas();
    chango.estado = ARRIBA;
    dibujar_chango();
    dibujar_particulas();
    EndScissorMode();

    dibujar_marco();

    // Reloj del minijuego en la tnt
    int t = (int)tiempo_minijuego;
    char reloj[8];
    sprintf(reloj, "00:%02d", t);
    texto_centrado(fuente_digital, reloj, TIMER_X, TIMER_Y, 50, 2, YELLOW);

    char marca[24];
    sprintf(marca, "x%d", bananas_recolectadas);
    texto_centrado(fuente_hud, marca, COMBO_X, COMBO_Y, 30, 1, GOLD);
}

// ==================================================== PANTALLA DE FIN

void actualizar_fin()
{
    if (!record_guardado)
    {
        int c = GetCharPressed();
        while (c > 0)
        {
            if (es_letra(c) && largo_nombre < MAX_NOMBRE)
            {
                nombre_jugador[largo_nombre] = (char)c;
                largo_nombre++;
                nombre_jugador[largo_nombre] = '\0';
            }
            c = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && largo_nombre > 0)
        {
            largo_nombre--;
            nombre_jugador[largo_nombre] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (largo_nombre == 0)
                strcpy(nombre_jugador, "JUGADOR");
            insertar_record(nombre_jugador, puntos);
            record_guardado = true;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
            iniciar_partida(); // Volver a jugar directo, sin menu
    }
}

void dibujar_fin()
{
    DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){30, 10, 10, 255});
    texto_centrado(fuente_hud, "GAME OVER", V_ANCHO / 2.0f, 170, 90, 2, RED);

    const char *motivo = gano_por_tiempo ? "SE ACABO EL TIEMPO" : "TE QUEDASTE SIN VIDAS";
    texto_centrado(fuente_hud, motivo, V_ANCHO / 2.0f, 290, 30, 1, ORANGE);

    char res[64];
    sprintf(res, "NIVEL %d   PUNTOS %d", nivel_actual, puntos);
    texto_centrado(fuente_hud, res, V_ANCHO / 2.0f, 360, 32, 1, YELLOW);

    if (!record_guardado)
    {
        texto_centrado(fuente_hud, "ESCRIBE TU NOMBRE (SOLO LETRAS)", V_ANCHO / 2.0f, 460, 26, 1, RAYWHITE);
        Rectangle caja = {V_ANCHO / 2.0f - 250, 510, 500, 70};
        DrawRectangleRec(caja, (Color){20, 20, 30, 255});
        DrawRectangleLinesEx(caja, 4, RAYWHITE);
        const char *cursor = ((int)(GetTime() * 2) % 2 == 0) ? "_" : " ";
        char linea[40];
        sprintf(linea, "%s%s", nombre_jugador, cursor);
        texto_centrado(fuente_hud, linea, V_ANCHO / 2.0f, 528, 36, 1, GOLD);
        texto_centrado(fuente_hud, "[ENTER] GUARDAR RECORD", V_ANCHO / 2.0f, 630, 24, 1, LIGHTGRAY);
    }
    else
    {
        texto_centrado(fuente_hud, "RECORD GUARDADO", V_ANCHO / 2.0f, 500, 32, 1, GREEN);
        texto_centrado(fuente_hud, "[ENTER] JUGAR DE NUEVO", V_ANCHO / 2.0f, 600, 26, 1, LIGHTGRAY);
    }
}

#endif
