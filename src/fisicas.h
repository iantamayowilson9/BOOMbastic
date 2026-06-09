#ifndef FISICAS_H
#define FISICAS_H

// Toda la logica de movimiento del chango, el desfile de frutas en fila
// india, las colisiones, el rebote elastico, el combo y las particulas.

#include "definis.h"
#include "assets.h"
#include <stdio.h>
#include <string.h>

// ==================================================== PARTICULAS DE JUGO

void crear_particulas(Vector2 origen, Color color, int cantidad)
{
    int hechas = 0;
    for (int i = 0; i < MAX_PARTICULAS && hechas < cantidad; i++)
    {
        if (particulas[i].activa)
            continue;
        particulas[i].activa = true;
        particulas[i].pos = origen;
        particulas[i].vel = (Vector2){(float)GetRandomValue(-260, 260),
                                      (float)GetRandomValue(-420, -120)};
        particulas[i].tamano = (float)GetRandomValue(5, 11);
        particulas[i].vida = 1.0f;
        particulas[i].color = color;
        hechas++;
    }
}

void actualizar_particulas(float dt)
{
    for (int i = 0; i < MAX_PARTICULAS; i++)
    {
        if (!particulas[i].activa)
            continue;
        particulas[i].vel.y += GRAVEDAD_JUGO * dt;
        particulas[i].pos.x += particulas[i].vel.x * dt;
        particulas[i].pos.y += particulas[i].vel.y * dt;
        particulas[i].vida -= dt * 1.5f;
        if (particulas[i].vida <= 0)
            particulas[i].activa = false;
    }
}

void dibujar_particulas()
{
    for (int i = 0; i < MAX_PARTICULAS; i++)
    {
        if (!particulas[i].activa)
            continue;
        Color c = Fade(particulas[i].color, particulas[i].vida);
        Vector2 t = {particulas[i].tamano, particulas[i].tamano};
        DrawRectangleV(particulas[i].pos, t, c);
    }
}

// ==================================================== TEXTOS FLOTANTES

void crear_texto_flotante(Vector2 origen, const char texto[], Color color)
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (textos[i].activa)
            continue;
        textos[i].activa = true;
        textos[i].pos = origen;
        textos[i].vida = 1.0f;
        textos[i].color = color;
        strncpy(textos[i].texto, texto, sizeof(textos[i].texto) - 1);
        textos[i].texto[sizeof(textos[i].texto) - 1] = '\0';
        return;
    }
}

void actualizar_textos(float dt)
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (!textos[i].activa)
            continue;
        textos[i].pos.y -= 45.0f * dt;
        textos[i].vida -= dt;
        if (textos[i].vida <= 0)
            textos[i].activa = false;
    }
}

void dibujar_textos()
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (!textos[i].activa)
            continue;
        Color c = Fade(textos[i].color, textos[i].vida);
        texto_centrado(fuente_hud, textos[i].texto, textos[i].pos.x,
                       textos[i].pos.y, 26, 1, c);
    }
}

// ==================================================== FRUTAS

// Regresa el indice de una fruta libre o -1 si no hay
int hueco_fruta()
{
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            return i;
    }
    return -1;
}

// Calcula donde aparece una fruta nueva sin encimarse con la ultima
float entrada_fruta(float radio)
{
    float x = VP_X + VP_ANCHO + radio; // Fuera de la vista a la derecha
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;
        float borde = frutas[i].pos.x + frutas[i].radio + radio + 6.0f;
        if (borde > x)
            x = borde;
    }
    return x;
}

// Asigna valor, color y tipo a la fruta. A veces es bomba
void valores_fruta(int indice)
{
    int enteros[4] = {10, 5, 2, 1};
    int minutos[4] = {60, 30, 15, 10};

    bool es_bomba = config.permite_negativos && (GetRandomValue(0, 100) < prob_bomba);
    if (es_bomba)
    {
        frutas[indice].valor = -minutos[GetRandomValue(0, 3)];
        frutas[indice].tipo = 4;
        frutas[indice].color = DARKGRAY;
    }
    else
    {
        int tipo = GetRandomValue(0, 3);
        frutas[indice].tipo = tipo;
        frutas[indice].valor = (config.fase == ENTEROS) ? enteros[tipo] : minutos[tipo];
        Color colores[4] = {GREEN, YELLOW, PURPLE, RED};
        frutas[indice].color = colores[tipo];
    }
}

void crear_fruta()
{
    int i = hueco_fruta();
    if (i == -1)
        return;
    frutas[i].activa = true;
    frutas[i].radio = 42.0f;
    frutas[i].vel = config.vel_frutas;
    frutas[i].pos.x = entrada_fruta(frutas[i].radio);
    frutas[i].pos.y = SUELO_Y - frutas[i].radio;
    valores_fruta(i);
}

// Empuje mecanico rigido en cadena para que ninguna fruta se traslape
void resolver_fila_india()
{
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;
        for (int j = 0; j < MAX_FRUTAS; j++)
        {
            if (i == j || !frutas[j].activa)
                continue;
            // La fruta j va adelante si su x es menor
            if (frutas[j].pos.x < frutas[i].pos.x)
            {
                float minimo = frutas[j].pos.x + frutas[j].radio + frutas[i].radio;
                if (frutas[i].pos.x < minimo)
                    frutas[i].pos.x = minimo;
            }
        }
    }
}

void actualizar_frutas(float dt)
{
    float vel_cinta = velocidad_extra; // Bono temporal de velocidad
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;
        frutas[i].pos.x -= (frutas[i].vel + vel_cinta) * dt;
        // Sale por la izquierda y se recicla sin castigo
        if (frutas[i].pos.x + frutas[i].radio < VP_X)
            frutas[i].activa = false;
    }
    resolver_fila_india();

    // El bono de velocidad se desvanece poco a poco
    if (velocidad_extra > 0)
    {
        velocidad_extra -= 40.0f * dt;
        if (velocidad_extra < 0)
            velocidad_extra = 0;
    }
}

// ==================================================== COMBO

// Sube o reinicia el combo segun la ventana de tiempo
void subir_combo()
{
    if (reloj_juego - tiempo_ultimo_combo <= VENTANA_COMBO)
    {
        if (combo < COMBO_MAX)
            combo++;
    }
    else
    {
        combo = 1;
    }
    tiempo_ultimo_combo = reloj_juego;
    if (combo >= COMBO_MAX)
        sonar(sfx_combomax, 1.0f);
}

// ==================================================== JUGADOR

void actualizar_jugador(float dt)
{
    bool movio_lateral = false;

    // Movimiento horizontal a los lados
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        chango.pos.x -= VEL_LATERAL * dt;
        chango.estado = IZQUIERDA;
        movio_lateral = true;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        chango.pos.x += VEL_LATERAL * dt;
        chango.estado = DERECHA;
        movio_lateral = true;
    }

    // No dejar que salga del viewport por los lados
    if (chango.pos.x < VP_X)
        chango.pos.x = VP_X;
    if (chango.pos.x > VP_X + VP_ANCHO - chango.tam.x)
        chango.pos.x = VP_X + VP_ANCHO - chango.tam.x;

    // Espacio en el aire hace una picada vertical pesada
    if (IsKeyPressed(KEY_SPACE))
    {
        chango.vel_y = PICADA;
        chango.clavando = true;
        chango.estado = APLASTAR;
    }
    if (chango.vel_y < 0)
        chango.clavando = false;

    // Gravedad
    chango.vel_y += GRAVEDAD * dt;
    chango.pos.y += chango.vel_y * dt;

    // Textura por defecto al subir si no se movio a los lados
    if (chango.clavando)
        chango.estado = APLASTAR;
    else if (!movio_lateral)
        chango.estado = ARRIBA;

    // Control anti techo: anula la subida y fuerza una caida suave
    if (chango.pos.y < VP_Y)
    {
        chango.pos.y = VP_Y;
        if (chango.vel_y < 0)
            chango.vel_y = 60.0f;
    }

    if (chango.invulnerable > 0)
        chango.invulnerable -= dt;
}

// Aplica el rebote y los efectos al aplastar una fruta buena
void rebotar_en_fruta(int i, bool vino_de_picada)
{
    bool me_paso = (valor_actual + frutas[i].valor) > config.meta;

    if (me_paso)
    {
        combo = 0;
        crear_texto_flotante(frutas[i].pos, "TE PASASTE!", ORANGE);
    }
    else
    {
        subir_combo();
        puntos += frutas[i].valor * combo;
        char texto[24];
        sprintf(texto, "+%d x%d", frutas[i].valor, combo);
        crear_texto_flotante(frutas[i].pos, texto,
                             (combo >= COMBO_MAX) ? GOLD : GREEN);
    }

    valor_actual += frutas[i].valor;
    parpadeo_puntaje = 0.35f;

    // Rebote elastico, mas fuerte si venia de una picada
    if (vino_de_picada)
    {
        chango.vel_y = REBOTE * BONO_PICADA;
        velocidad_extra += 60.0f;
        if (velocidad_extra > 320.0f)
            velocidad_extra = 320.0f;
        sonar(sfx_aplastar, 0.9f + GetRandomValue(0, 20) / 100.0f);
        crear_particulas(frutas[i].pos, frutas[i].color, GetRandomValue(15, 20));
    }
    else
    {
        chango.vel_y = REBOTE;
        sonar(sfx_rebote, 1.0f);
        crear_particulas(frutas[i].pos, frutas[i].color, 12);
    }

    chango.clavando = false;
    chango.estado = ARRIBA;
    frutas[i].activa = false;
}

// Revisa todos los choques del chango contra frutas y bombas
void revisar_colisiones()
{
    Rectangle caja = {chango.pos.x, chango.pos.y, chango.tam.x, chango.tam.y};
    float pies = chango.pos.y + chango.tam.y;

    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;
        if (!CheckCollisionCircleRec(frutas[i].pos, frutas[i].radio, caja))
            continue;

        if (frutas[i].valor < 0)
        {
            // Bomba: pierde vida y rompe el combo
            if (chango.invulnerable <= 0)
            {
                chango.vida--;
                combo = 0;
                chango.invulnerable = INVULN;
                chango.vel_y = REBOTE;
                chango.clavando = false;
                chango.estado = ARRIBA;
                sonar(sfx_bomba, 1.0f);
                sonar(sfx_dano, 1.0f);
                crear_particulas(frutas[i].pos, RED, 18);
                crear_texto_flotante(frutas[i].pos, "BOMBA! -1", RED);
                frutas[i].activa = false;
            }
        }
        else
        {
            // Fruta buena: solo cuenta si cae sobre ella
            bool cayendo = chango.vel_y > 0;
            if (cayendo && pies < frutas[i].pos.y + frutas[i].radio)
                rebotar_en_fruta(i, chango.clavando);
        }
    }

    // Tocar el suelo vacio cuesta una vida
    if (pies >= SUELO_Y)
    {
        chango.pos.y = SUELO_Y - chango.tam.y;
        if (chango.invulnerable <= 0)
        {
            chango.vida--;
            combo = 0;
            chango.invulnerable = INVULN;
            sonar(sfx_dano, 1.0f);
            crear_texto_flotante((Vector2){chango.pos.x + chango.tam.x / 2,
                                           chango.pos.y},
                                 "SUELO! -1", ORANGE);
        }
        chango.vel_y = REBOTE;
        chango.clavando = false;
        chango.estado = ARRIBA;
    }
}

// ==================================================== DIBUJO DEL CHANGO

void dibujar_chango()
{
    // Parpadeo durante la invulnerabilidad
    if (chango.invulnerable > 0 && ((int)(chango.invulnerable * 12) % 2 == 0))
        return;

    Texture2D actual = tex_chango_arriba;
    switch (chango.estado)
    {
    case IZQUIERDA:
        actual = tex_chango_izquierda;
        break;
    case DERECHA:
        actual = tex_chango_derecha;
        break;
    case APLASTAR:
        actual = tex_chango_aplastar;
        break;
    case ARRIBA:
    default:
        actual = tex_chango_arriba;
        break;
    }

    Rectangle origen = {0, 0, (float)actual.width, (float)actual.height};
    Rectangle destino = {chango.pos.x, chango.pos.y, chango.tam.x, chango.tam.y};
    Color tinte = chango.clavando ? (Color){255, 180, 180, 255} : WHITE;
    DrawTexturePro(actual, origen, destino, (Vector2){0, 0}, 0.0f, tinte);
}

#endif
