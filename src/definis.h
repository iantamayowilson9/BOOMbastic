#ifndef DEFINIS_H
#define DEFINIS_H

// Modulo central de datos. Aqui viven las constantes, los tipos y las
// variables globales que comparte todo el juego. No hay apuntadores.

#include "raylib.h"
#include <stdbool.h>

// ==================================================================== MEDIDAS

#define V_ANCHO 1280
#define V_ALTO 720

// Rectangulo central donde transcurre el gameplay (para el ScissorMode)
#define VP_X 40
#define VP_Y 130
#define VP_ANCHO 1200
#define VP_ALTO 520
#define SUELO_Y (VP_Y + VP_ALTO) // El piso queda al fondo del viewport

// ================================================================== LIMITES

#define MAX_FRUTAS 20
#define MAX_PARTICULAS 256
#define MAX_TEXTOS 16
#define MAX_BANANAS 60
#define MAX_RECORDS 5
#define MAX_NOMBRE 12

// =============================================================== REGLAS

#define TIEMPO_PARTIDA 120.0f  // Dos minutos por partida
#define COMBO_MAX 5            // El combo llega hasta 5
#define VENTANA_COMBO 1.5f     // Segundos para encadenar el combo
#define TIEMPO_MINIJUEGO 15.0f // Duracion de la lluvia de bananas
#define TIEMPO_GRACIA 3.0f     // Segundos flotando al inicio mientras se llena el piso

// =============================================================== FISICA

#define GRAVEDAD 1050.0f   // Peso constante estilo burrito bison
#define REBOTE -1000.0f    // Impulso vertical al rebotar sobre una fruta
#define PICADA 1500.0f     // Velocidad de caida al hacer fast smash
#define VEL_LATERAL 460.0f // Movimiento horizontal del chango
#define BONO_PICADA 1.25f  // Rebote 25 por ciento mas fuerte tras picada
#define INVULN 0.8f        // Segundos sin recibir dano tras perder vida

#define GRAVEDAD_JUGO 1400.0f // Peso de las particulas de jugo

#define ARCH_RECORDS "datos/records.dat"

// =============================================================== TIPOS

// Fases pedagogicas del juego
enum Fase
{
    ENTEROS, // Sumas sencillas
    MINUTOS, // Minutos y restas
    HORAS    // Horas con minutos
};

// Textura activa del chango segun su movimiento
enum EstadoChango
{
    ARRIBA,
    IZQUIERDA,
    DERECHA,
    APLASTAR
};

// Estado general (sin menu: solo partida, minijuego y fin)
enum EstadoJuego
{
    JUGANDO,
    MINIJUEGO,
    FIN
};

// Personaje principal
struct DatosJugador
{
    Vector2 pos;
    Vector2 tam;
    float vel_y;
    int vida;
    bool clavando;       // Verdadero mientras hace fast smash
    float invulnerable;  // Cuenta regresiva de invulnerabilidad
    int estado;          // Cual textura mostrar
    float x_congelada;   // Columna fija durante el minijuego
};

// Fruta que desfila por el suelo de la jungla
struct Fruta
{
    Vector2 pos; // Centro de la fruta
    float radio;
    float vel;
    int valor;  // Negativo si es bomba
    int tipo;   // 0 sandia 1 platano 2 uva 3 manzana 4 bomba
    Color color;
    bool activa;
};

// Gota de jugo que salta al aplastar
struct Particula
{
    Vector2 pos;
    Vector2 vel;
    float tamano;
    float vida; // Va de 1.0 a 0.0
    Color color;
    bool activa;
};

// Texto que sube y se desvanece (ejemplo +5 x3)
struct TextoFlotante
{
    Vector2 pos;
    char texto[24];
    float vida;
    Color color;
    bool activa;
};

// Banana que cae en el minijuego
struct Banana
{
    Vector2 pos;
    float vel;
    float radio;
    bool activa;
};

// Record que se guarda en disco
struct Record
{
    char nombre[MAX_NOMBRE + 1];
    int puntos;
};

// Configuracion de cada nivel
struct ConfigNivel
{
    int nivel;
    Fase fase;
    int meta; // Numeros enteros o minutos
    float vel_frutas;
    float intervalo; // Segundos entre frutas
    bool permite_negativos;
};

// ==================================================== VARIABLES GLOBALES

// Solo main.cpp compila estos modulos, asi que se definen aqui mismo.
DatosJugador chango;
Fruta frutas[MAX_FRUTAS];
Particula particulas[MAX_PARTICULAS];
TextoFlotante textos[MAX_TEXTOS];
Banana bananas[MAX_BANANAS];
Record records[MAX_RECORDS];
ConfigNivel config;

EstadoJuego estado_juego = JUGANDO;

int nivel_actual = 1;
int valor_actual = 0; // Lo que llevo hacia la meta
int puntos = 0;       // Puntaje para los records
int combo = 0;

float tiempo_partida = TIEMPO_PARTIDA;
float tiempo_aparicion = 0.0f;
float reloj_juego = 0.0f;       // Tiempo total para medir los combos
float tiempo_ultimo_combo = 0.0f;
float parpadeo_puntaje = 0.0f;
float velocidad_extra = 0.0f;   // Se suma a la cinta tras una picada buena
float scroll_fondo = 0.0f;      // Desplazamiento del fondo de jungla
float tiempo_gracia = 0.0f;     // Cuenta regresiva del periodo de gracia
int metas_logradas = 0;         // Cuantas metas completo el jugador
int prob_bomba = 20;            // Probabilidad de que una fruta sea bomba

// Minijuego
float tiempo_minijuego = 0.0f;
int bananas_recolectadas = 0;
float tiempo_spawn_banana = 0.0f;

// Captura de nombre en la pantalla de fin
char nombre_jugador[MAX_NOMBRE + 1] = "";
int largo_nombre = 0;
bool record_guardado = false;
bool gano_por_tiempo = false; // Para explicar el porque del game over
bool pausa = false;           // Verdadero cuando el juego esta en pausa

#endif
