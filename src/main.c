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
    char instrucciones[MAX_INSTRUCCIONES][LINEA_MAX];// array[20][100]
    int num_instrucciones;
} Proceso;

void imprimir_proceso(Proceso p, FILE *log_file) {
    fprintf(log_file, "  PID: %d, Estado: %s, PC: %d, AX: %d, BX: %d, CX: %d\n",
           p.pid, p.estado, p.pc, p.ax, p.bx, p.cx);
}

void ejecutar_instruccion(Proceso *p, FILE *log_file) {
    char instruccion_copia[LINEA_MAX];
    strcpy(instruccion_copia, p->instrucciones[p->pc]);

    char *comando = strtok(instruccion_copia, " \t\n");
    char *op1_str = strtok(NULL, " ,\t\n");
    char *op2_str = strtok(NULL, " ,\t\n");
    
    int *op1_reg = NULL;
    int *op2_reg = NULL;
    int op2_val = 0; 

    if (op1_str != NULL) {
        if (strcmp(op1_str, "AX") == 0) op1_reg = &p->ax;
        else if (strcmp(op1_str, "BX") == 0) op1_reg = &p->bx; 
        else if (strcmp(op1_str, "CX") == 0) op1_reg = &p->cx;
        else op2_val = atoi(op1_str);
    }
    if (op2_str != NULL) {
        if (strcmp(op2_str, "AX") == 0) op2_reg = &p->ax;
        else if (strcmp(op2_str, "BX") == 0) op2_reg = &p->bx;
        else if (strcmp(op2_str, "CX") == 0) op2_reg = &p->cx;
        else op2_val = atoi(op2_str);
    }

    if (strcmp(comando, "CANCEL") == 0) {
        strcpy(p->estado, "Cancelado");
        fprintf(log_file, "  > Proceso %d cancelado por instruccion CANCEL.\n", p->pid);
        return;
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

    FILE *log_file = fopen("simulacion.log", "w");
    if (log_file == NULL) {
        printf("Error: No se pudo abrir el archivo de log.\n");
        return 1;
    }

    FILE *archivo_procesos = fopen("procesos.txt", "r");
    if (archivo_procesos == NULL) {
        fprintf(log_file, "No se puede abrir el archivo procesos.txt.\n");
        fclose(log_file);
        return 1;
    }

    if (fgets(linea, LINEA_MAX, archivo_procesos) == NULL) {
        fprintf(log_file, "Archivo vacio o no se puede leer la cantidad de procesos.\n");
        fclose(archivo_procesos);
        fclose(log_file);
        return 1;
    }
    num_procesos = atoi(linea);

    for (i = 0; i < num_procesos; i++) {
        if (fgets(linea, LINEA_MAX, archivo_procesos) == NULL) {
            fprintf(log_file, "Faltan datos para el proceso %d.\n", i + 1);
            fclose(archivo_procesos);
            fclose(log_file);
            return 1;
        }

        if (sscanf(linea, "PID:%d, PC=%d, AX=%d, BX=%d, CX=%d, Quantum=%d",
                   &procesos[i].pid, &procesos[i].pc, &procesos[i].ax,
                   &procesos[i].bx, &procesos[i].cx, &procesos[i].quantum) != 6) {
            fprintf(log_file, "No estan todos los datos requeridos %d.\n", i + 1);
            fclose(archivo_procesos);
            fclose(log_file);
            return 1;
        }

        char nombre_instrucciones[20];
        sprintf(nombre_instrucciones, "%d.txt", procesos[i].pid);
        FILE *archivo_instrucciones = fopen(nombre_instrucciones, "r");
        if (archivo_instrucciones == NULL) {
            fprintf(log_file, "No se pudo abrir el archivo de instrucciones %s\n", nombre_instrucciones);
            fclose(archivo_procesos);
            fclose(log_file);
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
    
    fprintf(log_file, "Procesos:\n");
    for (i = 0; i < num_procesos; i++) {
        imprimir_proceso(procesos[i], log_file);
    }

    fprintf(log_file, "--- Round Robin ---\n");

    int procesos_terminados = 0;
    int proceso_actual_idx = 0;
    int tiempo_total = 0;
    
    while (procesos_terminados < num_procesos) {
        // Restaurar procesos cancelados temporalmente
        for (i = 0; i < num_procesos; i++) {
            if (strcmp(procesos[i].estado, "Cancelado") == 0 && procesos[i].pc < procesos[i].num_instrucciones) {
                strcpy(procesos[i].estado, "Listo");
            }
        }

        int inicio_busqueda = proceso_actual_idx;
        // Saltar procesos terminados
        while (strcmp(procesos[proceso_actual_idx].estado, "Terminado") == 0) {
            proceso_actual_idx = (proceso_actual_idx + 1) % num_procesos;
            if (proceso_actual_idx == inicio_busqueda) {
                break;
            }
        }

        Proceso *p = &procesos[proceso_actual_idx];

        if (strcmp(p->estado, "Terminado") != 0) {
            fprintf(log_file, "\n--- Tiempo de simulacion: %d, Id proceso: %d ---\n", tiempo_total, p->pid);
            strcpy(p->estado, "Ejecutando");

            int instrucciones_ejecutadas = 0;
            while (instrucciones_ejecutadas < p->quantum &&
                   p->pc < p->num_instrucciones &&
                   strcmp(p->estado, "Cancelado") != 0 &&
                   strcmp(p->estado, "Terminado") != 0) {

                fprintf(log_file, "  > Ejecutando instruccion: %s\n", p->instrucciones[p->pc]);
                int pc_anterior = p->pc;
                ejecutar_instruccion(p, log_file);

                // Si fue cancelado, no avanzar PC ni tiempo
                if (strcmp(p->estado, "Cancelado") == 0) {
                    fprintf(log_file, "\nProceso %d ha sido cancelado temporalmente y pasará al siguiente proceso.\n", p->pid);
                    break;
                } else if (strncmp(p->instrucciones[pc_anterior], "JMP", 3) != 0) {
                    p->pc++;
                }

                instrucciones_ejecutadas++;
                tiempo_total++;

                fprintf(log_file, "  > Estado despues de la instruccion: ");
                imprimir_proceso(*p, log_file);

                if (p->pc >= p->num_instrucciones) {
                    strcpy(p->estado, "Terminado");
                    procesos_terminados++;
                    fprintf(log_file, "\nProceso %d ha terminado su ejecucion.\n", p->pid);
                    break;
                }
            }

            // Solo cambio de contexto si no terminó ni fue cancelado
            if (strcmp(p->estado, "Terminado") != 0 && strcmp(p->estado, "Cancelado") != 0) {
                fprintf(log_file, "\n[Cambio de contexto]\n");
                fprintf(log_file, "Guardando estado de Proceso %d: PC=%d, AX=%d, BX=%d, CX=%d\n",
                        p->pid, p->pc, p->ax, p->bx, p->cx);

                strcpy(p->estado, "Listo");
            }
        }

        proceso_actual_idx = (proceso_actual_idx + 1) % num_procesos;
    }

    fprintf(log_file, "\n--- Simulacion finalizada: Todos los procesos han terminado ---\n");
    fclose(log_file);
    return 0;
}