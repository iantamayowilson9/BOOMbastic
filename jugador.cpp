#include "jugador.h"

void dibujar_jugador()
{
    if (chango.invulnerable > 0 && ((int)(chango.invulnerable * 12) % 2 == 0))
    {
        return;
    }

    if (textura_chango_cargada)
    {
        Rectangle source = {0, 0, (float)textura_chango.width, (float)textura_chango.height};
        Rectangle dest = {chango.pos.x, chango.pos.y, chango.tam.x, chango.tam.y};

        // Efecto de color según estado
        Color tint = WHITE;
        if (chango.clavando)
        {
            tint = RED;
        }
        else
        {
            if (chango.invulnerable > 0) {
                tint = ColorAlpha(WHITE, 0.6f);
            }
        }

        DrawTexturePro(textura_chango, source, dest, (Vector2){0, 0}, 0.0f, tint);
    }
    else
    {
        Color color = (chango.invulnerable > 0) ? ColorAlpha(RED, 0.5f) : BLUE;
        if (chango.clavando) {
            color = DARKPURPLE;
        }
        DrawRectangleV(chango.pos, chango.tam, color);
    }
}