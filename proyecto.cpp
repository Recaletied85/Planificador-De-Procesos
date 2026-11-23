#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct Bloque {
    int id;
    int tamano;
    int cuanto;
    int ciclo_entrada;
};

struct Simulador {
    const int MIN_SPLIT = 32;
    vector<Bloque> memoria;
    queue<int> cola_rr;

    int tam_memoria_kb = 4096;
    int cuanto_sistema = 2;
    int max_tam_proceso = 512;
    int max_cuanto_proceso = 10;
    int algoritmo = 1;// 1=primer ajuste, 2=mejor ajuste
    int delay_ms = 800;

    int total_procesos_atendidos = 0;
    int suma_ciclos_atencion = 0;
    double suma_porcentaje_ocupacion = 0.0;
    int total_ciclos = 0;
};
void capturar(struct Simulador &sim){
    int opcionMemoria;
    do {
        cout << "1. Seleccione tamano fijo de memoria (1 = 1MB, 4 = 4MB, 8 = 8MB): ";
        cin >> opcionMemoria;
    } while (opcionMemoria != 1 && opcionMemoria != 4 && opcionMemoria != 8);
    sim.tam_memoria_kb = opcionMemoria * 1024;

    do {
        cout << "2. Seleccione Algoritmo (1 = Primer Ajuste, 2 = Mejor Ajuste): ";
        cin >> sim.algoritmo;
    } while (sim.algoritmo != 1 && sim.algoritmo != 2);

    do {
        cout << "3. Cuanto del sistema: ";
        cin >> sim.cuanto_sistema;
    } while (sim.cuanto_sistema <= 0);

    do {
        cout << "4. Tamano maximo de memoria por proceso en KB (32 - " << sim.tam_memoria_kb << "): ";
        cin >> sim.max_tam_proceso;
    } while (sim.max_tam_proceso < sim.MIN_SPLIT || sim.max_tam_proceso > sim.tam_memoria_kb);
     do {
        cout << "5. Tamano maximo del cuanto por proceso: ";
        cin >> sim.max_cuanto_proceso;
    } while (sim.max_cuanto_proceso <= 0);
}

int primerAjuste(const vector<Bloque>& memoria, int tamRequerido) {
    for (int i = 0; i < static_cast<int>(memoria.size()); i++) {
        if (memoria[i].id == 0 && memoria[i].tamano >= tamRequerido) {
            return i; 
        }
    }
    return -1;
}

int mejorAjuste(const vector<Bloque>& memoria, int tamRequerido) {
    int mejorIndice = -1;

    for (int i = 0; i < static_cast<int>(memoria.size()); i++) {
        if (memoria[i].id == 0 && memoria[i].tamano >= tamRequerido) {
            if (mejorIndice == -1 || memoria[i].tamano < memoria[mejorIndice].tamano) {
                mejorIndice = i;
            }
        }
    }
    return mejorIndice;
}

bool colocarProceso(Simulador& sim, int id, int tam, int cuanto) {
    int indice = -1;

    if (sim.algoritmo == 1) {
        indice = primerAjuste(sim.memoria, tam);
    } else {
        indice =  mejorAjuste(sim.memoria, tam);
    }
    if (indice == -1) {
        return false; // No hay espacio disponible
    }

    int sobrante = sim.memoria[indice].tamano - tam;
    if (sobrante >= sim.MIN_SPLIT) {
        sim.memoria[indice].id = id;
        sim.memoria[indice].tamano = tam;
        sim.memoria[indice].cuanto = cuanto;
        sim.memoria[indice].ciclo_entrada = sim.total_ciclos;

        Bloque nuevo_hueco = {0, sobrante, 0, 0};
        sim.memoria.insert(sim.memoria.begin() + indice + 1, nuevo_hueco);
    } else {
        sim.memoria[indice].id = id;
        sim.memoria[indice].cuanto = cuanto;
        sim.memoria[indice].ciclo_entrada = sim.total_ciclos;
    }

    sim.cola_rr.push(id);
    return true;
}
int main(){
    Simulador s;
    capturar(s);
    return 0;
}
