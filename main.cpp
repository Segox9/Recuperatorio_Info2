#include <stdio.h>
#include "network.h"

int main(){

    FILE *f = fopen("C:\\Users\\lauta\\Desktop\\SEGOVIA LAUTARO\\Parciales\\Info2\\Recuperatorio\\network_structure.dat","rb");
    if(f == NULL ){
        printf("error al abrir el archivo\n");
        return 0;
    }
    int cant= countFile(f);
    Data *v= new Data[cant];
    loadData(v, cant, f);
    uint16_t userID;
    int index;
    printf("\nBienvendio, ingrese el ID del equipo que desea ver su coneccion:\n");
    do {
        scanf("%d", &userID);
        index = busquedaID(v, cant, userID);
        if (index == -1) {
            printf("\n ID no encontrado. Por favor ingrese un ID valido.");
        }
    } while (index == -1);
    secuenciaIDs(userID, v, cant);
    typeEquipo(v, cant);
    deleteMemory(v, cant);
    fclose(f);

}