#include "frutas.h"

#include <stdio.h>
#include <stdbool.h>

int arr_indice()
{
    // Regresa el indice de una fruta inactiva para crear una activa en su lugar. Si todas estan activas, regresa -1
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
        {
            return i;
        }
    }
    return -1;
}

float posicion_inicial_frutas(float radio)
{
    float x = V_ANCHO + radio; // Fuera de la vista
    float desplazar_der = 0.0f;
    bool frente_otras = false;

    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (frutas[i].activa)
        {
            float borde_derecho = frutas[i].pos.x + frutas[i].radio;
            if (borde_derecho > desplazar_der)
            {
                desplazar_der = borde_derecho;
                frente_otras = true;
            }
        }
    }

    if (frente_otras) {
        float espacio_minimo = desplazar_der + radio + 20.0f;
        if (x < espacio_minimo) {
            x = espacio_minimo;
        }
    }

    return x;
}

void valores_frutas(int indice) {
    int enteros[4] = {10, 5, 2, 1};
    int minutos[4] = {60, 30, 15, 10};
    Color colores[4] = {RED, YELLOW, PURPLE, GREEN};

    bool val_negativo = config.permite_negativos && (GetRandomValue(0, 100) < 20); // 20% de probabilidad de ser bomba

    if (val_negativo) {
        frutas[indice].valor = -minutos[GetRandomValue(0, 3)];
        frutas[indice].color = DARKGRAY;
        frutas[indice].tipo = 4; // Bomba
    } else {
        int tipo = GetRandomValue(0, 3);
        frutas[indice].tipo = tipo;
        frutas[indice].valor = (config.fase == ENTEROS) ? enteros[tipo] : minutos[tipo];
        frutas[indice].color = colores[tipo];
    }
}

void crear_fruta()
{
    int indice = arr_indice();

    if (indice == -1) {
        return;
    }

    frutas[indice].activa = true;
    frutas[indice].radio = 96.0f;
    frutas[indice].vel = config.vel_frutas;

    // Posicion donde las frutas apareceran
    float x_inicio = posicion_inicial_frutas(frutas[indice].radio);
    float y_inicio = SUELO_Y - frutas[indice].radio;
    frutas[indice].pos = (Vector2){x_inicio, y_inicio};

    valores_frutas(indice);
}

void actualizar_frutas(float dt)
{
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa) {
            continue;   
        }

        frutas[i].pos.x -= frutas[i].vel * dt;

        // Si sale por la izquierda se recicla sin castigo
        if (frutas[i].pos.x + frutas[i].radio < 0)
        {
            frutas[i].activa = false;
            continue;
        }
    }
}

void dibujar_frutas()
{
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa) {
            continue;
        }

        DrawCircleV(frutas[i].pos, frutas[i].radio, frutas[i].color);
        DrawCircleLines((int)frutas[i].pos.x, (int)frutas[i].pos.y, frutas[i].radio, BLACK);

        // ANILLO BLANCO: FRUTAS QUE ME ACERCAN A LA META SIN PASARME
        // ANILLO DORADO: SI ME DEJAN JUSTO EN LA META
        if (frutas[i].valor > 0 && (valor_actual + frutas[i].valor) <= config.meta)
        {
            for (int k = 4; k <= 12; k++)
                DrawCircleLines((int)frutas[i].pos.x, (int)frutas[i].pos.y,
                                frutas[i].radio + k, RAYWHITE);
            if ((valor_actual + frutas[i].valor) == config.meta)
                for (int k = 14; k <= 22; k++)
                    DrawCircleLines((int)frutas[i].pos.x, (int)frutas[i].pos.y,
                                    frutas[i].radio + k, GOLD);
        }

        // EL NUMERO GRANDE ENCIMA DE LA FRUTA
        char etiqueta[16];
        if (frutas[i].valor >= 0)
            sprintf(etiqueta, "+%d", frutas[i].valor);
        else
            sprintf(etiqueta, "%d", frutas[i].valor);
        dibujar_texto_centrado(etiqueta, (int)frutas[i].pos.x, (int)frutas[i].pos.y - 24, 48, (frutas[i].valor >= 0) ? BLACK : RAYWHITE);
    }
}
