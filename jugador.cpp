#include "jugador.h"

void dibujar_jugador()
{
    // PARPADEA MIENTRAS ES INVULNERABLE (ACABA DE PERDER UNA VIDA)
    if (chango.invulnerable > 0 && ((int)(chango.invulnerable * 12) % 2 == 0))
        return;

    float x = chango.pos.x;
    float y = chango.pos.y;
    float w = chango.tam.x;
    float h = chango.tam.y;

    Color cuerpo = chango.clavando ? (Color){150, 90, 40, 255} : BROWN;

    DrawRectangleRounded((Rectangle){x, y + h * 0.35f, w, h * 0.65f}, 0.4f, 8, cuerpo);
    DrawCircle((int)(x + w / 2), (int)(y + h * 0.25f), w * 0.42f, cuerpo);    // CABEZA
    DrawCircle((int)(x + w * 0.1f), (int)(y + h * 0.18f), w * 0.16f, cuerpo); // OREJA IZQ
    DrawCircle((int)(x + w * 0.9f), (int)(y + h * 0.18f), w * 0.16f, cuerpo); // OREJA DER
    DrawCircle((int)(x + w / 2), (int)(y + h * 0.30f), w * 0.26f, BEIGE);     // CARA
    DrawCircle((int)(x + w * 0.38f), (int)(y + h * 0.20f), 4, BLACK);         // OJO
    DrawCircle((int)(x + w * 0.62f), (int)(y + h * 0.20f), 4, BLACK);         // OJO

    if (chango.clavando)
        DrawText("BOOM!", (int)(x - 6), (int)(y - 26), 24, RED);
}