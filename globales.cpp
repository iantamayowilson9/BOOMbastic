#include "globales.h"
#include <stdio.h>
#include <string.h>

Pantalla pantalla_actual = INTRO;
Jugador chango;
Fruta frutas[MAX_FRUTAS];
Particula particulas[MAX_PARTICULAS];
TextoFlotante textos[MAX_TEXTOS];
Record records[MAX_RECORDS];
ConfigNivel config;

int nivel_actual = 1;
int valor_actual = 0;
int puntos = 0;
float tiempo_aparicion = 0.0f;
float gravedad = GRAVEDAD_DEFECTO;
int combo = 0;
float tiempo_partida = TIEMPO_PARTIDA;
float parpadeo_puntaje = 0.0f;

char nombre_jugador[MAX_NOMBRE + 1] = "";
int largo_nombre = 0;
bool record_guardado = false;
bool filtro_activo = true;
int cuadro_intro = 0;
float tiempo_intro = 0.0f;
BotonMenu botones[3];

Texture2D intro_cuadros[CUADROS_INTRO];
bool intro_cargada = false;
Texture2D tex_logo;
Texture2D tex_boton_normal;
Texture2D tex_boton_hover;
bool ui_cargada = false;

Texture2D textura_chango_salto;
Texture2D textura_chango_izq;
Texture2D textura_chango_der;
Texture2D textura_chango_cayendo;

bool textura_chango_cargada = false;

void dibujar_texto_centrado(const char texto[], int centro_x, int y, int tam, Color color)
{
    int ancho_texto = MeasureText(texto, tam);
    DrawText(texto, centro_x - ancho_texto / 2, y, tam, color);
}

ConfigNivel cargar_config_nivel(int nivel)
{
    static const ConfigNivel config_fase[] = {
        // nivel, fase, meta, vel_frutas, intervalo, permite_negativos
        {0, ENTEROS, 20, 150.0f, 0.85f, false},
        {0, MINUTOS, 45, 245.0f, 0.70f, true},
        {0, HORAS, 60, 300.0f, 0.60f, true}};

    ConfigNivel c;
    int i_fase = 0;

    if (nivel <= 3)
    {
        i_fase = 0;
    }
    else
    {
        if (nivel <= 6)
        {
            i_fase = 1;
        }
        else
        {
            i_fase = 2;
        }
    }

    c = config_fase[i_fase];
    c.nivel = nivel;

    if (i_fase == 0)
    {
        c.meta = 15 + nivel * 5;
        c.vel_frutas = 130.f + nivel * 20.0f;
    }
    else
    {
        if (i_fase == 1)
        {
            int rel = nivel - 3;
            c.meta = 30 + rel * 15;
            c.vel_frutas = 220.0f + rel * 25.0f;
        }
        else
        {
            int horas = (nivel - 6) + 1;
            if (horas > 4)
                horas = 4;
            c.meta = horas * 60;
        }
    }

    return c;
}
