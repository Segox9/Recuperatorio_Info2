#include "network.h"
#include <stdint.h>
uint16_t extract_segment16(uint16_t data, uint8_t startBit, uint8_t endBit)
{
    if (endBit < 16 && startBit <= endBit)
    {
        uint16_t mask = ((1U << (endBit - startBit + 1)) - 1) << startBit; // Crear la máscara de bits
        return (data & mask) >> startBit;                                  // Aplicar la máscara y desplazar
    }
    else
    {
        printf("\nRango de posiciones incorrecto\n");
        return 0;
    }
}
int countFile(FILE *f)
{
    uint16_t h, lldc;
    int c = 0;
    while (fread(&h, sizeof(uint16_t), 1, f))
    {
        lldc = extract_segment16(h, 0, 5);
        fseek(f, sizeof(uint16_t), SEEK_CUR);
        fseek(f, sizeof(uint16_t), SEEK_CUR);
        fseek(f, lldc * sizeof(uint16_t), SEEK_CUR);
        c++;
    }
    rewind(f);
    return c;
}
void loadData(Data *D, int n, FILE *f)
{
    uint16_t h;
    uint16_t lldc, upid;
    uint16_t tpnh, tpnl;
    uint8_t aux, info;
    int i = 0;
    while (i < n)
    {
        fread(&h, sizeof(uint16_t), 1, f);
        D[i].ID = extract_segment16(h, 6, 15);
        lldc = extract_segment16(h, 0, 5);
        D[i].Lldc = lldc;
        fread(&h, sizeof(uint16_t), 1, f);
        tpnl = extract_segment16(h, 3, 3);
        aux = extract_segment16(h, 12, 12);
        tpnh = aux << 1;
        D[i].type = (uint8_t)(tpnh | tpnl);
        // Si el dispositivo es cpu o concentrador tendra equipos conectados por debajo de el
        if (D[i].type == 0 || D[i].type == 3)
        {
            D[i].Id_inf = new uint16_t[lldc];
        }
        if (D[i].type == 1)
        { // es un sensor
            info = (uint8_t)extract_segment16(h, 8, 9);
            D[i].info = info;
        }
        if (D[i].type == 2)
        { // es un actuador
            info = (uint8_t)extract_segment16(h, 6, 6);
            D[i].info = info;
        }
        // Leo y guardo el id superior
        fread(&h, sizeof(uint16_t), 1, f);
        upid = extract_segment16(h, 3, 12);
        D[i].Id_up = upid;
        // Leo y guardo los ids despues de cada header
        if (lldc > 0)
        {
            for (int j = 0; j < lldc; j++)
            {
                fread(&h, sizeof(uint16_t), 1, f);
                D[i].Id_inf[j] = h;
            }
        }
        i++;
    }
}
void deleteMemory(Data *v, int n)
{
    for (int i = 0; i < (v[i].Lldc); i++)
    {
        if (v[i].Lldc > 0)
        {
            delete[] v[i].Id_inf;
        }
        else
        {
            delete[] v[i].Id_inf;
        }
    }
    delete[] v;
}
void secuenciaIDs(uint16_t userID, Data *v, int n)
{
    // busco la posicion en la que esta el id ingresado por el usuario
    int posid = busquedaID(v, n, userID);
    int cantids = 0;
    // cuento cuantos ids tiene antes el id ingresado por el usuario
    while (v[posid].Id_up != 1023)
    {
        cantids++;
        posid = busquedaID(v, n, v[posid].Id_up);
        if (posid == -1)
        {
            break; // salgo del bucle con un break
        }
    }
    int *vid = new int[cantids + 1]; // +1 para incluir el ID de nivel superior
    posid = busquedaID(v, n, userID);
    for (int i = 0; i <= cantids; i++) {
        vid[i] = v[posid].ID;
        posid = busquedaID(v, n, v[posid].Id_up);
    }
    printf("\n");
    // Imprimir la cadena
    for (int i = cantids; i >= 0; i--) {
        printf("ID %d", vid[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
    delete[] v;
}
int busquedaID(Data *v, int n, uint16_t ID)
{
    for (int i = 0; i < n; i++)
    {
        if (v[i].ID == ID)
        {
            return i;
        }
    }
    return -1; // si el ID no se encuentra, retorna un -1
}
void typeEquipo(Data *d, int n)
{
    int cpu = 0, conc = 0;
    int sens_temp = 0, sens_pres = 0, sens_flow = 0, sens_level = 0;
    int valv = 0, motor = 0;
    for (int i = 0; i < n; i++)
    {
        if (d[i].type == 0)
        { // es cpu
            cpu++;
        }
        if (d[i].type == 1)
        { // es sensor
            if (d[i].info == 0)
            { // flow
                sens_flow++;
            }
            if (d[i].info == 1)
            { // temp
                sens_temp++;
            }
            if (d[i].info == 2)
            { // presure
                sens_pres++;
            }
            if (d[i].info == 3)
            { // level
                sens_level++;
            }
        }
        if (d[i].type == 2)
        { // es actuador
            if (d[i].info == 0)
            { // valve
                valv++;
            }
            if (d[i].info == 1)
            { // valve
                motor++;
            }
        }
        if (d[i].type == 3)
        { // es concentrador
            conc++;
        }
    }
    printf("\nEquipos en la red");
    printf("\nCPU: %d", cpu);
    printf("\nConcentrador: %d", conc);
    if (motor > 0)
    {
        printf("\nMotor: %d", motor);
    }
    if (valv > 0)
    {
        printf("\nValve: %d", valv);
    }
    if (sens_flow > 0)
    {
        printf("\nSensor flow: %d", sens_flow);
    }
    if (sens_level > 0)
    {
        printf("\nSensor level: %d", sens_level);
    }
    if (sens_pres > 0)
    {
        printf("\nSensor presure: %d", sens_pres);
    }
    if (sens_temp > 0)
    {
        printf("\nSensor temperature: %d", sens_temp);
    }
}