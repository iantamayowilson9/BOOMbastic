#ifndef GLOBALES_H
#define GLOBALES_H

#include "raylib.h"
#include <stdbool.h>

// ==================================================================================================== CONSTANTES

#define V_ANCHO 1280
#define V_ALTO 720
#define V_CENTRO_X (V_ANCHO / 2)
#define V_CENTRO_Y (V_ALTO / 2)
#define SUELO_Y 760

#define MAX_FRUTAS 15
#define MAX_PARTICULAS 256
#define MAX_TEXTOS 16
#define MAX_RECORDS 5

#define TIEMPO_PARTIDA 120.0f // Dos minutos por partida
#define COMBO_MAX 5 // Un combo llegara hasta 5

#define ARCH_RECORDS "datos/records.dat" // Archivo binario con records
#define MAX_NOMBRE 12 // Maximo de letras para el nombre

// Fisica del jugador
#define REBOTE -520.0f
#define CLAVADO 900.0f
#define VEL_LATERAL 440.0f
#define INVULN 0.7f // Segundos sin recibir daño al perder vida

#define GRAVEDAD_DEFECTO 380.0f
#define GRAVEDAD_MIN 200.0f
#define GRAVEDAD_MAX 1800.0f
#define GRAVEDAD_PASO 80.0f

#define GRAVEDAD_JUGO 1400.0f // Gravedad para las particulas

#define CUADROS_INTRO 215 // SON 215 CUADROS (frame_001 A frame_215)
#define DELAY_INTRO 0.04f // CADA CUADRO DURA 0.04 SEG (SUBE O BAJA ESTE NUMERO)

// ==================================================================================================== ESTRUCTURAS

// Pantallas del juego
enum Pantalla
{
    INTRO,
    MENU,
    RECORDS,
    JUEGO,
    FIN
};

// Fases del juego
enum Fase
{
    ENTEROS, // Sumas (sencillas) -> Objetivo: Llegar a 20
    MINUTOS, // Minutos y restas -> Objetivo: Llegar a 45 min
    HORAS // Horas (con minutos) -> Objetivo: Llegar a 2 horas
};

// Personaje que controla el jugador
struct Jugador
{
    Vector2 pos;
    Vector2 tam;
    float vel_y;
    bool clavando;      // Verdadero mientras realiza un clavado (presionar espacio)
    float invulnerable; // Cuenta regresiva para la invulneravilidad
    int vida;
};

// Frutas que se desplazan por el suelo
struct Fruta
{
    Vector2 pos; // Centro del circulo
    float radio;
    float vel;
    int valor; // Si es negativo es una bomba
    bool activa;
    Color color;
    int tipo; // 0 - Sandia, 1 - Platano, 2 - Uva, 3 - Manzana, 4 - Bomba
};

// UNA GOTA DE JUGO QUE SALTA AL APLASTAR
struct Particula
{
    Vector2 pos;
    Vector2 vel;
    float tamano;
    float vida; // VA DE 1.0 A 0.0
    Color color;
    bool activa;
};

// UN TEXTO QUE SUBE Y SE DESVANECE (EJEMPLO: +5 x3)
struct TextoFlotante
{
    Vector2 pos;
    char texto[16];
    float vida;
    Color color;
    bool activa;
};

// Para los records que se guardaran en "records.dat"
struct Record
{
    char nombre[MAX_NOMBRE + 1];
    int puntos;
};

// Para los botones del menu
struct BotonMenu
{
    Rectangle area;
    char texto[16];
};

// Configuracion para cada nivel
struct ConfigNivel
{
    int nivel;
    Fase fase;
    int meta; // Numeros enteros o minutos
    float vel_frutas;
    float intervalo; // Segundos entre la aparicion de cada fruta
    bool permite_negativos;
};

// ==================================================================================================== VARIABLES GLOBALES

extern Pantalla pantalla_actual;

extern Jugador chango;
extern Fruta frutas[MAX_FRUTAS];
extern Particula particulas[MAX_PARTICULAS];
extern TextoFlotante textos[MAX_TEXTOS];
extern Record records[MAX_RECORDS];

extern ConfigNivel config;
extern int nivel_actual;
extern int valor_actual; // LO QUE LLEVO HACIA LA META
extern int puntos;       // PUNTAJE PARA LOS RECORDS
extern float tiempo_aparicion;
extern float gravedad;
extern int combo;
extern float tiempo_partida;
extern float parpadeo_puntaje; // PARA QUE EL PUNTAJE FLOTANTE REACCIONE

// Nombre en pantalla FIN
extern char nombre_jugador[MAX_NOMBRE + 1];
extern int largo_nombre;
extern bool record_guardado;

extern bool filtro_activo; // Filtro lineas crtv
extern int cuadro_intro;      // En que frame de la intro estamos
extern float tiempo_intro;
extern BotonMenu botones[3];

// Imagenes de la intro (frame_001.png ... frame_215.png)
extern Texture2D intro_cuadros[CUADROS_INTRO];
extern bool intro_cargada;

// Menu, imagenes de botones y logo
extern Texture2D tex_logo;
extern Texture2D tex_boton_normal;
extern Texture2D tex_boton_hover;
extern bool ui_cargada;

// ==================================================================================================== DECLARACION DE FUNCIONES

void dibujar_texto_centrado(const char texto[], int centro_x, int y, int tam, Color color);
ConfigNivel cargar_config_nivel(int nivel);

#endif