# EJERCICIO 1: Uso del manual

# a) Buscar en el manual la lista de funciones disponibles para el manejo de hilos. Las
# funciones de manejo de hilos comienzan por ((pthread)).

man -k pthread

# b) Consultar en la ayuda en qu ́e seccion del manual se encuentran las ((llamadas al
# sistema)) y buscar informacion sobre la llamada al sistema write

man man # en la descripción sale las diferentes secciones y lo que contienen

# EJERCICIO 2: Comandos y redireccionamiento
# a) Escribir un comando que busque las l ́ıneas que contengan ((molino)) en el fichero
# ((don quijote.txt)) y las a ̃nada al final del fichero ((aventuras.txt)).

grep molino don quijote.txt >> aventuras.txt

# b) Elaborar un pipeline que cuente el n ́umero de ficheros en el directorio actual

ls | wc -l

# c) Elaborar un pipeline que cuente el numero de lıneas distintas al concatenar ((lista
# de la compra Pepe.txt)) y ((lista de la compra Elena.txt)) y lo escriba en ((num
# compra.txt)). Si alguno de los ficheros no existe, hay que ignorar los mensajes de
# error, para lo cual se redirigir ́a la salida de errores a /dev/null.

cat lista de la compra Pepe.txt lista de la compra Elena.txt | sort | uniq -u | wc -l > num compra.txt 2> /dev/null

# EJERCICIO 9: Directorio de información de procesos

# Buscar para alguno de los procesos la siguiente informacion en el directorio /proc. Hay que tener en cuenta que
# tanto las variables de entorno como la lista de comandos delimitan los elementos con \0,
# así que puede ser conveniente convertir los \0 a \n usando tr '\0' '\n'.

ls /proc

# a) El nombre del ejecutable

    cat status | grep Name

# b) El directorio actual del proceso

    ls cwd

# c) La línea de comandos que se usó para lanzarlo

    sudo cat cmdline | tr '\0' '\n'

# d) El valor de la variable de entorno LANG

    sudo cat environ | tr '\0' '\n' | grep LANG

# e) La lista de hilos del proceso

    ls task