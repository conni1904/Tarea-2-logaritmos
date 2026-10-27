#include "arbolA.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



int rangos_aleatorios(){
    double factor = (double) rand() / (double) RAND_MAX;
    return (int) (1546300800 + factor * (1754006400 - 1546300800));

}

int main(){
    int N= pow(2, 15);
    FILE *f_datos = fopen("datos.bin", "rb");
    par *buffer_datos = (par*) malloc(N * sizeof(par));
    size_t leidos = fread(buffer_datos, sizeof(par), N, f_datos);
    fclose(f_datos);

    int cantidad_nodos = 0;
    int capacidad = (int)(N / 169) + 100;
    int cantidadIOs = 0;
    clock_t inif = clock();
    nodoArbolA *arbol = crearArbol(buffer_datos, N, &cantidad_nodos, capacidad, &cantidadIOs);
    clock_t finf = clock();
    double tiempoCA = (double)(finf-inif)/ CLOCKS_PER_SEC;
    printf("Tiempo creacion arbol A: %.4f s \n", tiempoCA);
    printf("Cantidad de Nodos arbol A: %d\n", cantidad_nodos);
    printf("Cantidad de I/Os en insercion: %d\n", cantidadIOs);

    //aca debe ir lo mismo pero con el arbol B epsilon (2 veces...)


    for(int i =0; i<50; i++){
        int l = rangos_aleatorios();
        int u = l + 640800;

        int contadorIOs = 0;
        clock_t ini = clock();
        lista *resultado = busqueda_por_rango(arbol, 0, l, u, &contadorIOs);
        clock_t fin = clock();
        double tiempoBA = (double)(finf-inif)/ CLOCKS_PER_SEC;
        printf("Tiempo busqueda arbol A: %.4f s \n", tiempoBA);
        printf("Cantidad de I/Os en busqueda: %d\n", contadorIOs);

        //aca va lo mismo para los arboles Bepsilon
    }
    
}
