#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROCESOS 10
#define LINEA_MAX 100
#define MAX_INSTRUCCIONES 20

typedef struct {
    int pid;
    int pc;
    int ax, bx, cx;
    int quantum;
    char estado[10];
    int tiempo_restante;
    char instrucciones[MAX_INSTRUCCIONES][LINEA_MAX];
    int num_instrucciones;
} Proceso;

void imprimir_proceso(Proceso p) {
    printf("  PID: %d, Estado: %s, PC: %d, AX: %d, BX: %d, CX: %d\n",
           p.pid, p.estado, p.pc, p.ax, p.bx, p.cx);
}

void ejecutar_instruccion(Proceso *p) {
    char instruccion_copia[LINEA_MAX];
    strcpy(instruccion_copia, p->instrucciones[p->pc]); // strcpy(destino, origen)

    char *comando = strtok(instruccion_copia, " \t\n");
    char *op1_str = strtok(NULL, " ,\t\n");
    char *op2_str = strtok(NULL, " ,\t\n");
    
    int *op1_reg = NULL;
    int *op2_reg = NULL;
    int op2_val = 0;

    // Obtener la dirección del primer operando (registro)
    if (op1_str != NULL) {
        if (strcmp(op1_str, "AX") == 0) op1_reg = &p->ax;
        else if (strcmp(op1_str, "BX") == 0) op1_reg = &p->bx;
        else if (strcmp(op1_str, "CX") == 0) op1_reg = &p->cx;
        else op2_val = atoi(op1_str);
    }
    
    // Obtener la dirección o valor del segundo operando
    if (op2_str != NULL) {
        if (strcmp(op2_str, "AX") == 0) op2_reg = &p->ax;
        else if (strcmp(op2_str, "BX") == 0) op2_reg = &p->bx;
        else if (strcmp(op2_str, "CX") == 0) op2_reg = &p->cx;
        else op2_val = atoi(op2_str);
    }

    if (strcmp(comando, "INC") == 0) {
        if (op1_reg != NULL) (*op1_reg)++;
    } else if (strcmp(comando, "ADD") == 0) {
        if (op1_reg != NULL && op2_reg != NULL) {
            (*op1_reg) += (*op2_reg);
        } else if (op1_reg != NULL) {
            if (op2_str != NULL) {
                (*op1_reg) += atoi(op2_str);
            }
        }
    } else if (strcmp(comando, "SUB") == 0) {
        if (op1_reg != NULL && op2_reg != NULL) {
            (*op1_reg) -= (*op2_reg);
        } else if (op1_reg != NULL) {
            if (op2_str != NULL) {
                (*op1_reg) -= atoi(op2_str);
            }
        }
    } else if (strcmp(comando, "MUL") == 0) {
        if (op1_reg != NULL && op2_reg != NULL) {
            (*op1_reg) *= (*op2_reg);
        } else if (op1_reg != NULL) {
            if (op2_str != NULL) {
                (*op1_reg) *= atoi(op2_str);
            }
        }
    } else if (strcmp(comando, "JMP") == 0) {
        p->pc = op2_val;
    }
}

int main() {
    Proceso procesos[MAX_PROCESOS];
    int num_procesos = 0;
    int i;
    char linea[LINEA_MAX];

    FILE *archivo_procesos = fopen("procesos.txt", "r");
    if (archivo_procesos == NULL) {
        printf("Error: No se pudo abrir el archivo procesos.txt.\n");
        return 1;
    }

    if (fgets(linea, LINEA_MAX, archivo_procesos) == NULL) {
        printf("Error: Archivo vacio o no se pudo leer la cantidad de procesos.\n");
        fclose(archivo_procesos);
        return 1;
    }
    num_procesos = atoi(linea);

    for (i = 0; i < num_procesos; i++) {
        if (fgets(linea, LINEA_MAX, archivo_procesos) == NULL) {
            printf("Error: Fin de archivo inesperado. Faltan datos para el proceso %d.\n", i + 1);
            fclose(archivo_procesos);
            return 1;
        }

        if (sscanf(linea, "PID:%d, PC=%d, AX=%d, BX=%d, CX=%d, Quantum=%d",
                   &procesos[i].pid, &procesos[i].pc, &procesos[i].ax,
                   &procesos[i].bx, &procesos[i].cx, &procesos[i].quantum) != 6) {
            printf("Error: Formato de datos invalido para el proceso %d.\n", i + 1);
            fclose(archivo_procesos);
            return 1;
        }

        char nombre_instrucciones[20];
        sprintf(nombre_instrucciones, "%d.txt", procesos[i].pid);
        FILE *archivo_instrucciones = fopen(nombre_instrucciones, "r");
        if (archivo_instrucciones == NULL) {
            printf("Error: No se pudo abrir el archivo de instrucciones %s\n", nombre_instrucciones);
            fclose(archivo_procesos);
            return 1;
        }

        procesos[i].num_instrucciones = 0;
        while (fgets(linea, LINEA_MAX, archivo_instrucciones) != NULL) {
            linea[strcspn(linea, "\n")] = 0;
            strcpy(procesos[i].instrucciones[procesos[i].num_instrucciones], linea);
            procesos[i].num_instrucciones++;
        }
        fclose(archivo_instrucciones);
        
        strcpy(procesos[i].estado, "Listo");
        procesos[i].tiempo_restante = procesos[i].quantum;
    }
    fclose(archivo_procesos);
    
    printf("--- Procesos cargados desde procesos.txt ---\n");
    for (i = 0; i < num_procesos; i++) {
        imprimir_proceso(procesos[i]);
    }
    printf("----------------------------------\n\n");

    printf("--- Simulacion del planificador Round Robin ---\n");

    int procesos_terminados = 0;
    int proceso_actual_idx = 0;
    int tiempo_total = 0;
    
    while (procesos_terminados < num_procesos) {
        int inicio_busqueda = proceso_actual_idx;
        while (strcmp(procesos[proceso_actual_idx].estado, "Terminado") == 0) {
            proceso_actual_idx = (proceso_actual_idx + 1) % num_procesos;
            if (proceso_actual_idx == inicio_busqueda) {
                break;
            }
        }
        
        Proceso *p = &procesos[proceso_actual_idx];

        if (strcmp(p->estado, "Terminado") != 0) {
            printf("\n--- Tiempo de simulacion: %d, Proceso Activo: %d ---\n", tiempo_total, p->pid);
            strcpy(p->estado, "Ejecutando");
            
            if (p->pc < p->num_instrucciones) {
                printf("  > Ejecutando instruccion: %s\n", p->instrucciones[p->pc]);
                
                int pc_anterior = p->pc;
                ejecutar_instruccion(p);

                if (strncmp(p->instrucciones[pc_anterior], "JMP", 3) != 0) {
                    p->pc++;
                }
            }
            
            p->tiempo_restante--;
            
            printf("  > Estado despues de la instruccion: ");
            imprimir_proceso(*p);

            if (p->pc >= p->num_instrucciones) {
                strcpy(p->estado, "Terminado");
                procesos_terminados++;
                printf("\nProceso %d ha terminado su ejecucion.\n", p->pid);
            }
            else if (p->tiempo_restante == 0) {
                printf("\n[Cambio de contexto]\n");
                printf("Guardando estado de Proceso %d: PC=%d, AX=%d, BX=%d, CX=%d\n", p->pid, p->pc, p->ax, p->bx, p->cx);
                
                p->tiempo_restante = p->quantum;
                strcpy(p->estado, "Listo");
                
                int siguiente_proceso_idx = (proceso_actual_idx + 1) % num_procesos;
                while (strcmp(procesos[siguiente_proceso_idx].estado, "Terminado") == 0 && procesos_terminados < num_procesos) {
                    siguiente_proceso_idx = (siguiente_proceso_idx + 1) % num_procesos;
                }
                
                if (strcmp(procesos[siguiente_proceso_idx].estado, "Terminado") != 0) {
                    Proceso* siguiente_proceso = &procesos[siguiente_proceso_idx];
                    printf("Cargando estado de Proceso %d: PC=%d, AX=%d, BX=%d, CX=%d\n",
                           siguiente_proceso->pid, siguiente_proceso->pc, siguiente_proceso->ax, siguiente_proceso->bx, siguiente_proceso->cx);
                }
            }
        }
        
        proceso_actual_idx = (proceso_actual_idx + 1) % num_procesos;
        tiempo_total++;
    }

    printf("\n--- Simulacion finalizada: Todos los procesos han terminado ---\n");
    return 0;
}