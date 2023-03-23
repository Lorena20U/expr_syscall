
# Proyecto de Sistemas Operativos
## Problema

Determine si una expresión aritmética se encuentra correctamente cerrada. La función deberá retornar verdadero si la expresión está correctamente cerrada y falso de lo contrario.

Entiéndase por cerrada al hecho de que cada símbolo de paréntesis y/o corchete abierto debe tener su correspondiente cierre. Ejemplo (5+5), [(1+1)*(2+2)], (((([1]))))

## Requerimientos

* Instalar una herramiento de virutalización (VirtualBox, VmWare) - [Descargar VirtualBox](https://www.virtualbox.org/wiki/Downloads)
* Descargas una imagen de Ubuntu (ISO), en este proyecto se utilizó específicamente la versión 18.04.6 - [Releases Ubuntu](https://releases.ubuntu.com/)

## Proceso

### Pasos previos antes de agregar el nuevo Syscall.
##### _____________________

1. Levantar una imagen con el ISO de Ubuntu en VirtualBox para tener en Linux corriendo.
2. Ingresar con los privilegios sudo y confirmar la versión del Kernel disponible.

```bash
~$ uname -r
> 5.4.1-generic
```

3. Instalar y actualizar las dependencias/librerias que se necesitan para compilar el kernel.

```bash
~$ apt install build-essential libssl-dev gcc bc bison flex libelf-dev libncurses5-dev dwarves
~$ apt install make initramfs-tools-bin
~$ apt update && apt upgrade -y
```

4. Descargar el código de una versión igual o superior del kernel de la distribución seleccionada de la imagen (v5.x)

```bash
~$ wget https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-5.6.1.tar.xz
```
5. Descomprimir el archivo hacia la carpeta correspondiente.

```bash
~$ tar -xvf linux-5.6.1.tar.xz -C /usr/src/
```
### Aregando el nuevo Syscall
##### _____________________

1. Ingresar a la carpeta del nuevo kernel. Crear un directorio para agregar el código del nuevo syscall. Crear un archivo .c para agregar el código del nuevo syscall y guardarlo.

```bash
/linux-5.6.1$ mkdir expr
$ cd expr
$ nano expr_syscall.c
```
2. Crear un archivo Makefile en la carpeta del syscall, modificar y guardar.
```bash
$ nano Makefile
```
![Makefile](/img/expr-Makefile.png)

3. Modificar el Makefile del kernel para agregar la carpeta donde se encuentra el código del nuevo syscall.

```bash
/linux-5.6.1$ cd ..
/linux-5.6.1$ nano Makefile
```
![Makefile-kernel](/img/linux-5.6.1-Makefile.png)

4. Modificar el archivo syscalls.h para agregar el prototipo del nuevo syscall.
```bash
/linux-5.6.1$ cd include/linux/
/linux-5.6.1/include/linux$ nano syscalls.h 
```
![syscalls.h](/img/include-linux-syscalls.png)

5. Por último, modificar la tabla de los syscall. Verificar los números disponibles en la tabla, agregar y guardar.
```bash
/linux-5.6.1$ cd arch/x86/entry/syscalls/
/linux-5.6.1/arch/x86/entry/syscalls/$ nano syscall_64.tbl
```
![syscall_64.tbl](/img/arch-x86-entry-syscalls-syscall_64.png)



### Compilar el kernel
##### _____________________
1. Se copia el archivo de configuración del kernel en uso en el directorio /boot a un archivo oculto llamado ".confg" en el directorio actual. 
```
$ cp /boot/config-$(uname -r) ./.confg

```
2.  Se crea una nueva configuración del kernel basada en los módulos del sistema que están actualmente cargados en el sistema en ejecución.

```bash
$ make localmodconfig
```

3. Se compila el kernel de Linux a partir del código fuente y se crean los paquetes binarios de Debian (".deb").
```bash
$ make -j4 deb-pkg
```
4. Se instalan los paquetes en el sistema actual.
```
/src/linux-5.6.1# cd ..
/src/# dpkg -i *.deb
```
5. Se reinicia el sistema. 
```
# reboot
```

### Verificación
##### _____________________
1. Se verifica la instalación del kernel modificado.
```bash
~$ uname -r
> 5.6.1
```

2. Crear un directorio para almacenar el test. Crear un archivo .c en donde se agrege un código en el que llame al nuevo syscall, compilar y ver el output.
```bash
$ mkdir test
$ cd test
test$ nano test.c
test$ gcc -o test test.c
test$ ./test
> ...
```
![./test](/img/test-vm.jpg)

3. Ejecutar el siguiente comando para visualizar los mensajes del kernel.
```bash
$ dmesg
```

# Pasos previos para habilitar el uso de Docker
1. Con este comando se abre una interfaz gráfica que se basa en menús, en esta interfaz se pueden configurar los ajustes del kernel. Será necesario habilitar la virtualización y algunos drivers de red para poder utilizar Docker después de compilar el kernel. 

```bash
/src/usr/linux-5.6.1$ make menuconfig
```
![menuconfig](/img/menuconfig.png)


2. En el menú principal hay que buscar la opción de virtualización y hay que activarla presionando la tecla “Y”, luego hay que entrar en este menú presionando enter. 

![virtualizacion](/img/menuconfig-virt.png)

3. Dentro del menú de virtualización hay que activar todos los submenus con la tecla “y”. Uno de los requisitos previos a instalar Docker engine es tener soporte de Kernel-based Virtual Machine (KVM).

![virtualizacion - menu](/img/menuconfig-virt-menu.png)

4. Luego de activar las opciones de virtualización hay que salir al menú principal y dirigirse a la opción de Device Drivers y entrar presionando la tecla enter. 

![device drivers](/img/menuconfig-devicedrivers.png)

5. Dentro del menú de Device Drivers hay que dirigirse a la opción de Network device support y habilitar está opción presionando la tecla “y”.  Esto permite que el sistema operativo se comunique con los dispositivos de red después. 

![device drivers - menu](/img/menuconfig-devicedrivers-menu.png)

6. Esta opción se habilita para crear interfaces de red virtuales a partir de una única interfaz física. 

![device drivers - menu](/img/menuconfig-networkdevicesupport.png)


7. Esta opción se habilita para poder crear un par de interfaces de red virtuales en el sistema, Para que los contenedores de Docker se puedan comunicar entre ellos y con el sistema host, es necesario utilizar interfaces de redes virtuales.

![device drivers - menu](/img/menuconfig-networkdevicesupport-change.png)

8. Instalar los módulos del kernel compilados en el sistema.

```bash
$ sudo make modules_install install
```
9. Reiniciar el sistema.

```bash
$ reboot
```


## Uso de Docker

1. Instalar Docker en la máquina virtual siguiendo los pasos de instalación de guía en la página oficial de Docker. [Guía de Docker](https://docs.docker.com/engine/install/ubuntu/)

2. Verificar que el servicio de Docker este activo.
```bash
~$ systemctl status docker.service
```
![docker status](/img/docker-status.jpg)

3. Se ejecuta el siguiente comando para levantar un contenedor de Docker de la imagen "ubuntu:18.04" y abrir un shell de bash dentro de ese contenedor.

Ejecutar:
```bash
~$ docker run --privileged -it ubuntu:18.04 bash
```
O, si se le quiere dar un nombre específico a ese contenedor:

```bash
~$ docker run --privileged -it --name expr_ubuntu ubuntu:18.04 bash
```

4. Por último, realizar los mismos pasos que se realizó para realizar el test haciendo una llamada al syscall desde el contenedor.

![tes_docker](/img/test-docker.png)