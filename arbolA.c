#include <stdlib.h>
#include <stdbool.h>


/**
 * @brief Par llave-costo almacenado en el arbol B
 */
typedef struct par{
    int llave;
    float valor;
}par;

/**
 * @brief Nodo del arbol B de tamaño fijo de 4096 bytes
 */
typedef struct nodoArbolA{ //4096 bytes
    int es_interno; //1 o 0
    int k;  // entre b/2 - 1 y b
    par llaves_valores[340]; // arreglo de b=340 max pares, tamaño estatico y ordenado
    int hijos[341];
    int padding;
}nodoArbolA;

/**
 * @brief Resultado de hacer split a un arbol B
 */
typedef struct resSplit{
    nodoArbolA *izq;
    nodoArbolA *der;
    par mediano;
}resSplit;

/**
 * @brief Divide un nodo lleno en dos nodos y obtiene su mediana
 * 
 * @param nodo puntero al nodo que se dividira
 * @return Estructura con el nodo izquierdo, derecho y la mediana
 */
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

/**
 * @brief Inserta un par llave-valor a un arbol B
 * 
 * @param nodos Arreglo de nodos
 * @param nuevo Par llave-valor que se desea insertar
 * @param indiceAct Indice del nodo actual dentro del arreglo
 * @param cantidadNodo Puntero a la cantidad de nodos del arbol
 * @param esRaiz Indica si el nodo actual es raiz
 * @param contadorIOs Puntero al contador de I/Os
 */
void insertar(nodoArbolA *nodos, par nuevo, int indiceAct, int *cantidadNodo, bool esRaiz, int * contadorIOs){
    nodoArbolA *actual = &nodos[indiceAct];
    (*contadorIOs)++;
    if(actual->es_interno==0 && !esRaiz){
        int pos = actual->k-1;
        while(pos>=0 && actual->llaves_valores[pos].llave > nuevo.llave){
            actual->llaves_valores[pos+1] = actual->llaves_valores[pos];
            pos--;
        }
        actual->llaves_valores[pos+1] = nuevo;
        actual->k++;
        (*contadorIOs)++;
    }
    else if(actual->es_interno==1 && !esRaiz){
        int donde = 0;
        while(donde<actual->k && actual->llaves_valores[donde].llave < nuevo.llave){
            donde++;
        }
        int indiceHijo = actual->hijos[donde];
        nodoArbolA *hijo = &nodos[indiceHijo];
        (*contadorIOs)++;
        if(hijo->k<340){ //caso que no este lleno
            insertar(nodos, nuevo, indiceHijo, cantidadNodo, false, contadorIOs);
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
            (*contadorIOs)++;
            nodos[*cantidadNodo] = *result.der;
            (*contadorIOs)++; 
            //d)
            for(int i = actual->k + 1; i> donde + 1; i--){
                actual->hijos[i] = actual->hijos[i-1];
            }
            actual->hijos[donde] = indiceHijo;
            actual->hijos[donde+1] = *cantidadNodo;
            (*cantidadNodo)++;
            //e)
            actual->k++ ;
            (*contadorIOs)++;
            //g)
            if(nuevo.llave<=result.mediano.llave){
                insertar(nodos, nuevo, indiceHijo, cantidadNodo, false, contadorIOs);
            }
            else{
                insertar(nodos, nuevo, *cantidadNodo-1, cantidadNodo, false, contadorIOs);
            }
            free(result.izq);
            free(result.der);
        }
    }
    else if(esRaiz){ //caso de que es raiz
        if(actual->k<340){ //caso no lleno
            insertar(nodos, nuevo, indiceAct, cantidadNodo, false, contadorIOs);
        }
        else{ //caso lleno
            resSplit result = split(actual);

            nodos[*cantidadNodo] = *result.izq;
            (*contadorIOs)++;
            (*cantidadNodo)++;
            nodos[*cantidadNodo] = *result.der;
            (*contadorIOs)++;
            (*cantidadNodo)++;

            nodoArbolA *nuevaRaiz = malloc(sizeof(nodoArbolA));

            nuevaRaiz->es_interno = 1;
            nuevaRaiz->k = 1;
            nuevaRaiz->llaves_valores[0] = result.mediano;
            nuevaRaiz->hijos[0] = *cantidadNodo-2;
            nuevaRaiz->hijos[1] = *cantidadNodo-1;
            nuevaRaiz->padding = 0;

            nodos[0] = *nuevaRaiz;
            (*contadorIOs)++;

            if(nuevo.llave<=result.mediano.llave){
                insertar(nodos, nuevo, *cantidadNodo-2, cantidadNodo, false, contadorIOs);
            }
            else{
                insertar(nodos, nuevo, *cantidadNodo-1, cantidadNodo, false,contadorIOs);
            }
            free(result.izq);
            free(result.der);
        }
    }
}

/**
 * @brief Nodo de una lista enlazada de pares llave-valor
 */
typedef struct lista{
    par valor;
    struct lista *sig;
}lista;

/**
 * @brief Agrega un par al inicio de la lista
 * 
 * @param list Lista donde se va agregar el nuevo par
 * @param nuevo Par que se quiere agregar a la lista
 * @return Puntero al inicio de la lista actualizada.
 */
lista *agregar(lista *list, par nuevo){
    lista *nueva_lista = malloc(sizeof(lista));
    nueva_lista->valor = nuevo;
    nueva_lista->sig = list;
    return nueva_lista;
}

/**
 * @brief Une dos listas
 * 
 * @param uno primera lista que se quiere unir
 * @param dos segunda lista que se quiere unir
 * @return Puntero al inicio de la lista resultante
 */
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

/**
 * @brief Busca los pares cuyas llaves pertenezcan al rango
 * 
 * @param nodos Arreglo de nodos
 * @param indiceAct Indice del nodo en el que se buscara
 * @param desde Limite inferior del rango
 * @param hasta Limite superior del rango
 * @param contadorIOs Puntero al contador de I/Os
 * @return lista enlazada de los pares encontrados
 */
lista *busqueda_por_rango(nodoArbolA *nodos, int indiceAct, int desde, int hasta, int *contadorIOs){
    nodoArbolA *actual = &nodos[indiceAct];
    (*contadorIOs)++;
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
                list = unir(list, busqueda_por_rango(nodos,actual->hijos[i],desde,hasta, contadorIOs));
            }
        }
        return list;
    }
}

/**
 * @brief Crea un arbol e inserta los pares llave-valores recibidos
 * 
 * @param pares Arreglo de pares llave-valor a insertar
 * @param n cantidad de pares a insertar
 * @param cantidadNodos Puntero donde se guardan la cantidad de nodos del arbol
 * @param capacidad Capacidad maxima del arreglo de nodos
 * @param contadorIOs Puntero al contador de I/Os
 * @return Puntero al arreglo de nodos del arbol
 */
nodoArbolA *crearArbol(par *pares, int n, int *cantidadNodos, int capacidad, int *contadorIOs){
    nodoArbolA *nodos = malloc(capacidad * sizeof(nodoArbolA));

    nodos[0].es_interno = 0;
    nodos[0].k = 0;
    nodos[0].padding = 0;
    *cantidadNodos = 1;

    for(int i = 0; i<n; i++){
        insertar(nodos, pares[i], 0, cantidadNodos, true, contadorIOs);
    }
    return nodos;
}
