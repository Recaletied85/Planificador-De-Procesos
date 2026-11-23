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

    // Contadores de estadísticas
    int total_procesos_atendidos = 0;
    int suma_ciclos_atencion = 0;
    double suma_porcentaje_ocupacion = 0.0;
    int total_ciclos = 0;
};

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
    int idx = -1;

    if (sim.algoritmo == 1) {
        idx = primerAjuste(sim.memoria, tam);
    } else {
        idx =  mejorAjuste(sim.memoria, tam);
    }
    if (idx == -1) {
        return false; // No hay espacio disponible
    }

    int sobrante = sim.memoria[idx].tamano - tam;
    if (sobrante >= sim.MIN_SPLIT) {
        sim.memoria[idx].id = id;
        sim.memoria[idx].tamano = tam;
        sim.memoria[idx].cuanto = cuanto;
        sim.memoria[idx].ciclo_entrada = sim.total_ciclos;

        Bloque nuevo_hueco = {0, sobrante, 0, 0};
        sim.memoria.insert(sim.memoria.begin() + idx + 1, nuevo_hueco);
    } else {
        sim.memoria[idx].id = id;
        sim.memoria[idx].cuanto = cuanto;
        sim.memoria[idx].ciclo_entrada = sim.total_ciclos;
    }

    sim.cola_rr.push(id);
    return true;
}
