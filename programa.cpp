#include "programa.h"
#include "jugador.h"
#include "frutas.h"
#include "interfaz.h"

#include <stdio.h>
#include <string.h>

bool es_letra(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void cargar_cuadros_intro()
{
    for (int i = 0; i < CUADROS_INTRO; i++)
    {
        // LOS ARCHIVOS VAN DE frame_001 A frame_215 (POR ESO i + 1 Y %03d)
        intro_cuadros[i] = LoadTexture(TextFormat("assets/intro/frame_%03d.png", i + 1));
    }
    intro_cargada = true;
}

void descargar_cuadros_intro()
{
    if (!intro_cargada)
        return;
    for (int i = 0; i < CUADROS_INTRO; i++)
        UnloadTexture(intro_cuadros[i]);
    intro_cargada = false;
}

void cargar_ui()
{
    tex_logo = LoadTexture("assets/ui/logo.png");
    tex_boton_normal = LoadTexture("assets/ui/boton_normal.png");
    tex_boton_hover = LoadTexture("assets/ui/boton_hover.png");
    ui_cargada = true;
}

void descargar_ui()
{
    if (!ui_cargada)
        return;
    UnloadTexture(tex_logo);
    UnloadTexture(tex_boton_normal);
    UnloadTexture(tex_boton_hover);
    ui_cargada = false;
}
// Leer tabla de records de "records.dat" o crearla si no existe
void cargar_records()
{
    FILE *archivo = fopen(ARCH_RECORDS, "rb");
    if (archivo == NULL)
    {
        for (int i = 0; i < MAX_RECORDS; i++)
        {
            strcpy(records[i].nombre, "---");
            records[i].puntos = 0;
        }
        return;
    }
    fread(records, sizeof(Record), MAX_RECORDS, archivo);
    fclose(archivo);
}
// Escribe los records en el archivo
void guardar_records()
{
    FILE *archivo = fopen(ARCH_RECORDS, "wb");
    if (archivo == NULL)
        return;
    fwrite(records, sizeof(Record), MAX_RECORDS, archivo);
    fclose(archivo);
}

void insertar_record(const char nombre[], int nuevos_puntos)
{
    int lugar = -1;
    for (int i = 0; i < MAX_RECORDS; i++)
    {
        if (nuevos_puntos > records[i].puntos)
        {
            lugar = i;
            break;
        }
    }
    if (lugar == -1)
        return; // NO ALCANZO EL TOP

    // EMPUJO HACIA ABAJO DESDE EL ULTIMO HASTA EL LUGAR
    for (int i = MAX_RECORDS - 1; i > lugar; i--)
    {
        records[i] = records[i - 1];
    }
    strncpy(records[lugar].nombre, nombre, MAX_NOMBRE);
    records[lugar].nombre[MAX_NOMBRE] = '\0';
    records[lugar].puntos = nuevos_puntos;

    guardar_records();
}

void crear_particulas(Vector2 origen, Color color, int cantidad)
{
    int hechas = 0;
    for (int i = 0; i < MAX_PARTICULAS && hechas < cantidad; i++)
    {
        if (particulas[i].activa)
            continue;
        particulas[i].activa = true;
        particulas[i].pos = origen;
        particulas[i].vel = (Vector2){(float)GetRandomValue(-260, 260), (float)GetRandomValue(-420, -120)};
        particulas[i].tamano = (float)GetRandomValue(4, 10);
        particulas[i].vida = 1.0f;
        particulas[i].color = color;
        hechas++;
    }
}

void actualizar_particulas(float dt)
{
    for (int i = 0; i < MAX_PARTICULAS; i++)
    {
        if (!particulas[i].activa)
            continue;
        particulas[i].vel.y += GRAVEDAD_JUGO * dt;
        particulas[i].pos.x += particulas[i].vel.x * dt;
        particulas[i].pos.y += particulas[i].vel.y * dt;
        particulas[i].vida -= dt * 1.5f;
        if (particulas[i].vida <= 0)
            particulas[i].activa = false;
    }
}

void dibujar_particulas()
{
    for (int i = 0; i < MAX_PARTICULAS; i++)
    {
        if (!particulas[i].activa)
            continue;
        Color c = Fade(particulas[i].color, particulas[i].vida);
        DrawRectangleV(particulas[i].pos,
                       (Vector2){particulas[i].tamano, particulas[i].tamano}, c);
    }
}

void iniciar_partida()
{
    nivel_actual = 1;
    valor_actual = 0;
    puntos = 0;
    tiempo_aparicion = 0.0f;
    gravedad = GRAVEDAD_DEFECTO;
    combo = 0;
    tiempo_partida = TIEMPO_PARTIDA;
    config = cargar_config_nivel(nivel_actual);

    chango.tam = (Vector2){150, 190};
    chango.pos = (Vector2){V_CENTRO_X - chango.tam.x / 2.0f, 60}; // El jugador comienza en el aire, en el centro de la pantalla
    chango.vel_y = 0;
    chango.clavando = false;
    chango.invulnerable = 0;
    chango.vida = 3;

    for (int i = 0; i < MAX_FRUTAS; i++)
        frutas[i].activa = false;
    for (int i = 0; i < MAX_PARTICULAS; i++)
        particulas[i].activa = false;
    for (int i = 0; i < MAX_TEXTOS; i++)
        textos[i].activa = false;

    // Iniciar algunas frutas
    for (int i = 0; i < 8; i++)
    {
        crear_fruta();
        if (frutas[i].activa)
        {
            frutas[i].pos.x = 120.0f + i * 200.0f;
        }
    }
}
// Preparar pantalla FIN, ahi se escribira el nombre para guardar el record, o solo se mostrara el resultado alcanzado
void iniciar_fin_juego()
{
    nombre_jugador[0] = '\0';
    largo_nombre = 0;
    record_guardado = false;
    pantalla_actual = FIN;
}

void actualizar_juego(float dt)
{
    // EL RELOJ DE LA PARTIDA (DOS MINUTOS)
    tiempo_partida -= dt;
    if (tiempo_partida <= 0)
    {
        tiempo_partida = 0;
        iniciar_fin_juego();
        return;
    }

    // MOVER A LOS LADOS SIN SALIR DE LA PANTALLA
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        chango.pos.x -= VEL_LATERAL * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        chango.pos.x += VEL_LATERAL * dt;
    if (chango.pos.x < 0)
        chango.pos.x = 0;
    if (chango.pos.x > V_ANCHO - chango.tam.x)
        chango.pos.x = V_ANCHO - chango.tam.x;

    // SUBIR Y BAJAR LA GRAVEDAD CON LAS FLECHAS
    if (IsKeyDown(KEY_UP))
        gravedad += GRAVEDAD_PASO * 40 * dt;
    if (IsKeyDown(KEY_DOWN))
        gravedad -= GRAVEDAD_PASO * 40 * dt;
    if (IsKeyPressed(KEY_UP))
        gravedad += GRAVEDAD_PASO;
    if (IsKeyPressed(KEY_DOWN))
        gravedad -= GRAVEDAD_PASO;
    if (gravedad < GRAVEDAD_MIN)
        gravedad = GRAVEDAD_MIN;
    if (gravedad > GRAVEDAD_MAX)
        gravedad = GRAVEDAD_MAX;

    // CLAVADO CON ESPACIO PARA CAER MAS RAPIDO
    if (IsKeyPressed(KEY_SPACE))
    {
        chango.vel_y = CLAVADO;
        chango.clavando = true;
    }
    if (chango.vel_y < 0)
        chango.clavando = false;

    // FISICA HACIA ARRIBA Y ABAJO
    chango.vel_y += gravedad * dt;
    chango.pos.y += chango.vel_y * dt;
    if (chango.pos.y < 0)
    { // TOPE DE ARRIBA
        chango.pos.y = 0;
        if (chango.vel_y < 0)
            chango.vel_y = 0;
    }
    if (chango.invulnerable > 0)
        chango.invulnerable -= dt;

    Rectangle caja_chango = {chango.pos.x, chango.pos.y,
                             chango.tam.x, chango.tam.y};
    float pies = chango.pos.y + chango.tam.y;

    // VAN NACIENDO FRUTAS CON EL TIEMPO
    tiempo_aparicion += dt;
    if (tiempo_aparicion >= config.intervalo)
    {
        tiempo_aparicion = 0.0f;
        crear_fruta();
    }

    // FRUTAS: SE MUEVEN A LA IZQUIERDA Y SE REVISAN LOS CHOQUES
    for (int i = 0; i < MAX_FRUTAS; i++)
    {
        if (!frutas[i].activa)
            continue;

        frutas[i].pos.x -= frutas[i].vel * dt;

        // SI SALE POR LA IZQUIERDA SE RECICLA SIN CASTIGO
        if (frutas[i].pos.x + frutas[i].radio < 0)
        {
            frutas[i].activa = false;
            continue;
        }

        if (!CheckCollisionCircleRec(frutas[i].pos, frutas[i].radio, caja_chango))
            continue;

        if (frutas[i].valor < 0)
        {
            // BOMBA: PIERDE VIDA Y SE ROMPE EL COMBO
            if (chango.invulnerable <= 0)
            {
                chango.vida--;
                combo = 0;
                chango.invulnerable = INVULN;
                chango.vel_y = REBOTE;
                crear_particulas(frutas[i].pos, RED, 18);
                crear_texto_flotante(frutas[i].pos, "BOMBA! -1", RED);
                frutas[i].activa = false;
            }
        }
        else
        {
            // FRUTA BUENA: SOLO REBOTA SI VIENE CAYENDO SOBRE ELLA
            bool cayendo = chango.vel_y > 0;
            if (cayendo && pies < frutas[i].pos.y + frutas[i].radio)
            {

                bool me_paso = (valor_actual + frutas[i].valor) > config.meta;

                if (me_paso)
                {
                    // ME PASE DE LA META: PIERDO EL COMBO
                    combo = 0;
                    puntos += frutas[i].valor;
                    crear_texto_flotante(frutas[i].pos, "TE PASASTE!", ORANGE);
                }
                else
                {
                    // SUBE EL COMBO Y MULTIPLICA LOS PUNTOS
                    if (combo < COMBO_MAX)
                        combo++;
                    puntos += frutas[i].valor * combo; // x1 HASTA x5
                    char texto[24];
                    sprintf(texto, "+%d x%d", frutas[i].valor, combo);
                    crear_texto_flotante(frutas[i].pos, texto,
                                         (combo >= COMBO_MAX) ? GOLD : GREEN);
                }

                valor_actual += frutas[i].valor;
                parpadeo_puntaje = 0.35f;
                chango.vel_y = REBOTE;
                chango.clavando = false;
                crear_particulas(frutas[i].pos, frutas[i].color, 14);
                frutas[i].activa = false;
            }
        }
    }

    // TOCAR EL SUELO: PIERDE VIDA, ROMPE COMBO Y REBOTA
    if (pies >= SUELO_Y)
    {
        chango.pos.y = SUELO_Y - chango.tam.y;
        if (chango.invulnerable <= 0)
        {
            chango.vida--;
            combo = 0;
            chango.invulnerable = INVULN;
            crear_texto_flotante((Vector2){chango.pos.x + chango.tam.x / 2,
                                           chango.pos.y},
                                 "SUELO! -1", ORANGE);
        }
        chango.vel_y = REBOTE;
        chango.clavando = false;
    }

    // REVISAR SI YA LLEGUE A LA META DEL NIVEL
    if (valor_actual < 0)
        valor_actual = 0;
    if (valor_actual >= config.meta)
    {
        puntos += 100;
        nivel_actual++;
        valor_actual = 0;
        config = cargar_config_nivel(nivel_actual);
        crear_texto_flotante((Vector2){V_ANCHO / 2.0f, 200}, "NIVEL!", GOLD);
    }

    // FIN DEL JUEGO POR QUEDARSE SIN VIDAS
    if (chango.vida <= 0)
    {
        iniciar_fin_juego();
        return;
    }

    // EFECTOS Y TECLAS SUELTAS
    if (parpadeo_puntaje > 0)
        parpadeo_puntaje -= dt;
    actualizar_particulas(dt);
    actualizar_textos_flotantes(dt);

    if (IsKeyPressed(KEY_F))
        filtro_activo = !filtro_activo;
    if (IsKeyPressed(KEY_R))
        gravedad = GRAVEDAD_DEFECTO; // REINICIA LA GRAVEDAD
    if (IsKeyPressed(KEY_ESCAPE))
        pantalla_actual = MENU;
}

void dibujar_puntaje_flotante()
{
    char texto[32];
    if (config.fase == ENTEROS)
        sprintf(texto, "%d", valor_actual);
    else if (config.fase == MINUTOS)
        sprintf(texto, "%d MIN", valor_actual);
    else
        sprintf(texto, "%dh %02dm",
                valor_actual / 60, valor_actual % 60);

    bool reaccion = (parpadeo_puntaje > 0);
    int tam = reaccion ? 34 : 28;
    Color color = reaccion ? GREEN : RAYWHITE;

    int cx = (int)(chango.pos.x + chango.tam.x / 2);
    int y = (int)(chango.pos.y - 28 - (reaccion ? 4 : 0));

    int ancho_texto = MeasureText(texto, tam);
    DrawRectangle(cx - ancho_texto / 2 - 8, y - 4, ancho_texto + 16, tam + 8, Fade(BLACK, 0.45f));
    dibujar_texto_centrado(texto, cx, y, tam, color);
}

void dibujar_interfaz()
{
    // OBJETIVO FIJO, GRANDE, ARRIBA Y AL CENTRO
    char objetivo[64];
    if (config.fase == ENTEROS)
        sprintf(objetivo, "OBJETIVO: %d", config.meta);
    else if (config.fase == MINUTOS)
        sprintf(objetivo, "OBJETIVO: %d MIN", config.meta);
    else
    {
        int horas = config.meta / 60;
        sprintf(objetivo, "OBJETIVO: %d HORA%s", horas, (horas == 1) ? "" : "S");
    }
    int ancho_obj = MeasureText(objetivo, 52);
    DrawRectangle(V_ANCHO / 2 - ancho_obj / 2 - 24, 12, ancho_obj + 48, 70, Fade(BLACK, 0.45f));
    dibujar_texto_centrado(objetivo, V_ANCHO / 2, 24, 52, YELLOW);

    // ESQUINA SUPERIOR IZQUIERDA: RELOJ Y GRAVEDAD
    int segundos = (int)tiempo_partida;
    Color color_reloj = (tiempo_partida < 15.0f) ? RED : RAYWHITE;
    DrawCircle(40, 42, 18, GOLD);
    DrawCircleLines(40, 42, 18, BLACK);
    DrawText(TextFormat("%d:%02d", segundos / 60, segundos % 60), 70, 20, 44, color_reloj);
    DrawText(TextFormat("GRAVEDAD: %.0f", gravedad), 24, 74, 20, SKYBLUE);

    // ESQUINA SUPERIOR DERECHA: NIVEL Y VIDAS
    DrawText(TextFormat("NIVEL %d", nivel_actual), V_ANCHO - 220, 20, 30, GOLD);
    for (int i = 0; i < chango.vida; i++)
        DrawCircle(V_ANCHO - 36 - i * 42, 78, 16, RED);

    // ABAJO AL CENTRO: BARRA DE COMBO
    dibujar_texto_centrado(TextFormat("COMBO  x%d", combo), V_ANCHO / 2, V_ALTO - 78, 28,
                           (combo >= COMBO_MAX) ? GOLD : RAYWHITE);
    int seg = 50, hueco = 6, alto_barra = 32;
    int ancho_total = COMBO_MAX * seg + (COMBO_MAX - 1) * hueco;
    int barra_x = V_ANCHO / 2 - ancho_total / 2;
    int barra_y = V_ALTO - 44;
    for (int i = 0; i < COMBO_MAX; i++)
    {
        Rectangle r = {(float)(barra_x + i * (seg + hueco)), (float)barra_y,
                       (float)seg, (float)alto_barra};
        DrawRectangleRec(r, (i < combo) ? GREEN : (Color){60, 60, 60, 255});
        DrawRectangleLinesEx(r, 2, BLACK);
    }
}

void dibujar_juego()
{
    // CIELO, TIERRA Y CESPED
    DrawRectangle(0, 0, V_ANCHO, SUELO_Y, (Color){110, 180, 230, 255});
    DrawRectangle(0, SUELO_Y, V_ANCHO, V_ALTO - SUELO_Y, (Color){90, 60, 35, 255});
    DrawRectangle(0, SUELO_Y, V_ANCHO, 8, (Color){70, 140, 60, 255});

    dibujar_frutas();
    dibujar_jugador();
    dibujar_puntaje_flotante();
    dibujar_particulas();
    dibujar_textos_flotantes();
    dibujar_interfaz();
}
