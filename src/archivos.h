#ifndef ARCHIVOS_H
#define ARCHIVOS_H

// Manejo de records en disco con fopen y fwrite. Los nombres solo
// aceptan letras.

#include "definis.h"
#include <stdio.h>
#include <string.h>

// Verifica que un caracter sea una letra del abecedario
bool es_letra(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// Lee la tabla de records o la inicializa vacia si no existe
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

// Escribe la tabla de records en el archivo binario
void guardar_records()
{
    FILE *archivo = fopen(ARCH_RECORDS, "wb");
    if (archivo == NULL)
        return;
    fwrite(records, sizeof(Record), MAX_RECORDS, archivo);
    fclose(archivo);
}

// Inserta un puntaje nuevo en la posicion que le toque del top
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
        return; // No alcanzo el top

    // Empujo hacia abajo desde el ultimo hasta el lugar libre
    for (int i = MAX_RECORDS - 1; i > lugar; i--)
        records[i] = records[i - 1];

    strncpy(records[lugar].nombre, nombre, MAX_NOMBRE);
    records[lugar].nombre[MAX_NOMBRE] = '\0';
    records[lugar].puntos = nuevos_puntos;

    guardar_records();
}

#endif
