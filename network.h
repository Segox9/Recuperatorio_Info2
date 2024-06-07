#pragma once
#include <stdio.h>
#include <stdint.h>
struct data{
    uint16_t ID;
    uint16_t Id_up;
    uint16_t *Id_inf;
    uint8_t info;
    uint8_t type;
    uint8_t Lldc;
};
typedef struct data Data;

int countFile(FILE *f);
void secuenciaIDs(uint16_t ID, Data *v, int n);
void readFile(FILE *f);
void loadData(Data *d, int n, FILE *f);
void deleteMemory(Data *v, int n);
void typeEquipo(Data *d, int n);
int busquedaID(Data *v, int n, uint16_t ID);