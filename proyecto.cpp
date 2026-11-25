#include <iostream>
#include <vector>
#include <queue>
#include <ctime>
#include <limits>
#include <conio.h>
using namespace std;

struct Bloque {
    int id;
    int tam;
    int cuanto;
    int cicloActual;
};

struct Simulador {
    const int particionMin = 32;
    vector<Bloque> memoria;
    queue<int> cola_rr;

    int tamMemoria = 4096;
    int cuantoSistema = 2;
    int tamMaxProceso = 512;
    int cuantoMaxProceso = 10;
    int algoritmo = 1;// 1=primer ajuste, 2=mejor ajuste
    int totalCiclos=0;

};

void capturar(struct Simulador &simulador){
    int opcionMemoria;
    do {
        cout << "1. Seleccione tamano fijo de memoria (1 = 1MB, 4 = 4MB, 8 = 8MB): ";
        cin >> opcionMemoria;
    } while (opcionMemoria != 1 && opcionMemoria != 4 && opcionMemoria != 8);
    simulador.tamMemoria = opcionMemoria * 1024;

    do {
        cout << "2. Seleccione Algoritmo (1 = Primer Ajuste, 2 = Mejor Ajuste): ";
        cin >> simulador.algoritmo;
    } while (simulador.algoritmo != 1 && simulador.algoritmo != 2);

    do {
        cout << "3. Cuanto del sistema: ";
        cin >> simulador.cuantoSistema;
    } while (simulador.cuantoSistema <= 0);

    do {
        cout << "4. Tamano maximo de memoria por proceso en KB (32 - " << simulador.tamMemoria << "): ";
        cin >> simulador.tamMaxProceso;
    } while (simulador.tamMaxProceso < simulador.particionMin || simulador.tamMaxProceso > simulador.tamMemoria);
     do {
        cout << "5. Tamano maximo del cuanto por proceso: ";
        cin >> simulador.cuantoMaxProceso;
    } while (simulador.cuantoMaxProceso <= 0);

    cout << "Presiona |s| o |S| para salir del simulador" << endl;
}

int BuscarProc(const vector<Bloque> &memoria, int indice){
    for(int i=0; i<static_cast<int> (memoria.size()); i++){
        if(memoria[i].id==indice){
            return i;
        }
    }
    return -1;
}

int primerAjuste(const vector<Bloque>& memoria, int tamRequerido) {
    for (int i = 0; i < static_cast<int>(memoria.size()); i++) {
        if (memoria[i].id == 0 && memoria[i].tam >= tamRequerido) {
            return i; 
        }
    }
    return -1;
}

int mejorAjuste(const vector<Bloque>& memoria, int tamRequerido) {
    int mejorIndice = -1;

    for (int i = 0; i < static_cast<int>(memoria.size()); i++) {
        if (memoria[i].id == 0 && memoria[i].tam >= tamRequerido) {
            if (mejorIndice == -1 || memoria[i].tam < memoria[mejorIndice].tam) {
                mejorIndice = i;
            }
        }
    }
    return mejorIndice;
}

bool colocarProceso(Simulador& simulador, int id, int tam, int cuanto) {
    int indice = -1;

    if (simulador.algoritmo == 1) {
        indice = primerAjuste(simulador.memoria, tam);
    } else {
        indice =  mejorAjuste(simulador.memoria, tam);
    }
    if (indice == -1) {
        return false; // No hay espacio disponible
    }

    int sobrante = simulador.memoria[indice].tam - tam;
    if (sobrante >= simulador.particionMin) {
        simulador.memoria[indice].id = id;
        simulador.memoria[indice].tam = tam;
        simulador.memoria[indice].cuanto = cuanto;
        simulador.memoria[indice].cicloActual = simulador.totalCiclos;

        Bloque nuevo_hueco = {0, sobrante, 0, 0};
        simulador.memoria.insert(simulador.memoria.begin() + indice + 1, nuevo_hueco);
    } else {
        simulador.memoria[indice].id = id;
        simulador.memoria[indice].cuanto = cuanto;
        simulador.memoria[indice].cicloActual = simulador.totalCiclos;
    }

    simulador.cola_rr.push(id);
    return true;
}

void imprimirLista(Simulador &simulador){
    if(simulador.cola_rr.empty()){
        cout <<"LISTA VACIA" << endl;
    }else{
        queue<int> temp;
        temp=simulador.cola_rr;
        cout << "Lista RR: ";
        while(!temp.empty()){
            int proc= temp.front();
            temp.pop();
            int indice= BuscarProc(simulador.memoria, proc);
            if(indice!=-1){
                const Bloque& proceso =simulador.memoria[indice];
                cout <<"["<< proceso.id<<","<<proceso.tam<<","<<proceso.cuanto<<"]";
            }
        }
        cout << endl;
    }
}

void imprimirMemoria(const vector<Bloque>&memoria){
    for (const Bloque &b: memoria){
        cout <<"["<< b.id<<","<<b.tam<<","<<b.cuanto<<"]";
    }
    cout<<"\n";
}

void condensarHueco(vector<Bloque> &memoria){
    bool condensacion = false;
    int i = 0;

    while(i+1 < static_cast<int>(memoria.size())){
        if(memoria[i].id == 0 && memoria[i+1].id == 0){
            memoria[i].tam += memoria[i+1].tam;
            memoria.erase(memoria.begin()+i+1);
            condensacion = true;
        } else {
            i++;
        }
    }
    if(condensacion){
        cout << "Condensando huecos: " << endl;
        imprimirMemoria(memoria);
    }
}

void randomProceso(Bloque &procesoBase, const Simulador &simulador){
    procesoBase.id++;
    procesoBase.tam = simulador.particionMin + (rand()% (simulador.tamMaxProceso - simulador.particionMin + 1));
    procesoBase.cuanto = 1 + (rand()% simulador.cuantoMaxProceso);
    procesoBase.cicloActual = 0;
}

void RounRobin(Simulador &simulador){
    if(simulador.cola_rr.empty()){
        return;
    }
    int atenderproc= simulador.cola_rr.front();
    simulador.cola_rr.pop();
    int indice=BuscarProc(simulador.memoria, atenderproc);
    if(indice !=-1){
        Bloque &proceso= simulador.memoria[indice];
        cout << "Atendiendo: " << endl;
        cout <<"["<< proceso.id<<","<<proceso.tam<<","<<proceso.cuanto<<"]\n";
        proceso.cuanto -= simulador.cuantoSistema;
        if(proceso.cuanto>0){ 
            simulador.cola_rr.push(proceso.id);
            imprimirLista(simulador);
            imprimirMemoria(simulador.memoria);
        }else {
            proceso.cuanto=0;
            imprimirLista(simulador);
            cout << "Descargando: "<< endl;;
            cout <<"["<< proceso.id<<","<<proceso.tam<<","<<proceso.cuanto<<"]\n";
            proceso.id=0;
            imprimirMemoria(simulador.memoria);
            condensarHueco(simulador.memoria);
        }
    }
}
bool detenerSimulador(Simulador &simulador){
  while (_kbhit()) {
        char tecla = static_cast<char>(_getch());
        if (tecla == 's' || tecla == 'S') {
            cout << "\nSimulador detenido por el usuario. Total de ciclos: " << simulador.totalCiclos << endl;
            return true;
        }
    }
    return false;
}

void retrasarElCiclo(int milisegundos) {
    clock_t start_time = clock();
    while (clock() < start_time + milisegundos * CLOCKS_PER_SEC / 3000);
}

void verificarHuecos(const Bloque &proceso, const Simulador &simulador) {
    int cont = 0;
    int correcto = 0;

    for (size_t i = 0; i < simulador.memoria.size(); ++i) {
        if (simulador.memoria[i].id == 0) {
            cont++;
            if (proceso.tam <= simulador.memoria[i].tam) {
                correcto++;
            }
        }
    }

    if (correcto >= 3) {
        cout << "--------------------------------------------------------------------" << endl;
        cout << "Huecos libres: " << cont <<endl;
        cout << "------------------------------------------------------------------------" << endl;
    }
}


int main() {
    srand(static_cast<unsigned int>(time(0)));
    Simulador s;
    capturar(s);
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //limpiar buffer 

    s.memoria.push_back({0, s.tamMemoria, 0, 0});
    Bloque proceso = {0, 0, 0, 0};
    randomProceso(proceso, s);

    bool salir = false;
    while (!salir) {
        s.totalCiclos++;
        cout << "CICLO " << s.totalCiclos << endl;

        if (colocarProceso(s, proceso.id, proceso.tam, proceso.cuanto)) {

            Bloque entrante = proceso; // Proceso que sí entró en este ciclo
            cout << "Entrando: [" << entrante.id << "," << entrante.tam << "," << entrante.cuanto << "]" << endl;
            randomProceso(proceso, s);
            imprimirMemoria(s.memoria);
            verificarHuecos(entrante, s);
        
        } else {
            cout << "proceso: " << proceso.id << " en espera" << endl;
        }

        RounRobin(s);
        retrasarElCiclo(2000);
        
        if (detenerSimulador(s)) {
            salir = true;
        }
        cout << endl;
    }

    return 0;
}

