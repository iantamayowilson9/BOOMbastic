#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "globales.h"

void actualizar_intro(float dt);
void dibujar_intro();
void iniciar_botones_menu();
void actualizar_menu();
void dibujar_menu();
void actualizar_records();
void dibujar_records();
void actualizar_fin();
void dibujar_fin();
void dibujar_filtro();
void crear_texto_flotante(Vector2 pos, const char txt[], Color col);
void actualizar_textos_flotantes(float dt);
void dibujar_textos_flotantes();

#endif