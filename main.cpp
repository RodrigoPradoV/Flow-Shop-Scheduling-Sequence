/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: valro
 *
 * Created on 24 de noviembre de 2024, 10:10 p. m.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;

#define ITERACIONES 1000
#define ALFA 0.5

const int MAX_TAREAS = 100;
const int MAX_MAQUINAS = 100;

// Estructura para representar una tarea
struct Tarea {
    int id;
    vector<int> tiemposProcesamiento;
};

// Función para calcular el makespan utilizando matrices estáticas
int calcularMakespan(const vector<Tarea>& tareas, const vector<int>& orden, int numMaquinas) {
    int tiemposFinalizacion[MAX_TAREAS + 1][MAX_MAQUINAS + 1] = {0};

    for (size_t i = 1; i <= orden.size(); ++i) {
        for (int j = 1; j <= numMaquinas; ++j) {
            tiemposFinalizacion[i][j] = max(tiemposFinalizacion[i - 1][j], tiemposFinalizacion[i][j - 1]) +
                                        tareas[orden[i - 1]].tiemposProcesamiento[j - 1];
        }
    }

    return tiemposFinalizacion[orden.size()][numMaquinas];
}

bool compara(pair<int, int>a,pair<int, int>b){
    return a.second>b.second;
}

// Fase de construcción de GRASP
vector<int> construirGRASP(const vector<Tarea>& tareas, int numMaquinas) {
    vector<int> candidatos, solucion;
    for (size_t i = 0; i < tareas.size(); ++i) candidatos.push_back(i);

    while (!candidatos.empty()) {
        vector<pair<int, int>> makespanCandidatos;

        // Evaluar cada candidato en la solución parcial
        for (int tarea : candidatos) {
            vector<int> solucionTemporal = solucion;
            solucionTemporal.push_back(tarea);
            int makespan = calcularMakespan(tareas, solucionTemporal, numMaquinas);
            makespanCandidatos.push_back({tarea, makespan});
        }

        // Encontrar límites del RCL
        sort(makespanCandidatos.begin(),makespanCandidatos.end(),compara);
        int beta = makespanCandidatos[0].second;
        int tau = makespanCandidatos[makespanCandidatos.size()-1].second;
        

        // Crear RCL (Lista de Candidatos Restringida)
        vector<int> RCL;
        int inda = beta + ALFA * (tau - beta);
        for (const auto& cm : makespanCandidatos) {
            if (cm.second <= inda) {
                RCL.push_back(cm.first);
            }
        }

        // Seleccionar aleatoriamente un elemento de la RCL
        int seleccionado = RCL[rand() % RCL.size()];
        solucion.push_back(seleccionado);
        candidatos.erase(remove(candidatos.begin(), candidatos.end(), seleccionado), candidatos.end());
    }

    return solucion;
}

// Fase de búsqueda local
vector<int> busquedaLocal(const vector<Tarea>& tareas, vector<int> secuencia, int numMaquinas) {
    bool mejora = true;
    vector<int> mejorSecuencia = secuencia;
    int mejorMakespan = calcularMakespan(tareas, secuencia, numMaquinas);

    while (mejora) {
        mejora = false;
        for (size_t i = 0; i < secuencia.size(); ++i) {
            for (size_t j = i + 1; j < secuencia.size(); ++j) {
                swap(secuencia[i], secuencia[j]);
                int nuevoMakespan = calcularMakespan(tareas, secuencia, numMaquinas);
                if (nuevoMakespan < mejorMakespan) {
                    mejorMakespan = nuevoMakespan;
                    mejorSecuencia = secuencia;
                    mejora = true;
                }
                swap(secuencia[i], secuencia[j]);
            }
        }
    }

    return mejorSecuencia;
}

// Función general de GRASP
vector<int> GRASP(const vector<Tarea>& tareas, int numMaquinas) {
    vector<int> mejorSecuencia;
    int mejorMakespan = INT_MAX;

    for (int iter = 0; iter < ITERACIONES; ++iter) {
        vector<int> secuenciaInicial = construirGRASP(tareas, numMaquinas);
        vector<int> secuenciaMejorada = busquedaLocal(tareas, secuenciaInicial, numMaquinas);
        int makespanActual = calcularMakespan(tareas, secuenciaMejorada, numMaquinas);

        if (makespanActual < mejorMakespan) {
            mejorMakespan = makespanActual;
            mejorSecuencia = secuenciaMejorada;
        }

        
    }

    return mejorSecuencia;
}

int main() {
    srand(time(0));

    int numTareas = 5, numMaquinas = 5;


    vector<Tarea> tareas = {
        {1, {2, 3, 4, 3, 3}},
        {2, {4, 2, 3, 4, 6}},
        {3, {6, 4, 1, 3, 5}},
        {4, {8, 5, 4, 8, 7}},
        {5, {10, 7, 5, 4, 3}}
    };

    vector<int> mejorSecuencia = GRASP(tareas, numMaquinas);

    cout << "Mejor secuencia óptima: ";
    for (const auto& tarea : mejorSecuencia) {
        cout << tareas[tarea].id << " ";
    }
    cout << endl;
    cout << "Mejor Makespan: " << calcularMakespan(tareas, mejorSecuencia, numMaquinas) << endl;

    return 0;
}





