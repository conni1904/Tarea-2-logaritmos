#include <stdlib.h>
#include <stdbool.h>


/**
 * @brief Par que almacena el costo asociado a un nodo
 */
typedef struct par{
    int llave;
    float valor;
}par;

/**
 * @brief 
 */
typedef struct nodoArbolA{ //4096 bytes
    int es_interno; //1 o 0
    int k;  // entre b/2 - 1 y b
    par llaves_valores[340]; // arreglo de b=340 max pares, tamaño estatico y ordenado
    int hijos[341];
    int padding;
}nodoArbolA;

typedef struct resSplit{
    nodoArbolA *izq;
    nodoArbolA *der;
    par mediano;
}resSplit;

resSplit split(nodoArbolA *nodo){
    resSplit retorno;

    retorno.mediano = nodo->llaves_valores[169];

    nodoArbolA *izq = malloc(sizeof(nodoArbolA));
    nodoArbolA *der = malloc(sizeof(nodoArbolA));
    izq->es_interno = nodo->es_interno;
    der->es_interno = nodo->es_interno;
    izq->k = 169;
    der->k = 170;
    izq->padding = 0;
    der->padding = 0;

    for(int i = 0; i<izq->k; i++){
        izq->llaves_valores[i] = nodo->llaves_valores[i];
    }
    for(int i = 0; i<der->k; i++){
        der->llaves_valores[i] = nodo->llaves_valores[170 + i];
    }
    
    if(nodo->es_interno == 1){ //es interno
        for(int i = 0; i<=izq->k; i++){
            izq->hijos[i] = nodo->hijos[i];
        }
        for(int i = 0; i<=der->k; i++){
            der->hijos[i] = nodo->hijos[170 + i];
        }
    }

    retorno.izq = izq;
    retorno.der = der;

    return retorno;
}

void insertar(nodoArbolA *nodos, par nuevo, int indiceAct, int *cantidadNodo, bool esRaiz){
    nodoArbolA *actual = &nodos[indiceAct];
    if(actual->es_interno==0 && !esRaiz){
        int pos = actual->k-1;
        while(pos>=0 && actual->llaves_valores[pos].llave > nuevo.llave){
            actual->llaves_valores[pos+1] = actual->llaves_valores[pos];
            pos--;
        }
        actual->llaves_valores[pos+1] = nuevo;
        actual->k++;
    }
    else if(actual->es_interno==1 && !esRaiz){
        int donde = 0;
        while(donde<actual->k && actual->llaves_valores[donde].llave < nuevo.llave){
            donde++;
        }
        int indiceHijo = actual->hijos[donde];
        nodoArbolA *hijo = &nodos[indiceHijo];
        if(hijo->k<340){ //caso que no este lleno
            insertar(nodos, nuevo, indiceHijo, cantidadNodo, false);
        }
        else{ //caso si lleno
            //a)
            resSplit result = split(hijo);

            //b)
            int pos = actual->k-1;
            while(pos>=0 && actual->llaves_valores[pos].llave > result.mediano.llave){
                actual->llaves_valores[pos+1] = actual->llaves_valores[pos];
                pos--;
            }
            actual->llaves_valores[pos+1] =result.mediano;
            //c)
            nodos[indiceHijo] = *result.izq;
            nodos[*cantidadNodo] = *result.der; 
            //d)
            for(int i = actual->k + 1; i> donde + 1; i--){
                actual->hijos[i] = actual->hijos[i-1];
            }
            actual->hijos[donde] = indiceHijo;
            actual->hijos[donde+1] = *cantidadNodo;
            (*cantidadNodo)++;
            //e)
            actual->k++ ;
            //g)
            if(nuevo.llave<=result.mediano.llave){
                insertar(nodos, nuevo, indiceHijo, cantidadNodo, false);
            }
            else{
                insertar(nodos, nuevo, *cantidadNodo-1, cantidadNodo, false);
            }
            free(result.izq);
            free(result.der);
        }
    }
    else if(esRaiz){ //caso de que es raiz
        if(actual->k<340){ //caso no lleno
            insertar(nodos, nuevo, indiceAct, cantidadNodo, false);
        }
        else{ //caso lleno
            resSplit result = split(actual);

            nodos[*cantidadNodo] = *result.izq;
            (*cantidadNodo)++;
            nodos[*cantidadNodo] = *result.der;
            (*cantidadNodo)++;

            nodoArbolA *nuevaRaiz = malloc(sizeof(nodoArbolA));

            nuevaRaiz->es_interno = 1;
            nuevaRaiz->k = 1;
            nuevaRaiz->llaves_valores[0] = result.mediano;
            nuevaRaiz->hijos[0] = *cantidadNodo-2;
            nuevaRaiz->hijos[1] = *cantidadNodo-1;
            nuevaRaiz->padding = 0;

            nodos[0] = *nuevaRaiz;

            if(nuevo.llave<=result.mediano.llave){
                insertar(nodos, nuevo, *cantidadNodo-2, cantidadNodo, false);
            }
            else{
                insertar(nodos, nuevo, *cantidadNodo-1, cantidadNodo, false);
            }
            free(result.izq);
            free(result.der);
        }
    }
}

typedef struct lista{
    par valor;
    struct lista *sig;
}lista;

lista *agregar(lista *list, par nuevo){
    lista *nueva_lista = malloc(sizeof(lista));
    nueva_lista->valor = nuevo;
    nueva_lista->sig = list;
    return nueva_lista;
}

lista *unir(lista *uno, lista *dos){
    if(uno==NULL){
        return dos;
    }
    lista *nueva = uno;
    while(uno->sig!=NULL){
        uno = uno->sig;
    }
    uno->sig=dos;
    return nueva;
}

lista *busqueda_por_rango(nodoArbolA *nodos, int indiceAct, int desde, int hasta){
    nodoArbolA *actual = &nodos[indiceAct];
    lista *list = NULL;
    if(actual->es_interno==0){//hoja
        for(int i=0; i<actual->k; i++){
            par act = actual->llaves_valores[i];
            if(act.llave>=desde && act.llave<=hasta){
                list = agregar(list, act);
            }
        }
        return list;
    }
    else{
        for(int i=0; i<actual->k; i++){
            par act = actual->llaves_valores[i];
            if(act.llave>=desde && act.llave<=hasta){
                list = agregar(list, act);
            }
        }
        for(int i=0; i<=actual->k; i++){
            if((i==0 ||hasta>actual->llaves_valores[i-1].llave) && (i==actual->k || desde<=actual->llaves_valores[i].llave)){
                list = unir(list, busqueda_por_rango(nodos,actual->hijos[i],desde,hasta));
            }
        }
        return list;
    }
}

nodoArbolA *crearArbol(par *pares, int n, int *cantidadNodos, int capacidad){
    nodoArbolA *nodos = malloc(capacidad * sizeof(nodoArbolA));

    nodos[0].es_interno = 0;
    nodos[0].k = 0;
    nodos[0].padding = 0;
    *cantidadNodos = 1;

    for(int i = 0; i<n; i++){
        insertar(nodos, pares[i], 0, cantidadNodos, true);
    }
    return nodos;
}
