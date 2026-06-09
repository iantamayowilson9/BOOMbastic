#include "interfaz.h"
#include "programa.h"

#include <string.h>

void actualizar_intro(float dt)
{
    tiempo_intro += dt;
    // CADA CUADRO DURA DELAY_INTRO SEGUNDOS
    if (tiempo_intro >= DELAY_INTRO)
    {
        tiempo_intro = 0.0f;
        cuadro_intro++;
    }
    // SE SALTA CON ESPACIO O AL TERMINARSE LOS CUADROS
    if (cuadro_intro >= CUADROS_INTRO || IsKeyPressed(KEY_SPACE))
    {
        cuadro_intro = 0;
        pantalla_actual = MENU;
    }
}

void dibujar_intro()
{
    DrawRectangle(0, 0, V_ANCHO, V_ALTO, BLACK);

    // DIBUJO EL CUADRO ACTUAL ESTIRADO A TODA LA PANTALLA
    if (intro_cargada && cuadro_intro < CUADROS_INTRO &&
        intro_cuadros[cuadro_intro].id != 0)
    {

        Texture2D cuadro = intro_cuadros[cuadro_intro];
        Rectangle origen = {0, 0, (float)cuadro.width, (float)cuadro.height};
        Rectangle destino = {0, 0, (float)V_ANCHO, (float)V_ALTO};
        DrawTexturePro(cuadro, origen, destino, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        // SI NO ENCUENTRA LAS IMAGENES MUESTRO UN AVISO PARA DARME CUENTA
        dibujar_texto_centrado("FALTAN LOS CUADROS EN assets/intro", V_ANCHO / 2, 420, 30, RAYWHITE);
    }

    DrawText("[ESPACIO] saltar", 24, V_ALTO - 44, 22, RAYWHITE);
}

void iniciar_botones_menu()
{
    float btn_ancho = 360, btn_alto = 70, boton_x = (V_CENTRO_X - btn_ancho / 2.0f);

    botones[0].area = (Rectangle){boton_x, 410, btn_ancho, btn_alto};
    strcpy(botones[0].texto, "JUGAR");

    botones[1].area = (Rectangle){boton_x, 510, btn_ancho, btn_alto};
    strcpy(botones[1].texto, "RECORDS");

    botones[2].area = (Rectangle){boton_x, 610, btn_ancho, btn_alto};
    strcpy(botones[2].texto, "SALIR");
}

void actualizar_menu()
{
    Vector2 raton = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(raton, botones[0].area))
        {
            iniciar_partida();
            pantalla_actual = JUEGO;
        }
        else if (CheckCollisionPointRec(raton, botones[1].area))
        {
            pantalla_actual = RECORDS;
        }
        else if (CheckCollisionPointRec(raton, botones[2].area))
        {
            CloseWindow();
        }
    }
    if (IsKeyPressed(KEY_F))
        filtro_activo = !filtro_activo;
}

void dibujar_menu()
{
    DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){25, 25, 35, 255});

    // LOGO: SI EXISTE assets/ui/logo.png LO DIBUJO, SI NO USO TEXTO
    if (ui_cargada && tex_logo.id != 0)
    {
        Rectangle ori = {0, 0, (float)tex_logo.width, (float)tex_logo.height};
        Rectangle des = {V_ANCHO / 2.0f - 350, 80, 700, 240}; // TAMANO DEL LOGO EN PANTALLA
        DrawTexturePro(tex_logo, ori, des, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        dibujar_texto_centrado("BOOMBASTIC", V_ANCHO / 2, 200, 110, YELLOW);
        dibujar_texto_centrado("Aprende matematicas a pisotones", V_ANCHO / 2, 320, 28, LIGHTGRAY);
    }

    // BOTONES: SI EXISTEN LAS IMAGENES LAS DIBUJO, SI NO PINTO RECTANGULOS
    Vector2 raton = GetMousePosition();
    for (int i = 0; i < 3; i++)
    {
        bool encima = CheckCollisionPointRec(raton, botones[i].area);

        Texture2D imagen = encima ? tex_boton_hover : tex_boton_normal;
        if (ui_cargada && imagen.id != 0)
        {
            Rectangle ori = {0, 0, (float)imagen.width, (float)imagen.height};
            DrawTexturePro(imagen, ori, botones[i].area, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else
        {
            Color relleno = encima ? (Color){170, 170, 170, 255}
                                   : (Color){80, 80, 80, 255};
            DrawRectangleRec(botones[i].area, relleno);
            DrawRectangleLinesEx(botones[i].area, 3, BLACK);
        }

        // EL TEXTO DEL BOTON SIEMPRE VA ENCIMA (JUGAR / RECORDS / SALIR)
        int cx = (int)(botones[i].area.x + botones[i].area.width / 2);
        int cy = (int)(botones[i].area.y + botones[i].area.height / 2 - 16);
        dibujar_texto_centrado(botones[i].texto, cx, cy, 32, encima ? BLACK : RAYWHITE);
    }

    DrawText("[F] filtro retro on/off", 20, V_ALTO - 36, 20, GRAY);
}

void actualizar_records()
{
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
        pantalla_actual = MENU;
}

void dibujar_records()
{
    DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){20, 20, 30, 255});
    dibujar_texto_centrado("RECORDS", V_ANCHO / 2, 140, 72, GOLD);
    for (int i = 0; i < MAX_RECORDS; i++)
    {
        DrawText(TextFormat("%d.  %-12s", i + 1, records[i].nombre),
                 V_ANCHO / 2 - 240, 320 + i * 72, 36, RAYWHITE);
        DrawText(TextFormat("%6d pts", records[i].puntos),
                 V_ANCHO / 2 + 110, 320 + i * 72, 36, YELLOW);
    }
    dibujar_texto_centrado("[ENTER] volver al menu", V_ANCHO / 2, V_ALTO - 80, 24, LIGHTGRAY);
}

void actualizar_fin()
{
    if (!record_guardado)
    {
        // LEO LO QUE TECLEA EL JUGADOR PERO SOLO ACEPTO LETRAS
        int c = GetCharPressed();
        while (c > 0)
        {
            if (es_letra(c) && largo_nombre < MAX_NOMBRE)
            {
                nombre_jugador[largo_nombre] = (char)c;
                largo_nombre++;
                nombre_jugador[largo_nombre] = '\0';
            }
            c = GetCharPressed();
        }
        // BORRAR CON RETROCESO
        if (IsKeyPressed(KEY_BACKSPACE) && largo_nombre > 0)
        {
            largo_nombre--;
            nombre_jugador[largo_nombre] = '\0';
        }
        // GUARDAR CON ENTER
        if (IsKeyPressed(KEY_ENTER))
        {
            if (largo_nombre == 0)
            { // SI NO ESCRIBIO NADA
                strcpy(nombre_jugador, "JUGADOR");
            }
            insertar_record(nombre_jugador, puntos);
            record_guardado = true;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
            pantalla_actual = MENU;
        if (IsKeyPressed(KEY_R))
            pantalla_actual = RECORDS;
    }
}

void dibujar_fin()
{
    DrawRectangle(0, 0, V_ANCHO, V_ALTO, (Color){30, 10, 10, 255});
    dibujar_texto_centrado("GAME OVER", V_ANCHO / 2, 200, 96, RED);

    // EXPLICO POR QUE PERDIO
    if (tiempo_partida <= 0)
        dibujar_texto_centrado("SE ACABO EL TIEMPO", V_ANCHO / 2, 330, 32, ORANGE);
    else
        dibujar_texto_centrado("TE QUEDASTE SIN VIDAS", V_ANCHO / 2, 330, 32, ORANGE);

    dibujar_texto_centrado(TextFormat("Llegaste al nivel %d   -   Puntaje: %d",
                                      nivel_actual, puntos),
                           V_ANCHO / 2, 400, 34, YELLOW);

    if (!record_guardado)
    {
        // CAJA PARA ESCRIBIR EL NOMBRE
        dibujar_texto_centrado("ESCRIBE TU NOMBRE (solo letras):", V_ANCHO / 2, 500, 30, RAYWHITE);

        Rectangle caja = {V_ANCHO / 2.0f - 250, 555, 500, 70};
        DrawRectangleRec(caja, (Color){20, 20, 30, 255});
        DrawRectangleLinesEx(caja, 4, RAYWHITE);

        // EL CURSOR PARPADEA UN POCO
        const char *cursor = ((int)(GetTime() * 2) % 2 == 0) ? "_" : " ";
        dibujar_texto_centrado(TextFormat("%s%s", nombre_jugador, cursor),
                               V_ANCHO / 2, 570, 40, GOLD);

        dibujar_texto_centrado("[ENTER] guardar record", V_ANCHO / 2, 670, 26, LIGHTGRAY);
    }
    else
    {
        dibujar_texto_centrado("Record guardado!", V_ANCHO / 2, 520, 34, GREEN);
        dibujar_texto_centrado("[ENTER] Menu     [R] Records", V_ANCHO / 2, 620, 28, LIGHTGRAY);
    }
}

void dibujar_filtro()
{
    if (!filtro_activo)
        return;
    for (int y = 0; y < V_ALTO; y += 4)
        DrawRectangle(0, y, V_ANCHO, 1, ColorAlpha(BLACK, 0.15f));
}

void crear_texto_flotante(Vector2 origen, const char texto[], Color color)
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (textos[i].activa)
            continue;
        textos[i].activa = true;
        textos[i].pos = origen;
        textos[i].vida = 1.0f;
        textos[i].color = color;
        strncpy(textos[i].texto, texto, sizeof(textos[i].texto) - 1);
        textos[i].texto[sizeof(textos[i].texto) - 1] = '\0';
        return;
    }
}

void actualizar_textos_flotantes(float dt)
{
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (!textos[i].activa) continue;
        textos[i].pos.y -= 40.0f * dt;
        textos[i].vida       -= dt;
        if (textos[i].vida <= 0) textos[i].activa = false;
    }
}

void dibujar_textos_flotantes()
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (!textos[i].activa)
            continue;
        Color c = Fade(textos[i].color, textos[i].vida);
        dibujar_texto_centrado(textos[i].texto, (int)textos[i].pos.x,
                               (int)textos[i].pos.y, 28, c);
    }
}
