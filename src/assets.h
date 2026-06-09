#ifndef ASSETS_H
#define ASSETS_H

// Carga unica de texturas, fuentes y sonidos al iniciar el juego.
// Cada recurso tiene un respaldo automatico para que nada crashee
// aunque falte el archivo en disco.

#include "definis.h"

// =============================================================== TEXTURAS
Texture2D tex_marco;
Texture2D tex_chango_arriba;
Texture2D tex_chango_aplastar;
Texture2D tex_chango_izquierda;
Texture2D tex_chango_derecha;
Texture2D tex_fondo;
Texture2D tex_particula;
Texture2D tex_fruta[5]; // sandia platano uva manzana bomba

bool marco_ok = false; // Verdadero solo si cargo el marco real

// =============================================================== FUENTES
Font fuente_hud;     // EARTHBOUND para el hud
Font fuente_digital; // DS-DIGIB para el reloj de la tnt
Font fuente_titulo;  // UPHEAVAL para el objetivo gigante

bool hud_ok = false;
bool digital_ok = false;
bool titulo_ok = false;

// =============================================================== AUDIO
Sound sfx_aplastar;
Sound sfx_rebote;
Sound sfx_dano;
Sound sfx_bomba;
Sound sfx_victoria;
Sound sfx_combomax;

Music musica_partida;
Music musica_minijuego;

// =============================================== VALIDACION POR VERSION
// raylib cambio los nombres de IsTextureReady/IsFontReady en la 5.5.
// Para que el juego compile en cualquier version revisamos los campos
// de las estructuras en lugar de llamar a esas funciones.

bool textura_lista(Texture2D t)
{
    return t.id > 0;
}

// Una fuente propia tiene textura distinta a la del sistema
bool fuente_propia(Font f)
{
    return f.texture.id > 0 && f.texture.id != GetFontDefault().texture.id;
}

bool sonido_listo(Sound s)
{
    return s.frameCount > 0;
}

bool musica_lista(Music m)
{
    return m.frameCount > 0;
}

// ==================================================== AYUDAS DE RESPALDO

// Carga una textura y si falla genera un cuadro de color plano en memoria
Texture2D cargar_textura_segura(const char ruta[], Color respaldo)
{
    Texture2D t = LoadTexture(ruta);
    if (!textura_lista(t))
    {
        Image img = GenImageColor(96, 96, respaldo);
        t = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    return t;
}

// Carga una fuente con tamano y avisa por bandera si quedo lista
Font cargar_fuente_segura(const char ruta[], int tam, bool ok[])
{
    Font f = LoadFontEx(ruta, tam, 0, 0);
    if (fuente_propia(f))
    {
        ok[0] = true;
        return f;
    }
    ok[0] = false;
    return GetFontDefault();
}

// Reproduce un sonido solo si quedo bien cargado
void sonar(Sound s, float pitch)
{
    if (sonido_listo(s))
    {
        SetSoundPitch(s, pitch);
        PlaySound(s);
    }
}

// ==================================================== DIBUJO DE TEXTO

// Dibuja texto centrado con cualquier fuente
void texto_centrado(Font fuente, const char texto[], float cx, float cy,
                    float tam, float esp, Color color)
{
    Vector2 medida = MeasureTextEx(fuente, texto, tam, esp);
    Vector2 base = {cx - medida.x / 2.0f, cy - medida.y / 2.0f};
    DrawTextEx(fuente, texto, base, tam, esp, color);
}

// Dibuja texto con un borde grueso por desplazamiento simetrico
void texto_contorno(Font fuente, const char texto[], float cx, float cy,
                    float tam, float esp, Color relleno, Color borde, int grosor)
{
    Vector2 medida = MeasureTextEx(fuente, texto, tam, esp);
    Vector2 base = {cx - medida.x / 2.0f, cy - medida.y / 2.0f};
    for (int dx = -grosor; dx <= grosor; dx++)
    {
        for (int dy = -grosor; dy <= grosor; dy++)
        {
            if (dx != 0 || dy != 0)
            {
                DrawTextEx(fuente, texto, (Vector2){base.x + dx, base.y + dy},
                           tam, esp, borde);
            }
        }
    }
    DrawTextEx(fuente, texto, base, tam, esp, relleno);
}

// ==================================================== CARGA Y DESCARGA

void cargar_assets()
{
    // Marco fisico de la cabina arcade
    tex_marco = LoadTexture("assets/textures/interfaz/marco_pantalla.png");
    marco_ok = textura_lista(tex_marco);
    if (!marco_ok)
    {
        Image vacio = GenImageColor(96, 96, BLANK);
        tex_marco = LoadTextureFromImage(vacio);
        UnloadImage(vacio);
    }

    // Personaje
    tex_chango_arriba = cargar_textura_segura("assets/textures/personaje/chango_arriba.png", (Color){90, 140, 230, 255});
    tex_chango_aplastar = cargar_textura_segura("assets/textures/personaje/chango_aplastar.png", (Color){200, 60, 60, 255});
    tex_chango_izquierda = cargar_textura_segura("assets/textures/personaje/chango_izquierda.png", (Color){120, 170, 240, 255});
    tex_chango_derecha = cargar_textura_segura("assets/textures/personaje/chango_derecha.png", (Color){120, 170, 240, 255});

    // Ambiente
    tex_fondo = cargar_textura_segura("assets/textures/ambiente/fondo_jungla.png", (Color){120, 190, 130, 255});
    tex_particula = cargar_textura_segura("assets/textures/ambiente/particula_jugo.png", WHITE);

    // Frutas
    tex_fruta[0] = cargar_textura_segura("assets/textures/frutas/fruta_sandia.png", (Color){40, 170, 70, 255});
    tex_fruta[1] = cargar_textura_segura("assets/textures/frutas/fruta_platano.png", (Color){235, 205, 60, 255});
    tex_fruta[2] = cargar_textura_segura("assets/textures/frutas/fruta_uva.png", (Color){150, 70, 200, 255});
    tex_fruta[3] = cargar_textura_segura("assets/textures/frutas/fruta_manzana.png", (Color){210, 50, 50, 255});
    tex_fruta[4] = cargar_textura_segura("assets/textures/frutas/fruta_bomba.png", (Color){45, 45, 45, 255});

    // Fuentes
    fuente_hud = cargar_fuente_segura("assets/fuentes/EARTHBOUND.TTF", 48, &hud_ok);
    fuente_digital = cargar_fuente_segura("assets/fuentes/DS-DIGIB.TTF", 64, &digital_ok);
    // El objetivo gigante usa upheavtt cargada en grande para que se vea nitida
    fuente_titulo = cargar_fuente_segura("assets/fuentes/upheavtt.ttf", 170, &titulo_ok);
    if (!titulo_ok)
        fuente_titulo = cargar_fuente_segura("assets/fuentes/UPHEAVAL.TTF", 170, &titulo_ok);

    // Sonidos
    sfx_aplastar = LoadSound("assets/sfx/aplastar.wav");
    sfx_rebote = LoadSound("assets/sfx/sfx_rebote.wav");
    sfx_dano = LoadSound("assets/sfx/sfx_dano.wav");
    sfx_bomba = LoadSound("assets/sfx/sfx_bomba_alerta.wav");
    sfx_victoria = LoadSound("assets/sfx/sfx_victoria.wav");
    sfx_combomax = LoadSound("assets/sfx/sfx_combomax.wav");

    // Musica
    musica_partida = LoadMusicStream("assets/music/music_partida.wav");
    musica_minijuego = LoadMusicStream("assets/music/music_minijuego.wav");
}

void descargar_assets()
{
    UnloadTexture(tex_marco);
    UnloadTexture(tex_chango_arriba);
    UnloadTexture(tex_chango_aplastar);
    UnloadTexture(tex_chango_izquierda);
    UnloadTexture(tex_chango_derecha);
    UnloadTexture(tex_fondo);
    UnloadTexture(tex_particula);
    for (int i = 0; i < 5; i++)
        UnloadTexture(tex_fruta[i]);

    // Solo descargo fuentes propias, nunca la del sistema
    if (hud_ok)
        UnloadFont(fuente_hud);
    if (digital_ok)
        UnloadFont(fuente_digital);
    if (titulo_ok)
        UnloadFont(fuente_titulo);

    if (sonido_listo(sfx_aplastar))
        UnloadSound(sfx_aplastar);
    if (sonido_listo(sfx_rebote))
        UnloadSound(sfx_rebote);
    if (sonido_listo(sfx_dano))
        UnloadSound(sfx_dano);
    if (sonido_listo(sfx_bomba))
        UnloadSound(sfx_bomba);
    if (sonido_listo(sfx_victoria))
        UnloadSound(sfx_victoria);
    if (sonido_listo(sfx_combomax))
        UnloadSound(sfx_combomax);

    if (musica_lista(musica_partida))
        UnloadMusicStream(musica_partida);
    if (musica_lista(musica_minijuego))
        UnloadMusicStream(musica_minijuego);
}

#endif
