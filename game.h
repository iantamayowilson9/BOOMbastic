/* ===========================================================================
   GAME.H  -  AQUI VIVE TODO EL FUNCIONAMIENTO DEL JUEGO BOOMBASTIC
   EL ARCHIVO PRINCIPAL (main.cpp) SOLO LLAMA A ESTAS FUNCIONES.

   REGLAS DE ESTILO:
   - FUNCIONES CON PALABRAS COMPLETAS EN ESPANOL SEPARADAS POR GUION BAJO
   - COMENTARIOS EN MAYUSCULAS, SIN ACENTOS, COMO NOTAS PERSONALES
   - SIN APUNTADORES: USO INDICES Y CORCHETES EN VEZ DE *
=========================================================================== */
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------------
   NOMBRES DE LOS PNG QUE USARA EL JUEGO (CUANDO LOS AGREGUES):

   INTRO   -> assets/intro/frame_001.png ... frame_215.png   (YA FUNCIONAN)

   BOTONES -> assets/ui/boton_normal.png      (BOTON SIN MOUSE ENCIMA)
              assets/ui/boton_hover.png       (BOTON CON MOUSE ENCIMA)
   LOGO    -> assets/ui/logo.png              (EL TITULO BOOMBASTIC DEL MENU)
   FILTRO  -> assets/ui/filtro_lineas.png     (OPCIONAL, EL FILTRO RETRO)

   CHANGO  -> assets/sprites/chango.png        (QUIETO / REBOTANDO)
              assets/sprites/chango_clavado.png (CUANDO HACE EL CLAVADO)
   BOMBA   -> assets/sprites/bomba.png

   FRUTAS  -> assets/frutas/sandia.png    (VALE 10 / 60 MIN)
              assets/frutas/platano.png   (VALE 5  / 30 MIN)
              assets/frutas/uva.png       (VALE 2  / 15 MIN)
              assets/frutas/manzana.png   (VALE 1  / 10 MIN)
--------------------------------------------------------------------------- */

/* ===========================================================================
   1. NUMEROS FIJOS DEL JUEGO (MEDIDAS, FISICA Y DIFICULTAD)
=========================================================================== */
#define ANCHO              1600
#define ALTO                900
#define SUELO_Y             760     // LINEA DEL SUELO

#define MAX_FRUTAS           48
#define MAX_PARTICULAS      256
#define MAX_TEXTOS           16
#define MAX_RECORDS           5

#define TIEMPO_PARTIDA     120.0f   // DOS MINUTOS POR PARTIDA
#define COMBO_MAX             5      // EL COMBO LLEGA HASTA x5

#define ARCHIVO_RECORDS  "datos/records.dat"   // ARCHIVO BINARIO DE RECORDS
#define MAX_NOMBRE          12                  // LETRAS MAXIMAS DEL NOMBRE

// FISICA DEL CHANGO (REBOTA, SUBE Y BAJA MUY LENTO Y FLOTA)
#define REBOTE            -520.0f
#define CLAVADO            900.0f
#define VEL_LATERAL        440.0f
#define INVULN               0.7f   // SEGUNDOS SIN RECIBIR DANO TRAS PERDER VIDA

// GRAVEDAD QUE SE PUEDE SUBIR Y BAJAR EN PLENO JUEGO (BAJA = LENTO)
#define GRAVEDAD_DEFECTO   380.0f
#define GRAVEDAD_MIN       200.0f
#define GRAVEDAD_MAX      1800.0f
#define GRAVEDAD_PASO       80.0f

#define GRAVEDAD_JUGO     1400.0f   // GRAVEDAD APARTE PARA LAS GOTAS DE JUGO

// INTRO HECHA DE CUADROS REALES: assets/intro/frame_001.png ... frame_215.png
#define CUADROS_INTRO       215      // SON 215 CUADROS (frame_001 A frame_215)
#define DELAY_INTRO          0.04f   // CADA CUADRO DURA 0.04 SEG (SUBE O BAJA ESTE NUMERO)

/* ===========================================================================
   2. ENUMERACIONES Y MOLDES DE DATOS (STRUCTS)
=========================================================================== */

// LAS PANTALLAS POR LAS QUE PASA EL JUEGO
enum Pantalla {
    PANTALLA_INTRO,
    PANTALLA_MENU,
    PANTALLA_RECORDS,
    PANTALLA_JUEGO,
    PANTALLA_FIN
};

// LAS FASES DE MATEMATICAS QUE VA APRENDIENDO EL NINO
enum Fase {
    FASE_ENTEROS,   // SUMAS SIMPLES        -> OBJETIVO: 20
    FASE_MINUTOS,   // MINUTOS Y RESTAS     -> OBJETIVO: 45 MIN
    FASE_HORAS      // HORAS (POR DENTRO MINUTOS) -> OBJETIVO: 2 HORAS
};

// EL CHANGO QUE CONTROLA EL JUGADOR
struct Jugador {
    Vector2 posicion;     // ESQUINA DE ARRIBA A LA IZQUIERDA
    Vector2 tamano;
    float   velocidad_y;
    bool    clavando;     // TRUE MIENTRAS HACE UN CLAVADO CON ESPACIO
    float   invulnerable; // CUENTA REGRESIVA DE INVULNERABILIDAD
    int     vidas;
};

// UNA FRUTA QUE DESFILA POR EL SUELO
struct Fruta {
    Vector2 posicion;     // CENTRO DEL CIRCULO
    float   radio;
    float   velocidad;
    int     valor;        // VALOR DE MATEMATICAS (NEGATIVO = BOMBA)
    bool    activa;
    Color   color;
    int     tipo;         // 0 SANDIA 1 PLATANO 2 UVA 3 MANZANA 4 BOMBA
};

// UNA GOTA DE JUGO QUE SALTA AL APLASTAR
struct Particula {
    Vector2 posicion;
    Vector2 velocidad;
    float   tamano;
    float   vida;         // VA DE 1.0 A 0.0
    Color   color;
    bool    activa;
};

// UN TEXTO QUE SUBE Y SE DESVANECE (EJEMPLO: +5 x3)
struct TextoFlotante {
    Vector2 posicion;
    char    texto[16];
    float   vida;
    Color   color;
    bool    activa;
};

// UN RECORD GUARDADO EN EL ARCHIVO
struct Record {
    char nombre[MAX_NOMBRE + 1];
    int  puntos;
};

// UN BOTON DEL MENU
struct BotonMenu {
    Rectangle area;
    char      etiqueta[16];
};

// LA CONFIGURACION QUE SALE DE CADA NIVEL
struct ConfigNivel {
    int   nivel;
    Fase  fase;
    int   meta;             // META POR DENTRO: ENTERO O MINUTOS
    float vel_frutas;
    float intervalo;        // SEGUNDOS ENTRE FRUTA Y FRUTA
    bool  permite_negativos;
};

/* ===========================================================================
   3. VARIABLES GLOBALES (EL ESTADO COMPLETO DEL JUEGO)
=========================================================================== */
Pantalla      pantalla_actual = PANTALLA_INTRO;

Jugador       chango;
Fruta         frutas[MAX_FRUTAS];
Particula     particulas[MAX_PARTICULAS];
TextoFlotante textos[MAX_TEXTOS];
Record        records[MAX_RECORDS];

ConfigNivel   config;
int           nivel_actual   = 1;
int           valor_actual   = 0;   // LO QUE LLEVO HACIA LA META
int           puntos         = 0;   // PUNTAJE PARA LOS RECORDS
float         tiempo_aparicion = 0.0f;
float         gravedad       = GRAVEDAD_DEFECTO;
int           combo          = 0;
float         tiempo_partida = TIEMPO_PARTIDA;
float         parpadeo_puntaje = 0.0f;  // PARA QUE EL PUNTAJE FLOTANTE REACCIONE

// ESCRIBIR EL NOMBRE EN LA PANTALLA DE FIN
char          nombre_jugador[MAX_NOMBRE + 1] = "";
int           largo_nombre   = 0;
bool          record_guardado = false;

bool          filtro_activo  = true;  // FILTRO DE LINEAS RETRO
int           cuadro_intro    = 0;     // EN QUE CUADRO DE LA INTRO VAMOS
float         tiempo_intro    = 0.0f;
BotonMenu     botones[3];

// LAS IMAGENES DE LA INTRO (frame_001.png ... frame_215.png)
Texture2D     intro_cuadros[CUADROS_INTRO];
bool          intro_cargada   = false;

// LAS IMAGENES DEL MENU (logo y botones)
Texture2D     tex_logo;
Texture2D     tex_boton_normal;
Texture2D     tex_boton_hover;
bool          ui_cargada      = false;

/* ===========================================================================
   4. DECLARACIONES (PARA PODER LLAMARLAS EN CUALQUIER ORDEN)
=========================================================================== */
void dibujar_texto_centrado(const char texto[], int centro_x, int y, int tam, Color color);
ConfigNivel cargar_configuracion_nivel(int nivel);
void iniciar_botones_menu();
void iniciar_partida();
void iniciar_fin_juego();
bool es_letra(int c);

void cargar_records();
void guardar_records();
void insertar_record(const char nombre[], int nuevos_puntos);

void crear_fruta();
void dibujar_frutas();
void crear_particulas(Vector2 origen, Color color, int cantidad);
void crear_texto_flotante(Vector2 origen, const char texto[], Color color);
void actualizar_particulas(float dt);
void actualizar_textos_flotantes(float dt);
void dibujar_particulas();
void dibujar_textos_flotantes();

void dibujar_chango();
void dibujar_interfaz();
void dibujar_puntaje_flotante();
void dibujar_filtro();

void actualizar_juego(float dt);
void dibujar_juego();
void cargar_cuadros_intro();
void descargar_cuadros_intro();
void cargar_ui();
void descargar_ui();
void actualizar_intro(float dt);
void dibujar_intro();
void actualizar_menu();
void dibujar_menu();
void actualizar_records();
void dibujar_records();
void actualizar_fin();
void dibujar_fin();

/* ===========================================================================
   5. UTILIDAD: DIBUJAR TEXTO CENTRADO
=========================================================================== */
void dibujar_texto_centrado(const char texto[], int centro_x, int y, int tam, Color color)
{
    int ancho_texto = MeasureText(texto, tam);
    DrawText(texto, centro_x - ancho_texto / 2, y, tam, color);
}

/* ===========================================================================
   6. DIFICULTAD: DECIDE FASE, META Y VELOCIDAD SEGUN EL NIVEL
=========================================================================== */
ConfigNivel cargar_configuracion_nivel(int nivel)
{
    ConfigNivel c;
    c.nivel = nivel;

    if (nivel <= 3) {
        // FASE 1: SUMAS DE ENTEROS SIMPLES
        c.fase              = FASE_ENTEROS;
        c.meta              = 15 + nivel * 5;          // 20, 25, 30
        c.vel_frutas        = 130.0f + nivel * 20.0f;
        c.intervalo         = 0.85f;
        c.permite_negativos = false;
    }
    else if (nivel <= 6) {
        // FASE 2: MINUTOS Y RESTAS
        c.fase              = FASE_MINUTOS;
        c.meta              = 30 + (nivel - 3) * 15;   // 45, 60, 75 MIN
        c.vel_frutas        = 220.0f + (nivel - 3) * 25.0f;
        c.intervalo         = 0.7f;
        c.permite_negativos = true;
    }
    else {
        // FASE 3: HORAS COMPLETAS (POR DENTRO SON MINUTOS)
        c.fase              = FASE_HORAS;
        int horas           = 1 + (nivel - 6);
        if (horas > 4) horas = 4;
        c.meta              = horas * 60;              // 60, 120, ...
        c.vel_frutas        = 300.0f;
        c.intervalo         = 0.6f;
        c.permite_negativos = true;
    }
    return c;
}

/* ===========================================================================
   7. INICIALIZACIONES
=========================================================================== */

// ARMA LOS TRES BOTONES DEL MENU CENTRADOS
void iniciar_botones_menu()
{
    float bw = 360, bh = 70, cx = ANCHO / 2.0f - bw / 2.0f;

    botones[0].area = (Rectangle){ cx, 410, bw, bh };
    strcpy(botones[0].etiqueta, "JUGAR");

    botones[1].area = (Rectangle){ cx, 510, bw, bh };
    strcpy(botones[1].etiqueta, "RECORDS");

    botones[2].area = (Rectangle){ cx, 610, bw, bh };
    strcpy(botones[2].etiqueta, "SALIR");
}

// DEJA TODO LISTO PARA EMPEZAR UNA PARTIDA NUEVA
void iniciar_partida()
{
    nivel_actual     = 1;
    valor_actual     = 0;
    puntos           = 0;
    tiempo_aparicion = 0.0f;
    gravedad         = GRAVEDAD_DEFECTO;
    combo            = 0;
    tiempo_partida   = TIEMPO_PARTIDA;
    config           = cargar_configuracion_nivel(nivel_actual);

    // EL CHANGO ARRANCA EN EL AIRE Y CAE HACIA LAS FRUTAS
    chango.tamano       = (Vector2){ 150, 190 };
    chango.posicion     = (Vector2){ ANCHO / 2.0f - chango.tamano.x / 2.0f, 60 };
    chango.velocidad_y  = 0;
    chango.clavando     = false;
    chango.invulnerable = 0;
    chango.vidas        = 3;

    for (int i = 0; i < MAX_FRUTAS; i++)    frutas[i].activa     = false;
    for (int i = 0; i < MAX_PARTICULAS; i++) particulas[i].activa = false;
    for (int i = 0; i < MAX_TEXTOS; i++)    textos[i].activa     = false;

    // LLENO EL SUELO CON VARIAS FRUTAS PARA QUE TENGA DONDE REBOTAR
    for (int i = 0; i < 8; i++) {
        crear_fruta();
        if (frutas[i].activa) frutas[i].posicion.x = 120.0f + i * 200.0f;
    }
}

// PREPARA LA PANTALLA DE FIN PARA QUE EL JUGADOR ESCRIBA SU NOMBRE
void iniciar_fin_juego()
{
    nombre_jugador[0] = '\0';
    largo_nombre      = 0;
    record_guardado   = false;
    pantalla_actual   = PANTALLA_FIN;
}

// REVISA SI UN CARACTER ES LETRA (SOLO LETRAS EN EL NOMBRE)
bool es_letra(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/* ===========================================================================
   8. RECORDS GUARDADOS EN ARCHIVO BINARIO (datos/records.dat)
=========================================================================== */

// LEE LA TABLA DEL ARCHIVO. SI NO EXISTE, LA DEJA VACIA
void cargar_records()
{
    FILE *archivo = fopen(ARCHIVO_RECORDS, "rb");
    if (archivo == NULL) {
        for (int i = 0; i < MAX_RECORDS; i++) {
            strcpy(records[i].nombre, "---");
            records[i].puntos = 0;
        }
        return;
    }
    fread(records, sizeof(Record), MAX_RECORDS, archivo);
    fclose(archivo);
}

// ESCRIBE TODA LA TABLA EN EL ARCHIVO
void guardar_records()
{
    FILE *archivo = fopen(ARCHIVO_RECORDS, "wb");
    if (archivo == NULL) return;   // SI NO SE PUEDE ESCRIBIR NO PASA NADA
    fwrite(records, sizeof(Record), MAX_RECORDS, archivo);
    fclose(archivo);
}

// METE UN PUNTAJE EN ORDEN SI ENTRA AL TOP, RECORRE EL RESTO Y GUARDA
void insertar_record(const char nombre[], int nuevos_puntos)
{
    int lugar = -1;
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (nuevos_puntos > records[i].puntos) { lugar = i; break; }
    }
    if (lugar == -1) return;   // NO ALCANZO EL TOP

    // EMPUJO HACIA ABAJO DESDE EL ULTIMO HASTA EL LUGAR
    for (int i = MAX_RECORDS - 1; i > lugar; i--) {
        records[i] = records[i - 1];
    }
    strncpy(records[lugar].nombre, nombre, MAX_NOMBRE);
    records[lugar].nombre[MAX_NOMBRE] = '\0';
    records[lugar].puntos = nuevos_puntos;

    guardar_records();
}

/* ===========================================================================
   9. FRUTAS: NACEN POR LA DERECHA Y CAMINAN A LA IZQUIERDA
=========================================================================== */

// CREA UNA FRUTA EN EL PRIMER ESPACIO LIBRE
void crear_fruta()
{
    int indice = -1;
    for (int i = 0; i < MAX_FRUTAS; i++) {
        if (!frutas[i].activa) { indice = i; break; }
    }
    if (indice == -1) return;

    frutas[indice].activa    = true;
    frutas[indice].radio     = 96.0f;
    frutas[indice].velocidad = config.vel_frutas;

    bool negativa = config.permite_negativos && (GetRandomValue(0, 100) < 22);

    if (negativa) {
        // BOMBA: TOCARLA QUITA UNA VIDA, NO SE PUEDE REBOTAR EN ELLA
        int opciones[] = { 5, 10, 15 };
        frutas[indice].valor = -opciones[GetRandomValue(0, 2)];
        frutas[indice].tipo  = 4;
        frutas[indice].color = DARKGRAY;
    } else {
        // CADA TIPO DE FRUTA TIENE SU PROPIO VALOR FIJO
        // ASI EL NINO APRENDE: LA SANDIA SIEMPRE VALE LO MISMO
        int tipo = GetRandomValue(0, 3);
        frutas[indice].tipo = tipo;

        int val_enteros[4] = { 10, 5,  2,  1  };  // SANDIA PLATANO UVA MANZANA
        int val_minutos[4] = { 60, 30, 15, 10 };

        if (config.fase == FASE_ENTEROS) frutas[indice].valor = val_enteros[tipo];
        else                             frutas[indice].valor = val_minutos[tipo];

        if (tipo == 0)      frutas[indice].color = GREEN;   // SANDIA
        else if (tipo == 1) frutas[indice].color = YELLOW;  // PLATANO
        else if (tipo == 2) frutas[indice].color = PURPLE;  // UVA
        else                frutas[indice].color = RED;     // MANZANA
    }

    // NACE PEGADA AL BORDE DERECHO, APOYADA EN EL SUELO
    frutas[indice].posicion = (Vector2){ ANCHO + frutas[indice].radio,
                                         SUELO_Y - frutas[indice].radio };
}

// DIBUJA TODAS LAS FRUTAS ACTIVAS CON SU NUMERO Y SUS ANILLOS DE AYUDA
void dibujar_frutas()
{
    for (int i = 0; i < MAX_FRUTAS; i++) {
        if (!frutas[i].activa) continue;

        DrawCircleV(frutas[i].posicion, frutas[i].radio, frutas[i].color);
        DrawCircleLines((int)frutas[i].posicion.x, (int)frutas[i].posicion.y,
                        frutas[i].radio, BLACK);

        // ANILLO BLANCO: FRUTAS QUE ME ACERCAN A LA META SIN PASARME
        // ANILLO DORADO: SI ME DEJAN JUSTO EN LA META
        if (frutas[i].valor > 0 && (valor_actual + frutas[i].valor) <= config.meta) {
            for (int k = 4; k <= 12; k++)
                DrawCircleLines((int)frutas[i].posicion.x, (int)frutas[i].posicion.y,
                                frutas[i].radio + k, RAYWHITE);
            if ((valor_actual + frutas[i].valor) == config.meta)
                for (int k = 14; k <= 22; k++)
                    DrawCircleLines((int)frutas[i].posicion.x, (int)frutas[i].posicion.y,
                                    frutas[i].radio + k, GOLD);
        }

        // EL NUMERO GRANDE ENCIMA DE LA FRUTA
        char etiqueta[16];
        if (frutas[i].valor >= 0) sprintf(etiqueta, "+%d", frutas[i].valor);
        else                      sprintf(etiqueta, "%d",  frutas[i].valor);
        dibujar_texto_centrado(etiqueta, (int)frutas[i].posicion.x,
                               (int)frutas[i].posicion.y - 24, 48,
                               (frutas[i].valor >= 0) ? BLACK : RAYWHITE);
    }
}

/* ===========================================================================
   10. EFECTOS: GOTAS DE JUGO Y TEXTOS QUE SUBEN
=========================================================================== */

// LANZA VARIAS GOTAS DE JUGO DESDE UN PUNTO
void crear_particulas(Vector2 origen, Color color, int cantidad)
{
    int hechas = 0;
    for (int i = 0; i < MAX_PARTICULAS && hechas < cantidad; i++) {
        if (particulas[i].activa) continue;
        particulas[i].activa    = true;
        particulas[i].posicion  = origen;
        particulas[i].velocidad = (Vector2){ (float)GetRandomValue(-260, 260),
                                             (float)GetRandomValue(-420, -120) };
        particulas[i].tamano = (float)GetRandomValue(4, 10);
        particulas[i].vida   = 1.0f;
        particulas[i].color  = color;
        hechas++;
    }
}

// CREA UN TEXTO FLOTANTE EN EL PRIMER ESPACIO LIBRE
void crear_texto_flotante(Vector2 origen, const char texto[], Color color)
{
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (textos[i].activa) continue;
        textos[i].activa   = true;
        textos[i].posicion = origen;
        textos[i].vida     = 1.0f;
        textos[i].color    = color;
        strncpy(textos[i].texto, texto, sizeof(textos[i].texto) - 1);
        textos[i].texto[sizeof(textos[i].texto) - 1] = '\0';
        return;
    }
}

// MUEVE LAS GOTAS Y LAS VA APAGANDO
void actualizar_particulas(float dt)
{
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (!particulas[i].activa) continue;
        particulas[i].velocidad.y += GRAVEDAD_JUGO * dt;
        particulas[i].posicion.x  += particulas[i].velocidad.x * dt;
        particulas[i].posicion.y  += particulas[i].velocidad.y * dt;
        particulas[i].vida        -= dt * 1.5f;
        if (particulas[i].vida <= 0) particulas[i].activa = false;
    }
}

// SUBE LOS TEXTOS Y LOS VA DESVANECIENDO
void actualizar_textos_flotantes(float dt)
{
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (!textos[i].activa) continue;
        textos[i].posicion.y -= 40.0f * dt;
        textos[i].vida       -= dt;
        if (textos[i].vida <= 0) textos[i].activa = false;
    }
}

// DIBUJA LAS GOTAS COMO CUADRITOS
void dibujar_particulas()
{
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (!particulas[i].activa) continue;
        Color c = Fade(particulas[i].color, particulas[i].vida);
        DrawRectangleV(particulas[i].posicion,
                       (Vector2){ particulas[i].tamano, particulas[i].tamano }, c);
    }
}

// DIBUJA LOS TEXTOS FLOTANTES
void dibujar_textos_flotantes()
{
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (!textos[i].activa) continue;
        Color c = Fade(textos[i].color, textos[i].vida);
        dibujar_texto_centrado(textos[i].texto, (int)textos[i].posicion.x,
                               (int)textos[i].posicion.y, 28, c);
    }
}

/* ===========================================================================
   11. DIBUJO DEL CHANGO (FIGURAS, SIN IMAGENES POR AHORA)
   EN EL FUTURO: assets/sprites/monkey_idle.png
=========================================================================== */
void dibujar_chango()
{
    // PARPADEA MIENTRAS ES INVULNERABLE (ACABA DE PERDER UNA VIDA)
    if (chango.invulnerable > 0 && ((int)(chango.invulnerable * 12) % 2 == 0))
        return;

    float x = chango.posicion.x;
    float y = chango.posicion.y;
    float w = chango.tamano.x;
    float h = chango.tamano.y;

    Color cuerpo = chango.clavando ? (Color){ 150, 90, 40, 255 } : BROWN;

    DrawRectangleRounded((Rectangle){ x, y + h * 0.35f, w, h * 0.65f }, 0.4f, 8, cuerpo);
    DrawCircle((int)(x + w / 2), (int)(y + h * 0.25f), w * 0.42f, cuerpo);       // CABEZA
    DrawCircle((int)(x + w * 0.1f), (int)(y + h * 0.18f), w * 0.16f, cuerpo);    // OREJA IZQ
    DrawCircle((int)(x + w * 0.9f), (int)(y + h * 0.18f), w * 0.16f, cuerpo);    // OREJA DER
    DrawCircle((int)(x + w / 2), (int)(y + h * 0.30f), w * 0.26f, BEIGE);        // CARA
    DrawCircle((int)(x + w * 0.38f), (int)(y + h * 0.20f), 4, BLACK);            // OJO
    DrawCircle((int)(x + w * 0.62f), (int)(y + h * 0.20f), 4, BLACK);            // OJO

    if (chango.clavando)
        DrawText("BOOM!", (int)(x - 6), (int)(y - 26), 24, RED);
}

/* ===========================================================================
   12. INTERFAZ: OBJETIVO ARRIBA, RELOJ Y VIDAS EN ESQUINAS, COMBO ABAJO
=========================================================================== */
void dibujar_interfaz()
{
    // OBJETIVO FIJO, GRANDE, ARRIBA Y AL CENTRO
    char objetivo[64];
    if (config.fase == FASE_ENTEROS)
        sprintf(objetivo, "OBJETIVO: %d", config.meta);
    else if (config.fase == FASE_MINUTOS)
        sprintf(objetivo, "OBJETIVO: %d MIN", config.meta);
    else {
        int horas = config.meta / 60;
        sprintf(objetivo, "OBJETIVO: %d HORA%s", horas, (horas == 1) ? "" : "S");
    }
    int ancho_obj = MeasureText(objetivo, 52);
    DrawRectangle(ANCHO / 2 - ancho_obj / 2 - 24, 12, ancho_obj + 48, 70, Fade(BLACK, 0.45f));
    dibujar_texto_centrado(objetivo, ANCHO / 2, 24, 52, YELLOW);

    // ESQUINA SUPERIOR IZQUIERDA: RELOJ Y GRAVEDAD
    int segundos = (int)tiempo_partida;
    Color color_reloj = (tiempo_partida < 15.0f) ? RED : RAYWHITE;
    DrawCircle(40, 42, 18, GOLD);
    DrawCircleLines(40, 42, 18, BLACK);
    DrawText(TextFormat("%d:%02d", segundos / 60, segundos % 60), 70, 20, 44, color_reloj);
    DrawText(TextFormat("GRAVEDAD: %.0f", gravedad), 24, 74, 20, SKYBLUE);

    // ESQUINA SUPERIOR DERECHA: NIVEL Y VIDAS
    DrawText(TextFormat("NIVEL %d", nivel_actual), ANCHO - 220, 20, 30, GOLD);
    for (int i = 0; i < chango.vidas; i++)
        DrawCircle(ANCHO - 36 - i * 42, 78, 16, RED);

    // ABAJO AL CENTRO: BARRA DE COMBO
    dibujar_texto_centrado(TextFormat("COMBO  x%d", combo), ANCHO / 2, ALTO - 78, 28,
                           (combo >= COMBO_MAX) ? GOLD : RAYWHITE);
    int seg = 50, hueco = 6, alto_barra = 32;
    int ancho_total = COMBO_MAX * seg + (COMBO_MAX - 1) * hueco;
    int barra_x = ANCHO / 2 - ancho_total / 2;
    int barra_y = ALTO - 44;
    for (int i = 0; i < COMBO_MAX; i++) {
        Rectangle r = { (float)(barra_x + i * (seg + hueco)), (float)barra_y,
                        (float)seg, (float)alto_barra };
        DrawRectangleRec(r, (i < combo) ? GREEN : (Color){ 60, 60, 60, 255 });
        DrawRectangleLinesEx(r, 2, BLACK);
    }
}

// EL PUNTAJE ACTUAL VA FLOTANDO SOBRE LA CABEZA DEL CHANGO
// SI ACABO DE CAMBIAR SE PONE VERDE Y UN POCO MAS GRANDE
void dibujar_puntaje_flotante()
{
    char texto[32];
    if (config.fase == FASE_ENTEROS)      sprintf(texto, "%d", valor_actual);
    else if (config.fase == FASE_MINUTOS) sprintf(texto, "%d MIN", valor_actual);
    else                                  sprintf(texto, "%dh %02dm",
                                                  valor_actual / 60, valor_actual % 60);

    bool reaccion = (parpadeo_puntaje > 0);
    int   tam     = reaccion ? 34 : 28;
    Color color   = reaccion ? GREEN : RAYWHITE;

    int cx = (int)(chango.posicion.x + chango.tamano.x / 2);
    int y  = (int)(chango.posicion.y - 28 - (reaccion ? 4 : 0));

    int ancho_texto = MeasureText(texto, tam);
    DrawRectangle(cx - ancho_texto / 2 - 8, y - 4, ancho_texto + 16, tam + 8, Fade(BLACK, 0.45f));
    dibujar_texto_centrado(texto, cx, y, tam, color);
}

// FILTRO RETRO: LINEAS HORIZONTALES TENUES CADA 4 PIXELES
void dibujar_filtro()
{
    if (!filtro_activo) return;
    for (int y = 0; y < ALTO; y += 4)
        DrawRectangle(0, y, ANCHO, 1, ColorAlpha(BLACK, 0.15f));
}

/* ===========================================================================
   13. LA PARTIDA: TODA LA LOGICA DEL JUEGO
=========================================================================== */
void actualizar_juego(float dt)
{
    // EL RELOJ DE LA PARTIDA (DOS MINUTOS)
    tiempo_partida -= dt;
    if (tiempo_partida <= 0) {
        tiempo_partida = 0;
        iniciar_fin_juego();
        return;
    }

    // MOVER A LOS LADOS SIN SALIR DE LA PANTALLA
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) chango.posicion.x -= VEL_LATERAL * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) chango.posicion.x += VEL_LATERAL * dt;
    if (chango.posicion.x < 0) chango.posicion.x = 0;
    if (chango.posicion.x > ANCHO - chango.tamano.x)
        chango.posicion.x = ANCHO - chango.tamano.x;

    // SUBIR Y BAJAR LA GRAVEDAD CON LAS FLECHAS
    if (IsKeyDown(KEY_UP))   gravedad += GRAVEDAD_PASO * 40 * dt;
    if (IsKeyDown(KEY_DOWN)) gravedad -= GRAVEDAD_PASO * 40 * dt;
    if (IsKeyPressed(KEY_UP))   gravedad += GRAVEDAD_PASO;
    if (IsKeyPressed(KEY_DOWN)) gravedad -= GRAVEDAD_PASO;
    if (gravedad < GRAVEDAD_MIN) gravedad = GRAVEDAD_MIN;
    if (gravedad > GRAVEDAD_MAX) gravedad = GRAVEDAD_MAX;

    // CLAVADO CON ESPACIO PARA CAER MAS RAPIDO
    if (IsKeyPressed(KEY_SPACE)) {
        chango.velocidad_y = CLAVADO;
        chango.clavando    = true;
    }
    if (chango.velocidad_y < 0) chango.clavando = false;

    // FISICA HACIA ARRIBA Y ABAJO
    chango.velocidad_y += gravedad * dt;
    chango.posicion.y  += chango.velocidad_y * dt;
    if (chango.posicion.y < 0) {                 // TOPE DE ARRIBA
        chango.posicion.y = 0;
        if (chango.velocidad_y < 0) chango.velocidad_y = 0;
    }
    if (chango.invulnerable > 0) chango.invulnerable -= dt;

    Rectangle caja_chango = { chango.posicion.x, chango.posicion.y,
                              chango.tamano.x, chango.tamano.y };
    float pies = chango.posicion.y + chango.tamano.y;

    // VAN NACIENDO FRUTAS CON EL TIEMPO
    tiempo_aparicion += dt;
    if (tiempo_aparicion >= config.intervalo) {
        tiempo_aparicion = 0.0f;
        crear_fruta();
    }

    // FRUTAS: SE MUEVEN A LA IZQUIERDA Y SE REVISAN LOS CHOQUES
    for (int i = 0; i < MAX_FRUTAS; i++) {
        if (!frutas[i].activa) continue;

        frutas[i].posicion.x -= frutas[i].velocidad * dt;

        // SI SALE POR LA IZQUIERDA SE RECICLA SIN CASTIGO
        if (frutas[i].posicion.x + frutas[i].radio < 0) {
            frutas[i].activa = false;
            continue;
        }

        if (!CheckCollisionCircleRec(frutas[i].posicion, frutas[i].radio, caja_chango))
            continue;

        if (frutas[i].valor < 0) {
            // BOMBA: PIERDE VIDA Y SE ROMPE EL COMBO
            if (chango.invulnerable <= 0) {
                chango.vidas--;
                combo = 0;
                chango.invulnerable = INVULN;
                chango.velocidad_y  = REBOTE;
                crear_particulas(frutas[i].posicion, RED, 18);
                crear_texto_flotante(frutas[i].posicion, "BOMBA! -1", RED);
                frutas[i].activa = false;
            }
        } else {
            // FRUTA BUENA: SOLO REBOTA SI VIENE CAYENDO SOBRE ELLA
            bool cayendo = chango.velocidad_y > 0;
            if (cayendo && pies < frutas[i].posicion.y + frutas[i].radio) {

                bool me_paso = (valor_actual + frutas[i].valor) > config.meta;

                if (me_paso) {
                    // ME PASE DE LA META: PIERDO EL COMBO
                    combo = 0;
                    puntos += frutas[i].valor;
                    crear_texto_flotante(frutas[i].posicion, "TE PASASTE!", ORANGE);
                } else {
                    // SUBE EL COMBO Y MULTIPLICA LOS PUNTOS
                    if (combo < COMBO_MAX) combo++;
                    puntos += frutas[i].valor * combo;   // x1 HASTA x5
                    char texto[24];
                    sprintf(texto, "+%d x%d", frutas[i].valor, combo);
                    crear_texto_flotante(frutas[i].posicion, texto,
                                         (combo >= COMBO_MAX) ? GOLD : GREEN);
                }

                valor_actual    += frutas[i].valor;
                parpadeo_puntaje = 0.35f;
                chango.velocidad_y = REBOTE;
                chango.clavando    = false;
                crear_particulas(frutas[i].posicion, frutas[i].color, 14);
                frutas[i].activa = false;
            }
        }
    }

    // TOCAR EL SUELO: PIERDE VIDA, ROMPE COMBO Y REBOTA
    if (pies >= SUELO_Y) {
        chango.posicion.y = SUELO_Y - chango.tamano.y;
        if (chango.invulnerable <= 0) {
            chango.vidas--;
            combo = 0;
            chango.invulnerable = INVULN;
            crear_texto_flotante((Vector2){ chango.posicion.x + chango.tamano.x / 2,
                                            chango.posicion.y }, "SUELO! -1", ORANGE);
        }
        chango.velocidad_y = REBOTE;
        chango.clavando    = false;
    }

    // REVISAR SI YA LLEGUE A LA META DEL NIVEL
    if (valor_actual < 0) valor_actual = 0;
    if (valor_actual >= config.meta) {
        puntos += 100;
        nivel_actual++;
        valor_actual = 0;
        config = cargar_configuracion_nivel(nivel_actual);
        crear_texto_flotante((Vector2){ ANCHO / 2.0f, 200 }, "NIVEL!", GOLD);
    }

    // FIN DEL JUEGO POR QUEDARSE SIN VIDAS
    if (chango.vidas <= 0) {
        iniciar_fin_juego();
        return;
    }

    // EFECTOS Y TECLAS SUELTAS
    if (parpadeo_puntaje > 0) parpadeo_puntaje -= dt;
    actualizar_particulas(dt);
    actualizar_textos_flotantes(dt);

    if (IsKeyPressed(KEY_F))      filtro_activo = !filtro_activo;
    if (IsKeyPressed(KEY_R))      gravedad = GRAVEDAD_DEFECTO;   // REINICIA LA GRAVEDAD
    if (IsKeyPressed(KEY_ESCAPE)) pantalla_actual = PANTALLA_MENU;
}

// ARMA EL DIBUJO COMPLETO DE LA PARTIDA
void dibujar_juego()
{
    // CIELO, TIERRA Y CESPED
    DrawRectangle(0, 0, ANCHO, SUELO_Y, (Color){ 110, 180, 230, 255 });
    DrawRectangle(0, SUELO_Y, ANCHO, ALTO - SUELO_Y, (Color){ 90, 60, 35, 255 });
    DrawRectangle(0, SUELO_Y, ANCHO, 8, (Color){ 70, 140, 60, 255 });

    dibujar_frutas();
    dibujar_chango();
    dibujar_puntaje_flotante();
    dibujar_particulas();
    dibujar_textos_flotantes();
    dibujar_interfaz();
}

/* ===========================================================================
   14. INTRO HECHA DE CUADROS REALES (assets/intro/frame_001.png ... frame_215.png)
=========================================================================== */

// CARGO LAS 215 IMAGENES UNA SOLA VEZ (DESPUES DE ABRIR LA VENTANA)
void cargar_cuadros_intro()
{
    for (int i = 0; i < CUADROS_INTRO; i++) {
        // LOS ARCHIVOS VAN DE frame_001 A frame_215 (POR ESO i + 1 Y %03d)
        intro_cuadros[i] = LoadTexture(TextFormat("assets/intro/frame_%03d.png", i + 1));
    }
    intro_cargada = true;
}

// LIBERO LAS IMAGENES AL SALIR DEL JUEGO
void descargar_cuadros_intro()
{
    if (!intro_cargada) return;
    for (int i = 0; i < CUADROS_INTRO; i++)
        UnloadTexture(intro_cuadros[i]);
    intro_cargada = false;
}

void actualizar_intro(float dt)
{
    tiempo_intro += dt;
    // CADA CUADRO DURA DELAY_INTRO SEGUNDOS
    if (tiempo_intro >= DELAY_INTRO) {
        tiempo_intro = 0.0f;
        cuadro_intro++;
    }
    // SE SALTA CON ESPACIO O AL TERMINARSE LOS CUADROS
    if (cuadro_intro >= CUADROS_INTRO || IsKeyPressed(KEY_SPACE)) {
        cuadro_intro    = 0;
        pantalla_actual = PANTALLA_MENU;
    }
}

void dibujar_intro()
{
    DrawRectangle(0, 0, ANCHO, ALTO, BLACK);

    // DIBUJO EL CUADRO ACTUAL ESTIRADO A TODA LA PANTALLA
    if (intro_cargada && cuadro_intro < CUADROS_INTRO &&
        intro_cuadros[cuadro_intro].id != 0) {

        Texture2D cuadro = intro_cuadros[cuadro_intro];
        Rectangle origen  = { 0, 0, (float)cuadro.width, (float)cuadro.height };
        Rectangle destino = { 0, 0, (float)ANCHO, (float)ALTO };
        DrawTexturePro(cuadro, origen, destino, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        // SI NO ENCUENTRA LAS IMAGENES MUESTRO UN AVISO PARA DARME CUENTA
        dibujar_texto_centrado("FALTAN LOS CUADROS EN assets/intro", ANCHO / 2, 420, 30, RAYWHITE);
    }

    DrawText("[ESPACIO] saltar", 24, ALTO - 44, 22, RAYWHITE);
}

/* ===========================================================================
   15. MENU (LOGO Y BOTONES CON IMAGEN, O FIGURAS SI FALTAN LOS PNG)
=========================================================================== */

// CARGO EL LOGO Y LOS BOTONES (DESPUES DE ABRIR LA VENTANA)
void cargar_ui()
{
    tex_logo         = LoadTexture("assets/ui/logo.png");
    tex_boton_normal = LoadTexture("assets/ui/boton_normal.png");
    tex_boton_hover  = LoadTexture("assets/ui/boton_hover.png");
    ui_cargada = true;
}

// LIBERO LAS IMAGENES DEL MENU AL SALIR
void descargar_ui()
{
    if (!ui_cargada) return;
    UnloadTexture(tex_logo);
    UnloadTexture(tex_boton_normal);
    UnloadTexture(tex_boton_hover);
    ui_cargada = false;
}

void actualizar_menu()
{
    Vector2 raton = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(raton, botones[0].area)) {
            iniciar_partida();
            pantalla_actual = PANTALLA_JUEGO;
        } else if (CheckCollisionPointRec(raton, botones[1].area)) {
            pantalla_actual = PANTALLA_RECORDS;
        } else if (CheckCollisionPointRec(raton, botones[2].area)) {
            CloseWindow();
        }
    }
    if (IsKeyPressed(KEY_F)) filtro_activo = !filtro_activo;
}

void dibujar_menu()
{
    DrawRectangle(0, 0, ANCHO, ALTO, (Color){ 25, 25, 35, 255 });

    // LOGO: SI EXISTE assets/ui/logo.png LO DIBUJO, SI NO USO TEXTO
    if (ui_cargada && tex_logo.id != 0) {
        Rectangle ori = { 0, 0, (float)tex_logo.width, (float)tex_logo.height };
        Rectangle des = { ANCHO / 2.0f - 350, 80, 700, 240 };   // TAMANO DEL LOGO EN PANTALLA
        DrawTexturePro(tex_logo, ori, des, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        dibujar_texto_centrado("BOOMBASTIC", ANCHO / 2, 200, 110, YELLOW);
        dibujar_texto_centrado("Aprende matematicas a pisotones", ANCHO / 2, 320, 28, LIGHTGRAY);
    }

    // BOTONES: SI EXISTEN LAS IMAGENES LAS DIBUJO, SI NO PINTO RECTANGULOS
    Vector2 raton = GetMousePosition();
    for (int i = 0; i < 3; i++) {
        bool encima = CheckCollisionPointRec(raton, botones[i].area);

        Texture2D imagen = encima ? tex_boton_hover : tex_boton_normal;
        if (ui_cargada && imagen.id != 0) {
            Rectangle ori = { 0, 0, (float)imagen.width, (float)imagen.height };
            DrawTexturePro(imagen, ori, botones[i].area, (Vector2){ 0, 0 }, 0.0f, WHITE);
        } else {
            Color relleno = encima ? (Color){ 170, 170, 170, 255 }
                                   : (Color){  80,  80,  80, 255 };
            DrawRectangleRec(botones[i].area, relleno);
            DrawRectangleLinesEx(botones[i].area, 3, BLACK);
        }

        // EL TEXTO DEL BOTON SIEMPRE VA ENCIMA (JUGAR / RECORDS / SALIR)
        int cx = (int)(botones[i].area.x + botones[i].area.width  / 2);
        int cy = (int)(botones[i].area.y + botones[i].area.height / 2 - 16);
        dibujar_texto_centrado(botones[i].etiqueta, cx, cy, 32, encima ? BLACK : RAYWHITE);
    }

    DrawText("[F] filtro retro on/off", 20, ALTO - 36, 20, GRAY);
}

/* ===========================================================================
   16. RECORDS (LISTA GUARDADA EN EL ARCHIVO)
=========================================================================== */
void actualizar_records()
{
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
        pantalla_actual = PANTALLA_MENU;
}

void dibujar_records()
{
    DrawRectangle(0, 0, ANCHO, ALTO, (Color){ 20, 20, 30, 255 });
    dibujar_texto_centrado("RECORDS", ANCHO / 2, 140, 72, GOLD);
    for (int i = 0; i < MAX_RECORDS; i++) {
        DrawText(TextFormat("%d.  %-12s", i + 1, records[i].nombre),
                 ANCHO / 2 - 240, 320 + i * 72, 36, RAYWHITE);
        DrawText(TextFormat("%6d pts", records[i].puntos),
                 ANCHO / 2 + 110, 320 + i * 72, 36, YELLOW);
    }
    dibujar_texto_centrado("[ENTER] volver al menu", ANCHO / 2, ALTO - 80, 24, LIGHTGRAY);
}

/* ===========================================================================
   17. FIN DEL JUEGO: ESCRIBIR EL NOMBRE (SOLO LETRAS) Y GUARDAR EL RECORD
=========================================================================== */
void actualizar_fin()
{
    if (!record_guardado) {
        // LEO LO QUE TECLEA EL JUGADOR PERO SOLO ACEPTO LETRAS
        int c = GetCharPressed();
        while (c > 0) {
            if (es_letra(c) && largo_nombre < MAX_NOMBRE) {
                nombre_jugador[largo_nombre] = (char)c;
                largo_nombre++;
                nombre_jugador[largo_nombre] = '\0';
            }
            c = GetCharPressed();
        }
        // BORRAR CON RETROCESO
        if (IsKeyPressed(KEY_BACKSPACE) && largo_nombre > 0) {
            largo_nombre--;
            nombre_jugador[largo_nombre] = '\0';
        }
        // GUARDAR CON ENTER
        if (IsKeyPressed(KEY_ENTER)) {
            if (largo_nombre == 0) {                  // SI NO ESCRIBIO NADA
                strcpy(nombre_jugador, "JUGADOR");
            }
            insertar_record(nombre_jugador, puntos);
            record_guardado = true;
        }
    } else {
        if (IsKeyPressed(KEY_ENTER)) pantalla_actual = PANTALLA_MENU;
        if (IsKeyPressed(KEY_R))     pantalla_actual = PANTALLA_RECORDS;
    }
}

void dibujar_fin()
{
    DrawRectangle(0, 0, ANCHO, ALTO, (Color){ 30, 10, 10, 255 });
    dibujar_texto_centrado("GAME OVER", ANCHO / 2, 200, 96, RED);

    // EXPLICO POR QUE PERDIO
    if (tiempo_partida <= 0)
        dibujar_texto_centrado("SE ACABO EL TIEMPO", ANCHO / 2, 330, 32, ORANGE);
    else
        dibujar_texto_centrado("TE QUEDASTE SIN VIDAS", ANCHO / 2, 330, 32, ORANGE);

    dibujar_texto_centrado(TextFormat("Llegaste al nivel %d   -   Puntaje: %d",
                           nivel_actual, puntos), ANCHO / 2, 400, 34, YELLOW);

    if (!record_guardado) {
        // CAJA PARA ESCRIBIR EL NOMBRE
        dibujar_texto_centrado("ESCRIBE TU NOMBRE (solo letras):", ANCHO / 2, 500, 30, RAYWHITE);

        Rectangle caja = { ANCHO / 2.0f - 250, 555, 500, 70 };
        DrawRectangleRec(caja, (Color){ 20, 20, 30, 255 });
        DrawRectangleLinesEx(caja, 4, RAYWHITE);

        // EL CURSOR PARPADEA UN POCO
        const char *cursor = ((int)(GetTime() * 2) % 2 == 0) ? "_" : " ";
        dibujar_texto_centrado(TextFormat("%s%s", nombre_jugador, cursor),
                               ANCHO / 2, 570, 40, GOLD);

        dibujar_texto_centrado("[ENTER] guardar record", ANCHO / 2, 670, 26, LIGHTGRAY);
    } else {
        dibujar_texto_centrado("Record guardado!", ANCHO / 2, 520, 34, GREEN);
        dibujar_texto_centrado("[ENTER] Menu     [R] Records", ANCHO / 2, 620, 28, LIGHTGRAY);
    }
}

#endif // GAME_H
