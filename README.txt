
COMPILACIÓN:

Para compilar el programa se deben ejecutar la siguiente acción dentro de la carpeta donde
se encuentre el archivo main.c:

$ gcc main.c libraries/participant.c libraries/participant_list.c -lncurses -pthread
    main.c                          : Compilar el archivo main.c
    libraries/participant.c         : Añadir el módulo Participant 
    libraries/participant_list.c    : Añadir el módulo Participant_list
    -lncurses                       : Añadir la librería de ncurses.
    -pthread                        : Añadir la librería de threads.

La compilación utilizando esta instrucción generará un archivo a.out, el cual se podrá ejecutar.
Para cambiar el nombre y extensión a tal archivo, agregar: -o nombre.[exe/out] posterior a gcc.

La ejecución del archivo a.out requiere 4 parámetros n k i d, tales que:
    - n:    Cantidad de particulas de cada participante.
    - k:    Cantidad de participantes.
    - i:    Iteraciones del proceso.
    - d:    Delay de refresco de ventana.
Para ejecutar el programa se deben entregar estos 4 valores de forma válida.
Ej: ./a.out 1000 5 -1 10000

El valor -1 para delay implica una ejecución sin límite.

En el archivo ejemplos.txt se entregan ejemplos de instrucciones de ejecución.



Sea ./laboratorio1.exe n k i d una ejecución válida, se asumen los siguientes puntos:
    - La ejecución posee todos sus parámetros válidos
    - El valor d representa un valor en microsegundos.
