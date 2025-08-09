# Proyecto de Sistemas Operativos

Integrantes : Juan Carlos Citelly Guzmán y Juan Manuel Zapata Olaya

Requisitos:

WSL (Windows Subsystem for Linux) o una distribución nativa de Linux. Asegúrese de tener un compilador de C/C++ instalado, como gcc


Pasos para compilar:

1. Desde una terminal de linux dirigirse a la ubicacion del proyecto
2. Utilizar el comando gcc src/main.c -o simulador
3. Utilizar el comando ./simulador
4. Visualizar en consola los resultados

### Ejemplo de procesos

5
PID:1, PC=0, AX=0, BX=1, CX=2, Quantum=3
PID:2, PC=0, AX=1, BX=3, CX=4, Quantum=2
PID:3, PC=0, AX=2, BX=5, CX=6, Quantum=1
PID:4, PC=0, AX=3, BX=7, CX=8, Quantum=2
PID:5, PC=0, AX=4, BX=9, CX=10, Quantum=3

### Ejemplo instrucciones

ADD AX,10
MUL BX,2
ADD AX,3
SUB AX,CX
INC AX
NOP
ADD AX,11
NOP
