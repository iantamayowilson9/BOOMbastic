#include "jugador.h"

void dibujar_jugador()
{
    if (chango.invulnerable > 0 && ((int)(chango.invulnerable * 12) % 2 == 0))
    {
        return;
    }

    if (textura_chango_cargada)
    {
        Texture2D textura_actual = textura_chango_salto;

        switch (chango.estado_textura)
        {
        case IZQ:
            textura_actual = textura_chango_izq;
            break;
        case DER:
            textura_actual = textura_chango_der;
            break;
        case CAIDA:
            textura_actual = textura_chango_cayendo;
            break;
        case SALTO:
        default:
            textura_actual = textura_chango_salto;
            break;
        }

        Rectangle destino = {chango.pos.x, chango.pos.y, chango.tam.x, chango.tam.y};

        // Efecto de color según estado
        Color tint = WHITE;
        if (chango.clavando)
        {
            tint = RED;
        }
        else
        {
            if (chango.invulnerable > 0)
            {
                tint = ColorAlpha(WHITE, 0.6f);
            }
        }

        DrawTexturePro(textura_actual, (Rectangle){0, 0, (float)textura_actual.width, (float)textura_actual.height}, destino, (Vector2){0, 0}, 0.0f, tint);
    }
    else
    {
        Color color = (chango.invulnerable > 0) ? ColorAlpha(RED, 0.5f) : BLUE;
        if (chango.clavando)
        {
            color = DARKPURPLE;
        }
        DrawRectangleV(chango.pos, chango.tam, color);
    }
}