# Game Design Document (GDD) Básico: Head Soccer Clash

## 1. Visión General y Concepto del Juego

**Head Soccer Clash** es un videojuego de deportes arcade en 2D desarrollado en C++ utilizando la biblioteca gráfica **Raylib**. Inspirado en los clásicos juegos de fútbol *head-to-head*, el proyecto enfrenta a dos personajes en partidos individuales de ritmo rápido en una única pantalla de vista lateral (*side-view*). 

El objetivo principal de los jugadores es impulsar una pelota hacia la portería rival respetando un límite de tiempo. El desarrollo de este proyecto está enfocado en una arquitectura modular y escalable mediante una máquina de estados, permitiendo iterar desde un **Mínimo Producto Jugable (MVP)** basado en primitivas geométricas hacia un motor completo con gestión de entidades y físicas 2D.

---

## 2. Descripción Narrativa del Bucle Jugable y Pantallas

El flujo de ejecución de la aplicación está controlado por un gestor de estados (*StateMachine*) que conmuta entre las distintas pantallas requeridas en el ciclo de vida del juego:

                                [Menú Principal] ---> [Pantalla de Juego (MatchState)] ---> [Game Over / Resultado]
                                        ^                                                                |
                                        |_______________________ Reintentar / Volver ____________________|

### Flujo de Navegación
1. **Pantalla de Inicio / Menú Principal (`MenuState`):** Constituye el punto de entrada a la aplicación. Renderiza un título descriptivo y un menú interactivo mediante teclado o ratón que permite al usuario iniciar una nueva partida o salir de la aplicación.
2. **Pantalla del Juego (`MatchState`):** Representa el núcleo jugable. Renderiza el terreno de juego, las porterías, los dos personajes y la pelota. Gestiona las entradas del usuario, calcula la integración de físicas (gravedad, velocidad y colisiones), actualiza el cronómetro descendente y contabiliza el marcador actual en el HUD.
3. **Pantalla de Fin de Juego (`GameOverState`):** Se activa automáticamente al agotarse el tiempo del partido o cuando un jugador alcanza la condición de victoria. Detiene la simulación física, muestra el resultado final (indicando qué jugador ha ganado o si ha habido un empate) y ofrece la posibilidad de reiniciar el partido inmediatamente o regresar al menú principal.

---

## 3. Arquitectura Mecánica y Motor de Físicas

La mecánica principal del juego gira en torno al control espacial del jugador y la transferencia de momento hacia la pelota mediante colisiones AABB (Axis-Aligned Bounding Box) y colisiones círculo-círculo / círculo-rectángulo.

### Movimiento y Control
Cada jugador se simula como una entidad cinemática dentro de sus límites laterales del campo:
* **Desplazamiento horizontal:** Al presionar las teclas de dirección o navegación (A/D para el Jugador 1, Flechas para el Jugador 2), se aplica una velocidad lineal constante en el eje horizontal.
* **Salto y Gravedad:** Al activar la acción de salto, se asigna un impulso vertical inicial negativo. Durante la fase de actualización (*update*), el sistema aplica una aceleración de gravedad constante que reduce la velocidad vertical hasta devolver al personaje a la coordenada del suelo.

### Simulación de la Pelota y Colisiones
La pelota actúa como un cuerpo rígido simplificado sujeto a aceleración gravitatoria y rebotes:
* **Vector de Velocidad y Restitución:** La pelota almacena su posición y un vector de velocidad. En cada fotograma, la posición se actualiza en función del tiempo transcurrido (*Delta Time*).
* **Interacción con el Entorno:** Al colisionar con los límites de la pantalla, el suelo o los postes de las porterías (definidos como rectángulos), la velocidad de la pelota se invierte y se multiplica por un coeficiente de restitución $e \in (0, 1)$ para simular la pérdida de energía en el bote.
* **Mecánica de Golpeo:** Al producirse un impacto entre la cabeza del jugador (círculo/rectángulo) y la pelota, se calcula el vector de dirección desde el centro del jugador hacia el centro de la pelota, transfiriendo parte de la velocidad del jugador a la pelota para permitir disparos bombeados, remates rectos o despejes defensivos.

---

## 4. Modelado Visual para el MVP (Hito 1)

Cumpliendo con los requerimientos del primer hito del proyecto, el renderizado de la versión inicial prescinde de *sprites* o texturas complejas, abstrayendo todos los elementos jugables mediante primitivas geométricas integradas en Raylib:

* **Terreno de Juego y Porterías:** El campo se dibuja mediante un rectángulo principal en la base de la pantalla. Las porterías situadas en los extremos izquierdo y derecho se componen de dos rectángulos verticales (los postes) y un área delimitadora interna que actúa como sensor de gol.
* **Jugadores:** Cada futbolista se representa mediante la combinación de un **círculo superior** (que simula la cabeza y actúa como la zona principal de remate) y un **rectángulo inferior** (que representa el cuerpo y delimita la caja de colisión para el desplazamiento en el suelo). Se diferencian visualmente mediante colores primarios distintos (por ejemplo, rojo para el Jugador 1 y azul para el Jugador 2).
* **Pelota:** Se renderiza mediante un único círculo blanco de radio prefijado, con una línea o indicador interno para visibilizar su rotación tras los impactos.
* **Interfaz de Usuario (HUD):** Se dibuja en la parte superior mediante la función `DrawText()` de Raylib, mostrando el tiempo restante formateado en segundos y la puntuación numérica formateada como `JUGADOR 1 [ X - Y ] JUGADOR 2`.

---

## 5. Componentes de Software y Estructura del Código

El proyecto se estructurará modularmente en C++ bajo los siguientes componentes principales:

1. **`GameManager` / `StateMachine`:** Clase encargada de administrar la pila de estados de la aplicación (`GameState`), gestionando las transiciones de pantalla sin acoplar la lógica de juego al bucle de eventos principal.
2. **`Player` (Entidad Jugador):** Encapsula la posición, dimensiones, velocidad, teclas de control asociadas y el método de renderizado geométrico.
3. **`Ball` (Entidad Pelota):** Almacena las propiedades físicas de la pelota, aplicando la integración del movimiento por *Delta Time* (`GetFrameTime()`) y los métodos de detección y respuesta ante colisiones.
4. **`ScoreManager` / `MatchRules`:** Módulo encargado de verificar si la posición de la pelota ha rebasado la línea de gol, incrementar los marcadores correspondientes, reiniciar la posición de las entidades al centro del campo tras un gol y evaluar el final del tiempo reglamentario.

---

## 6. Power-up de ralentización

Al comenzar cada saque aparece un círculo violeta con dos barras blancas en el centro del campo, a 150 píxeles sobre el suelo. Cualquiera de los dos jugadores puede recogerlo al saltar y tocarlo. La altura permite alcanzarlo con un salto normal, pero no recogerlo desde el suelo. La pelota no puede recogerlo ni chocar con él.

El objeto desaparece al recogerlo y la pelota se mueve al **70 % de su velocidad normal durante 12 segundos**. La pelota se vuelve violeta y un mensaje muestra el tiempo restante. Los jugadores y el reloj del partido mantienen su ritmo normal. El objeto reaparece **12 segundos después de recogerlo** y puede recogerse de nuevo con otro salto.

La clase `PowerUp` gestiona la recogida, los temporizadores y el tiempo de simulación de la pelota. `MainGameState` usa ese tiempo para aplicar gravedad, fricción y movimiento. La velocidad almacenada no se multiplica en cada fotograma: así se evita frenar la pelota repetidamente y se recupera el ritmo normal al terminar el efecto. Si el efecto termina dentro de un fotograma, se combinan la parte ralentizada y la parte normal.

Los temporizadores se detienen durante la celebración de un gol. El siguiente saque reinicia el objeto y cancela cualquier efecto anterior. Una partida nueva también empieza con el objeto disponible.

El modo debug sigue activándose con **F1**. Muestra el área de recogida y el porcentaje de velocidad de la pelota. La predicción de **F2** utiliza una copia del power-up para tener en cuenta la ralentización y su final sin modificar el partido.

### Compilación y pruebas en Linux

Se necesita G++ con C++17 y Raylib instalada. Desde la raíz del repositorio:

```bash
g++ -std=c++17 -Isrc -Ivendor/include src/*.cpp src/physics/*.cpp src/debug/*.cpp -L/usr/local/lib -lraylib -lX11 -lm -lpthread -ldl -lrt -o /tmp/headsoccer-power-up
/tmp/headsoccer-power-up
```

Las pruebas comprueban la recogida por ambos jugadores, que el objeto se alcanza saltando, el contacto con el borde, la duración, la reaparición, el reinicio y el movimiento ralentizado:

```bash
g++ -std=c++17 -Isrc -Ivendor/include tests/PowerUpTest.cpp src/PowerUp.cpp src/physics/body.cpp src/physics/physics.cpp -L/usr/local/lib -lraylib -lX11 -lm -lpthread -ldl -lrt -o /tmp/headsoccer-power-up-test
/tmp/headsoccer-power-up-test
```

Para comprobarlo jugando, inicia el partido con Enter, mueve un jugador debajo del objeto violeta y salta (W para el jugador rojo o flecha arriba para el azul). Comprueba el aviso de 12 segundos. Activa F1 y F2 durante el efecto, espera a que termine y salta de nuevo para recoger el objeto cuando reaparezca. Tras un gol, comprueba que el objeto vuelve a estar disponible y la pelota recupera su velocidad normal.
