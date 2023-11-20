# Descripción

El objetivo de este proyecto es simular el problema del "productor-consumidor". La tarea en cuestión consite en crear un conjunto de almacenes, proveedores y clientes y realizar una simulación con ello. Para eso se tendrán en cuenta las siguentes consideraciones:

Cada equipo implementará 3 programas: un _almacén_, un _productor_, y un _consumidor_.

## Sobre el almacén

- El almacén es un programa (servidor) que escucha por un puerto y permite conexiones de varios clientes.

- El _almacén_ tiene una capacidad limitada de objetos que puede tener en un momento dado en su inventario.

```bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # port=3000 capacity=50
```

- Cuando el servidor recibe una nueva conexión lo primero que recibirá será un string "producer" o un "consumer".

- El almacén debe poder que varios "productores" se conecten y también varios "consumidores".

- El almacén cada vez que reciba un producto nuevo, o cada vez que un consumidor consuma un producto, debe visualizar en la pantalla un estado actual del almacén (i.e. Cantidad total de productos, cuantos productos de cada tipo, capacidad disponible. etc.)

- El almacén puede además tener un limite máximo para cada tipo de producto que se puede generar. Esto será especificado en el momento de la creación del almacén a través de los parámetros.

```bash
$ gcc warehouse.c -o warehouse
$ ./warehouse 3000 50 # esto significa que puede almacenar un maximo de 50 elementos (de cualquier tipo)
$ ./warehouse 3000 A:20 B:30 # un máximo de 20 productos de tipo "A" y un máximo de 30 de tipo "B"
$ ./warehouse 3000 50 A:20 # máximo total es 50, pero solo 20 de tipo "A" (de tipo B pueden ser 50)
```

## Sobre el proveedor

- El _productor_ es un programa que genera nuevos productos y que se conecta al almacén para que este los guarde.

- El programa productor en el momento que comienza la conexión envia el string "producer" hacia el almacén para identificarse.

- Cada equipo debe implementar un productor simple que genera un producto _cada 1 segundo_ y lo envia hacia el almacén.

- Si el almacén no puede aceptar nuevos productos (porque está lleno) el productor debe parar la producción.

- El productor debe imprimir por la consola los productos que va generando (en el momento que los genera).

- Cada productor tendrá un identificador (de 3 caracteres) que formará parte de todos los productos que genera.

- Cada producto generado tambien tendra un identificador que será un número entero secuencial (i.e. el primero producto es el 1, el segundo es el 2 y asi sucesivamente).

- En el momento de la ejecución el productor recibe como parámetro el IP y puerto donde está el almacén de forma ip:puerto.

```bash
$ gcc producer.c -o producer
$ ./producer 10.6.122.44:3000
```

- El productor puede producir más de un tipo de producto. Esta información se especificará a la hora de la creación de un productor como parte de los parámetros.

- Además de los tipos de productos que se pueden generar, también recibirá como parámetro un número entero que significa la cantidad de productos de ese tipo que se generan por intervalo de tiempo. (cada 1 segundo)

```bash
$ gcc producer.c -o producer
$ ./producer A:1 B:2 10.6.122.44:3000 # un producto de tipo "A" cada 1 segundo y dos producto de tipo "B"
```

- El productor puede conectarse a más de un almacén y producir hacia todos los almacenes simultáneamente.

- Los requerimientos anteriores se mantienen, asi que si un productor genera un producto por segundo, y está conectado a 2 almacenes, entonces como máximo, cada almacén recibirá un producto cada 2 segundos.

- El productor recibirá como parámetros todos los almacenes a los que el se va a conectar.

```bash
$ gcc producer.c -o producer
$ ./producer A:1 B:2 10.6.122.44:3000 10.6.122.44:4000 10.6.122.45:3500 # 3 almacenes
```

- Cada equipo de estudiantes decidirá la estrategia para repartir los productos entre los diferentes almacenes.

## Sobre el consumidor

- El _consumidor_ es un programa que consume productos y que se conecta al almacén para que este se los provea.

- El programa consumidor en el momento que comienza la conexión envia el string "consumer" hacia el almacén para identificarse.

- Cada equipo debe implementar un consumidor simple que consume un producto _cada 1 segundo_ desde el almacén.

- Si el almacén no puede generar productos (porque está vacío) el consumidor debe quedarse esperando a que haya disponibilidad.

- El consumidor debe imprimir por la consola los productos que va consumiento (en el momento que los consume).

- En el momento de la ejecución el consumidor recibe como parámetro el IP y puerto donde está el almacén de forma ip:puerto.

```bash
$ gcc consumer.c -o consumer
$ ./consumer 10.6.122.44:3000
```

- El consumidor puede especificar la cantidad de productos que necesita en cada intervalo de tiempo y además debe poder especificar los tipos de productos que necesita.

```bash
$ gcc consumer.c -o consumer
$ ./consumer A:2 B:3 10.6.122.44:3000 # cada 1 segundo este consumidos necesita 2 productos de tipo "A" y 3 productos de tipo "B"
```

- Cada programa consumidor puede conectarse a más de un almacén.

- En la terminal, en el momento de la creación, se le pasará como parámetro todos los ip:puerto de todos los almacenes a los que el se conectará.

```bash
$ gcc consumer.c -o consumer
$ ./consumer A:2 B:3 10.6.122.44:3000 10.6.122.45:4000 # 2 almacenes
```

## Sobre los productos

- el producto es una instancia de la siguiente estructura de c

```c
struct product {
    char provider_id[3];     // identificador del productor
    int  product_id;         // numero secuencial del productor
    char product_type[3];    // tipo de producto generado
    char product_data[4084]  // esto es solo para que todos los productos tengan 4096 bytes de tamaño
}
```

## Sistema Operativo

Implementado en `Linux`.

## Colaboraciones

Cree un `issue` o envíe un `pull request`

## Autores

Iván Galbán Smith <ivan.galban.smith@gmail.com>

Raydel E. Alonso Baryolo <raydelalonsobaryolo@gmail.com>

3rd year Computer Science students, University of Havana
