#ifndef PROGRAMA_H
#define PROGRAMA_H

#include "globales.h"

#include <stdbool.h>

bool es_letra(int c);
void cargar_cuadros_intro();
void descargar_cuadros_intro();
void cargar_ui();
void descargar_ui();
void cargar_jugador();
void descargar_jugador();
void cargar_records();
void guardar_records();
void insertar_record(const char nom[], int pts);
void crear_particulas(Vector2 pos, Color col, int cantidad);
void actualizar_particulas(float dt);
void dibujar_particulas();
void iniciar_partida();
void iniciar_fin_juego();
void actualizar_juego(float dt);
void dibujar_puntaje_flotante();
void dibujar_interfaz();
void dibujar_juego();

#endif