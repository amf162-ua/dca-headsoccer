# Sprites de los jugadores

Los dos sprites son PNG de 50 x 80 pixeles con fondo transparente, dibujados
para el proyecto:

- `assets/sprites/player_1.png`: jugador rojo mirando a la derecha.
- `assets/sprites/player_2.png`: jugador azul mirando a la izquierda.

`ResourcesManager::LoadPlayerTexture` elige la imagen de cada jugador y utiliza
la misma carga de texturas que los estadios y las porterias.

`MainGameState` carga los sprites al iniciar la partida, los dibuja sobre el
rectangulo del cuerpo y los libera al salir. Las cajas de colision, los controles
y el pie de la patada mantienen su funcionamiento. F1 permite ver las cajas.

## Probar la carga

Desde la raiz, con Raylib instalada en `/usr/local/lib`:

```bash
g++ -std=c++17 -Isrc -Ivendor/include tests/ResourcesManagerTest.cpp src/ResourcesManager.cpp -L/usr/local/lib -lraylib -lX11 -lm -lpthread -ldl -lrt -o /tmp/resources-test
/tmp/resources-test
```

Si usas la biblioteca de `vendor/lib`, cambia la opcion `-L`. La prueba necesita
una sesion grafica: abre una ventana oculta, carga ambos sprites y comprueba
sus dimensiones, su transparencia y el rechazo de un numero de jugador invalido.
