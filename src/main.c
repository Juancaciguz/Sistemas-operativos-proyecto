#include <stdio.h>
#include <string.h>

typedef struct {
int pid;
int pc; // Program Counter simulado
int ax, bx, cx; // Registros simulados
int quantum;
char estado[10]; // "Listo", "Ejecutando", "Terminado"
} Proceso;

int main() {
    Proceso procesos[3]; 
    int num_procesos = 0;
    int i;
    printf("Ingrese la cantidad de procesos a simular (max. %d): ", 3);
    scanf("%d", &num_procesos);
    if (num_procesos > 3 || num_procesos <= 0) {
        printf("Error: Cantidad de procesos no valida.\n");
        return 1;
    }
   for (i = 0; i < num_procesos; i++) {
        printf("\n Ingrese los datos para el Proceso %d \n", i + 1);
        
        printf("PID: ");
        scanf("%d", &procesos[i].pid);

        printf("AX: ");
        scanf("%d", &procesos[i].ax);

        printf("BX: ");
        scanf("%d", &procesos[i].bx);

        printf("CX: ");
        scanf("%d", &procesos[i].cx);

        printf("Quantum: ");
        scanf("%d", &procesos[i].quantum);

        procesos[i].pc = 0;
        strcpy(procesos[i].estado, "Listo");
    }
    for (i = 0; i < num_procesos; i++) {
        printf("Proceso %d:\n", i + 1);
        printf("  PID: %d\n", procesos[i].pid);
        printf("  AX: %d, BX: %d, CX: %d\n", procesos[i].ax, procesos[i].bx, procesos[i].cx);
        printf("  Quantum: %d\n", procesos[i].quantum);
        printf("  PC: %d\n", procesos[i].pc);
        printf("  Estado: %s\n", procesos[i].estado);
    }

    return 0; 
}