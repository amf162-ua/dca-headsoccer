# Documentación del proyecto: Head Soccer (versión mejorada)

> Documento vivo del equipo. Aquí se recoge todo lo que tenga que ver con el proyecto: requisitos de la asignatura, diseño del juego, raylib, arquitectura, físicas, animaciones, recursos, optimización, testing, CI y organización.
> Cada vez que alguien aprenda algo útil o se tome una decisión, se añade aquí.

## Índice

1. [Requisitos de la asignatura](#1-requisitos-de-la-asignatura)
2. [Análisis del Head Soccer original](#2-análisis-del-head-soccer-original)
3. [Nuestra propuesta: qué mejoramos](#3-nuestra-propuesta-qué-mejoramos)
4. [Banco de ideas originales (con coste estimado)](#4-banco-de-ideas-originales-con-coste-estimado)
5. [Estado actual del código base y problemas detectados](#5-estado-actual-del-código-base-y-problemas-detectados)
6. [Arquitectura propuesta](#6-arquitectura-propuesta)
7. [Guía rápida de raylib para este juego](#7-guía-rápida-de-raylib-para-este-juego)
8. [Físicas: cómo hacerlas bien](#8-físicas-cómo-hacerlas-bien)
9. [Animaciones: cómo hacerlas y de dónde sacarlas](#9-animaciones-cómo-hacerlas-y-de-dónde-sacarlas)
10. [Gestor de recursos (requisito obligatorio)](#10-gestor-de-recursos-requisito-obligatorio)
11. [Inteligencia artificial del rival](#11-inteligencia-artificial-del-rival)
12. [Game feel ("juice")](#12-game-feel-juice)
13. [Optimización y rendimiento](#13-optimización-y-rendimiento)
14. [Idiomas (i18n)](#14-idiomas-i18n)
15. [Compilación sin Makefiles](#15-compilación-sin-makefiles)
16. [Testing (≥ 10 tests)](#16-testing--10-tests)
17. [CI/CD con GitHub Actions y releases](#17-cicd-con-github-actions-y-releases)
18. [Gestión del repositorio: ramas, bugtracking y versionado](#18-gestión-del-repositorio-ramas-bugtracking-y-versionado)
19. [Requisitos secundarios (+2 puntos)](#19-requisitos-secundarios-2-puntos)
20. [Reparto de roles y hoja de ruta](#20-reparto-de-roles-y-hoja-de-ruta)
21. [Recursos y referencias](#21-recursos-y-referencias)

---

## 1. Requisitos de la asignatura

Fuentes: [Proyecto (p00)](https://antoniorv6.github.io/dca-gii/practicas/p00/) · [Hito 1 (p01)](https://antoniorv6.github.io/dca-gii/practicas/p01/)

### 1.1 Requisitos generales del proyecto

| Aspecto | Requisito |
|---|---|
| Lenguaje / librería | C++ con raylib, juego 2D |
| Equipo | 3–4 personas con roles (gráficos, sonido, mecánicas, físicas, IA, motor) |
| Pantallas | Mínimo 3: menú principal, juego y game over (valen más si hay más) |
| Gráficos | Sprites + **gestor de contenido multimedia** que evite cargar lo mismo varias veces |
| Sistemas operativos | **Linux y Windows**, con ejecutable para ambos |
| Idiomas | **Inglés y español** como mínimo (i valencià :D) |
| Tests | **Suite de al menos 10 tests** |
| CI/CD | **GitHub Actions** |
| Gestión | Sistema de tareas y bugtracking |
| Releases | Publicadas en GitHub |
| Evaluación | **Si no compila, un 0.** Se valoran la calidad del código, la documentación y las respuestas a preguntas técnicas. La nota puede ser individual si la participación es desigual |

### 1.2 Hito 1 (Alpha): entrega el **28 de octubre de 2026**

- [ ] Al menos 2 pantallas: la de juego y una de **game over que permita reiniciar**.
- [ ] Mecánica principal con un bucle jugable completo.
- [ ] Gráficos con **figuras básicas** (sprites opcionales en este hito).
- [ ] **NO usar Makefiles** (ver sección 15).
- [ ] Estrategia de ramas definida y **justificada**.
- [ ] Bugtracking funcionando y **justificado**.
- [ ] Releases con **versionado semántico** (p. ej. `v0.1.0`).
- [ ] Documento Markdown con: **GDD básico**, justificación de ramas y justificación de bugtracking.
- [ ] Cualquier librería extra, documentada y justificada.
- [ ] Entregable: `.zip` o `.tar.gz` con la release compilada, el enlace al código y la documentación.
- [ ] Las revisiones del profesor llegarán como **tickets**, que hay que gestionar bien.

### 1.3 Ventaja de Head Soccer para la nota

Un Head Soccer es **multijugador local por naturaleza** (1 contra 1 en el mismo teclado o con mandos) y **necesita una IA** para el modo de un jugador. Así tenemos casi gratis dos requisitos secundarios: **multijugador local** e **IA**. Si además añadimos soporte de mandos, sumamos un tercero (ver sección 19).

---

## 2. Análisis del Head Soccer original

Head Soccer (D&D Dream, 2012, móvil y Flash) es un fútbol 1 contra 1 de vista lateral con personajes "cabezones":

- **Controles:** izquierda, derecha, saltar, chutar y súper tiro.
- **Física arcade:** el balón rebota con los jugadores, el suelo, las paredes y el larguero.
- **Partidos cortos** con tiempo límite; si hay empate, suele haber gol de oro o empate.
- **Súper poder:** una barra se llena durante el partido y permite lanzar un tiro especial propio de cada personaje (fuego, hielo, tornado...).
- **Power-ups aleatorios** que caen al campo (cabeza grande, jugador pequeño, portería congelada, balón dividido...).
- **Muchos personajes** que se desbloquean. Modos: arcade, torneo, liga, supervivencia, headcup...

### Lo que falla del original (y podemos mejorar)

| Problema | Cómo lo mejoramos |
|---|---|
| La IA hace trampas (reacciones perfectas, súper tiros injustos) | IA "honesta": tiempo de reacción y errores configurables según la dificultad |

| Los power-ups aleatorios deciden partidos sin que el jugador haga nada | Power-ups que hay que **ganarse** o que tienen contra-juego |

| Profundidad técnica escasa: tirar es pulsar un botón | Tiro cargado, tiros con efecto y bloqueo con timing |

| Los estadios son solo decoración | **Estadios con reglas propias** |

| Progresión basada en pagar o repetir mucho | Modo torneo roguelite con decisiones entre partidos |

| Sin repeticiones ni sensación de "momento épico" | **Repetición del gol** a cámara lenta y opción de skip |

---

## 3. Nuestra propuesta: qué mejoramos

> ⚠️ **Pendiente de decisión del equipo.** Esta es una propuesta de partida.

**Concepto:** Head Soccer con **habilidad técnica** (tiro cargado, efecto, timing) + **estadios con reglas propias** + **modo torneo roguelite** en el que, tras cada victoria, eliges 1 mejora de entre 3.

Pilares de diseño:

1. **Fácil de aprender, difícil de dominar:** 4 acciones básicas y matices de timing.
2. **Cada partido es distinto:** estadios y mejoras cambian cómo se juega.
3. **Momentos épicos:** hitstop, cámara lenta, repetición del gol, pantalla temblando.
4. **Justo:** nada decide un partido sin que el jugador lo haya provocado.

### GDD mínimo (para el Hito 1)

| Elemento | Hito 1 (Alpha) | Hitos posteriores |
|---|---|---|
| Pantallas | Juego, Game Over (reiniciar); idealmente también Menú | Menú, selección de personaje/estadio, pausa, repetición, opciones, créditos |
| Jugadores | 2 (P1 y P2 locales, o P1 contra una IA simple) | Personajes con estadísticas y súper tiros propios |
| Mecánicas | Moverse, saltar, chutar, rebotes, goles, marcador y tiempo | Tiro cargado, efecto, súper barra, power-ups y estadios |
| Gráficos | Círculos (cabeza, balón), rectángulos (cuerpo, pie, porterías) | Sprites y animaciones |
| Final de partido | Al acabar el tiempo o al llegar a N goles, Game Over muestra el ganador | Gol de oro, penaltis |

**Controles propuestos (teclado):**

| Acción | Jugador 1 | Jugador 2 |
|---|---|---|
| Moverse | `A` / `D` | `←` / `→` |
| Saltar | `W` | `↑` |
| Chutar | `Espacio` | `Ctrl dcho.` o `Numpad 0` |
| Súper tiro | `S` | `↓` |
| Pausa | `Esc` / `P` | — |

> ⚠️ **Ghosting del teclado:** muchos teclados no registran más de 2 o 3 teclas pulsadas a la vez en ciertas combinaciones. Hay que probar las combinaciones con dos personas jugando a la vez. Los mandos lo resuelven (ver sección 7.4).

---

## 4. Banco de ideas originales (con coste estimado)

Coste: 🟢 bajo (< 1 día) · 🟡 medio (2–4 días) · 🔴 alto (> 1 semana). Impacto: ★ a ★★★.

### Mecánicas de habilidad
| Idea | Descripción | Coste | Impacto |
|---|---|---|---|
| **Tiro cargado** | Mantener chutar carga la fuerza y soltar dispara. Un círculo alrededor del pie muestra la carga | 🟢 | ★★★ |
| **Tiro con efecto** | La dirección pulsada al chutar da efecto al balón, que curva en el aire (fuerza lateral proporcional al giro) | 🟡 | ★★★ |
| **Parry / bloqueo perfecto** | Chutar justo cuando el balón llega (ventana de ~100 ms) lo devuelve más fuerte, con hitstop y un destello | 🟢 | ★★★ |
| **Choque de pies** | Si los dos chutan el balón en el mismo frame, hay un "clash": el balón sale disparado hacia arriba y la pantalla tiembla | 🟢 | ★★ |
| **Doble salto / dash** | Movilidad extra con tiempo de recarga | 🟢 | ★★ |
| **Súper barra por buen juego** | Se llena con parries, remates y paradas, no con el paso del tiempo | 🟢 | ★★ |

### Estadios con reglas propias (la mejora con más personalidad)
| Estadio | Regla | Coste |
|---|---|---|
| **Luna** | Gravedad al 40 %, saltos enormes | 🟢 (cambiar un parámetro) |
| **Tormenta** | Viento lateral que cambia de sentido cada 15 s (se ve con partículas) | 🟢 |
| **Pista de hielo** | Poca fricción: los jugadores patinan | 🟢 |
| **Porterías móviles** | Las porterías suben y bajan | 🟡 |
| **Pinball** | Bumpers en el campo que rebotan el balón | 🟡 |
| **Noche** | Solo se ve un foco alrededor del balón y de los jugadores (círculos con alfa o un shader) | 🟡 |
| **Terremoto** | Cada cierto tiempo el suelo se inclina | 🔴 |

### Modos de juego
| Modo | Descripción | Coste |
|---|---|---|
| **Versus local** | 1 contra 1 en el mismo PC (requisito secundario) | 🟢 |
| **Contra la IA** | Varias dificultades | 🟡 |
| **Torneo roguelite** | Sucesión de rivales; tras cada victoria eliges 1 de 3 mejoras (cabeza +20 %, balón más pesado para el rival, doble salto, portería del rival más ancha...). Si pierdes, empiezas de cero. Gran rejugabilidad con poco contenido | 🟡 |
| **Balón bomba** | El balón explota a los X segundos; si explota en tu campo, pierdes un punto | 🟢 |
| **Supervivencia** | La IA mejora tras cada gol que marcas | 🟢 |
| **2 contra 2 local** | Con mandos | 🟡 |
| **Penaltis** | Para desempatar | 🟡 |

### Presentación y extras
| Idea | Descripción | Coste |
|---|---|---|
| **Repetición del gol** | Buffer circular con los últimos 3–4 s de estado que se reproduce a cámara lenta. Encaja con la máquina de estados: `push(ReplayState)` | 🟡 |
| **Estadísticas del partido** | Posesión, tiros y parries en la pantalla de Game Over | 🟢 |
| **Personajes procedurales** | En el Hito 1, cabezas hechas con primitivas (color, pelo con triángulos, ojos que siguen al balón) | 🟢 |
| **Editor de personaje** | Elegir color, ojos, pelo... y guardarlo en un fichero | 🟡 |
| **Logros** | "Gana sin encajar", "10 parries"... | 🟢 |
| **Público reactivo** | Rectángulos que saltan cuando hay gol | 🟢 |

> **Recomendación:** para el Hito 1, solo lo básico más **tiro cargado + parry**, que son baratos y ya nos diferencian. Para hitos posteriores: estadios, torneo roguelite y repetición.

---

## 5. Estado actual del código base y problemas detectados

El repositorio parte de la plantilla del profesor (`GameState`, `StateMachine`, `MainGameState`, `main.cpp`). Problemas que hay que corregir antes de construir encima:

| # | Archivo | Problema | Solución |
|---|---|---|---|
| 1 | `main.cpp` | No se llama a `InitWindow`, `SetTargetFPS` ni `CloseWindow`, y no se incluye `raylib.h` | Inicializar raylib antes del bucle y cerrar la ventana al salir |
| 2 | `main.cpp` | `delta_time` nunca se actualiza (siempre vale 0) | `delta_time = GetFrameTime();` en cada iteración (o usar un paso fijo, sección 8.1) |
| 3 | `main.cpp` | El bucle no comprueba `WindowShouldClose()`, así que cerrar la ventana no sale del juego | `while (!state_machine.is_game_ending() && !WindowShouldClose())` |
| 4 | `StateMachine.hpp` | `is_running` no está inicializado (comportamiento indefinido) | `bool is_running = true;` |
| 5 | `StateMachine.cpp` | En `handle_state_changes`, si se hace pop del último estado sin añadir otro, se llama a `top()->resume()` con la pila vacía (UB) | Comprobar `!states_machine.empty()` antes de `top()` |
| 6 | `StateMachine.hpp` | `getCurrentState()` sobre una pila vacía también es UB | Comprobar o documentar la precondición |
| 7 | `*.cpp` | `#include <GameState.hpp>` con `<>` exige `-Isrc` al compilar | Usar `"GameState.hpp"` o configurar los include dirs en el build |
| 8 | `MainGameState.hpp` | `pause()` y `resume()` no llevan `override`; `entered_key` no se usa | Añadir `override` y quitar lo que sobre |
| 9 | Varios | Includes sin usar (`<chrono>`, `<unordered_map>`, `<iostream>`) | Limpiar |
| 10 | `render()` | Algún sitio tiene que llamar a `BeginDrawing()` y `EndDrawing()` | Hacerlo en `main.cpp` alrededor de `render()` para que ningún estado lo olvide |

**`main.cpp` corregido (orientativo):**

```cpp
#include "raylib.h"
#include "StateMachine.hpp"
#include "MainGameState.hpp"
#include <memory>

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Head Soccer DCA");
    InitAudioDevice();
    SetExitKey(KEY_NULL);            // Esc no cierra el juego: lo usaremos para pausar
    SetTargetFPS(60);

    float delta_time = 0.0f;
    StateMachine state_machine;
    state_machine.add_state(std::make_unique<MainGameState>(), false);
    state_machine.handle_state_changes(delta_time);

    while (!state_machine.is_game_ending() && !WindowShouldClose())
    {
        delta_time = GetFrameTime();
        state_machine.handle_state_changes(delta_time);

        auto& state = state_machine.getCurrentState();
        state->handleInput();
        state->update(delta_time);

        BeginDrawing();
        state->render();
        EndDrawing();
    }

    // Liberar recursos (ResourceManager::clear()) ANTES de cerrar el contexto
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
```

---

## 6. Arquitectura propuesta

### 6.1 Principio clave: separar la lógica de la presentación

```
src/
├── core/            ← lógica pura: física, reglas, marcador, IA. SIN llamadas de dibujo ni ventana
│   ├── Physics.hpp/.cpp
│   ├── Match.hpp/.cpp        (marcador, tiempo, goles, saques)
│   ├── Player.hpp/.cpp
│   ├── Ball.hpp/.cpp
│   └── AI.hpp/.cpp
├── game/            ← estados, renderizado, input, audio (usa raylib)
│   ├── states/  MenuState, SelectState, MatchState, PauseState, ReplayState, GameOverState
│   ├── render/  PlayerRenderer, BallRenderer, Hud, Particles
│   └── input/   InputMap (teclado y mando → acciones)
├── engine/          ← StateMachine, ResourceManager, Localization, Config
└── main.cpp
tests/               ← tests de core/ y engine/ (sin ventana)
assets/              ← fonts/, sounds/, music/, textures/, lang/
```

**Por qué:** si `core/` no llama a funciones de ventana o de dibujo, podemos testear goles, rebotes, el tiempo y la IA sin abrir ventana, y en la CI de GitHub (que no tiene pantalla). Usar `Vector2` y las funciones de `raymath.h` en `core/` no da problemas, porque son solo matemáticas.

### 6.2 Flujo de estados

```
Menu ──► Selección ──► Partido ──(gol)──► push Repetición ──pop──► Partido
  ▲                      │  ▲                                        │
  │                      │  └──pop── Pausa ◄──push (Esc)             │
  └──── Game Over ◄──────┴────────────(fin de tiempo o goles)────────┘
```

- **Pausa** y **Repetición** se *apilan* (`add_state(..., false)`) y se quitan con `remove_state(false)`, así el partido se conserva debajo. Para eso existe la pila de la plantilla.
- **Menú → Partido → Game Over** se *reemplazan* (`add_state(..., true)`).
- Para que la pausa dibuje el partido congelado de fondo: guardar un `RenderTexture2D` del último frame o que el estado Pausa tenga un puntero de solo lectura al partido.

### 6.3 Separar las acciones del hardware

```cpp
enum class Action { Left, Right, Jump, Kick, Super, Pause };

struct InputMap {
    // Cada jugador tiene teclas y, opcionalmente, un mando
    bool down(int player, Action a) const;
    bool pressed(int player, Action a) const;
};
```

El juego solo pregunta por *acciones*. Así, mandos, IA, reasignación de teclas y repeticiones son intercambiables: **la IA es otro "input" que genera acciones**.

### 6.4 Datos del juego en ficheros

Guardar en ficheros de texto simples (`.ini`, `.txt` o JSON con [nlohmann/json](https://github.com/nlohmann/json)) los parámetros de gravedad, fuerza de tiro, tamaño de las porterías, estadísticas de los personajes y la configuración de los estadios. Ventajas: se ajusta el juego sin recompilar y el diseñador puede trabajar sin tocar código.

---

## 7. Guía rápida de raylib para este juego

- Versión recomendada: **raylib 5.5** (o la última estable). Todos debemos usar **la misma versión**.
- Chuleta oficial imprescindible: <https://www.raylib.com/cheatsheet/cheatsheet.html>
- Ejemplos (con código): <https://www.raylib.com/examples.html>
- raymath (vectores): <https://github.com/raysan5/raylib/blob/master/src/raymath.h>

### 7.1 Dibujar con primitivas (Hito 1)

| Elemento | Funciones |
|---|---|
| Cabeza y balón | `DrawCircleV(pos, r, color)`, `DrawCircleLinesV` para el contorno |
| Ojos que miran al balón | Dos `DrawCircleV` blancos con pupilas desplazadas hacia `Vector2Normalize(ball - eye) * k` |
| Pie o bota que gira al chutar | `DrawRectanglePro(rec, origin, rotation, color)`: rota sobre el origen (el "tobillo") |
| Porterías | `DrawRectangleRec` para los postes; red con `DrawLineV` en rejilla |
| Césped | `DrawRectangleGradientV` + franjas alternas |
| Barra de carga o súper | `DrawRectangleRec` + `DrawRectangleLinesEx`, o `DrawRing` / `DrawCircleSector` para una barra circular |
| Balón con gajos | Círculo + `DrawCircleSector` o líneas rotadas según el ángulo de giro del balón |
| Pelo y cuernos | `DrawTriangle` (vértices en **sentido antihorario** o no se ve) |
| Sombras | `DrawEllipse` con un color semitransparente bajo cada jugador |

### 7.2 Texto

- `DrawText` usa la fuente por defecto, que **no tiene tildes ni ñ** (solo ASCII). Para español hay que cargar una fuente con esos caracteres:

```cpp
// Cargar solo los glifos que necesitamos (ASCII + español)
int count = 0;
int* cps = LoadCodepoints(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~áéíóúÁÉÍÓÚñÑüÜ¿¡", &count);
Font font = LoadFontEx("assets/fonts/MiFuente.ttf", 48, cps, count);
UnloadCodepoints(cps);
SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
DrawTextEx(font, "¡GOL!", {x, y}, 48, 2, WHITE);
```

- Para centrar texto: `MeasureTextEx(font, text, size, spacing)`.

### 7.3 Input

```cpp
IsKeyDown(KEY_A);        // mantenida (movimiento, cargar tiro)
IsKeyPressed(KEY_W);     // solo el frame en que se pulsa (saltar)
IsKeyReleased(KEY_SPACE) // soltar (disparar el tiro cargado)
```

### 7.4 Mandos (requisito secundario)

```cpp
if (IsGamepadAvailable(0)) {
    float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);   // -1..1, aplicar zona muerta de ~0.2
    bool jump = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A / X
    bool kick = IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);    // X / □
}
```

- Nombres de botones genéricos (`RIGHT_FACE_DOWN` = A en Xbox, X en PlayStation).
- raylib trae una base de datos de mandos SDL. Si alguno no se reconoce: `SetGamepadMappings()`.

### 7.5 Audio

```cpp
InitAudioDevice();                              // una vez, tras InitWindow
Sound kick = LoadSound("assets/sounds/kick.wav"); // efectos cortos (se cargan en memoria)
Music music = LoadMusicStream("assets/music/stadium.ogg"); // música (streaming)
PlayMusicStream(music);
// en cada frame:
UpdateMusicStream(music);                       // si se olvida, la música se corta
// variación para que no suene repetitivo:
SetSoundPitch(kick, 0.9f + GetRandomValue(0, 20) / 100.0f);
PlaySound(kick);
```

- `PlaySound` sobre un `Sound` que ya suena lo **reinicia**. Para que varios suenen solapados: `LoadSoundAlias(kick)` (raylib ≥ 5.0) y usar varios alias por turnos.
- Formatos: `.wav` para efectos y `.ogg` para música (pesa poco y no da problemas de licencia).

### 7.6 Cámara 2D (temblor de pantalla y zoom en el gol)

```cpp
Camera2D cam = {0};
cam.offset = {w / 2.0f, h / 2.0f};   // centro de la pantalla
cam.target = {w / 2.0f, h / 2.0f};   // punto del mundo que se ve en el centro
cam.zoom = 1.0f;

BeginMode2D(cam);
  // dibujar el mundo
EndMode2D();
// dibujar el HUD fuera de BeginMode2D para que no tiemble ni haga zoom
```

### 7.7 Resolución virtual (se ve igual en cualquier monitor)

Trabajar siempre en coordenadas de 1280×720 (o 640×360 si se busca estilo pixel art), renderizar a un `RenderTexture2D` y escalarlo a la ventana con barras negras si hace falta:

```cpp
RenderTexture2D target = LoadRenderTexture(VW, VH);
// ...
BeginTextureMode(target);  ClearBackground(SKYBLUE);  /* dibujar juego */  EndTextureMode();

BeginDrawing();
ClearBackground(BLACK);
float scale = fminf((float)GetScreenWidth() / VW, (float)GetScreenHeight() / VH);
Rectangle src = {0, 0, (float)VW, -(float)VH};          // altura NEGATIVA: la textura va invertida en Y
Rectangle dst = {(GetScreenWidth() - VW * scale) / 2, (GetScreenHeight() - VH * scale) / 2, VW * scale, VH * scale};
DrawTexturePro(target.texture, src, dst, {0, 0}, 0, WHITE);
EndDrawing();
```

⚠️ Con resolución virtual, la posición del ratón hay que convertirla: `(GetMousePosition() - offset) / scale`.

### 7.8 Funciones útiles que conviene conocer

- `GetRandomValue(min, max)`: aleatorio entero (para la lógica testeable mejor `<random>` con semilla).
- `Clamp`, `Lerp`, `Vector2Lerp`, `Vector2Reflect`, `Vector2Rotate` (raymath).
- `CheckCollisionCircles`, `CheckCollisionCircleRec`, `CheckCollisionRecs`, `GetCollisionRec`.
- `ColorAlpha(color, 0.5f)`, `ColorFromHSV(h, s, v)` (colores de equipo o arcoíris en el súper tiro).
- `TakeScreenshot("x.png")`: capturas para la documentación y la release.
- `SetWindowIcon`, `ToggleFullscreen` / `ToggleBorderlessWindowed`.
- `GetTime()`: tiempo desde el inicio, útil para animaciones con `sinf`.

---

## 8. Físicas: cómo hacerlas bien

### 8.1 Paso de tiempo fijo (lo más importante)

Con `deltaTime` variable, la física cambia según los FPS: con lag se atraviesan paredes y los saltos tienen altura distinta en cada PC. La solución es el **paso fijo** ([Fix Your Timestep, Gaffer on Games](https://gafferongames.com/post/fix_your_timestep/)):

```cpp
constexpr float DT = 1.0f / 120.0f;   // física a 120 Hz
float accumulator = 0.0f;

while (running) {
    accumulator += fminf(GetFrameTime(), 0.25f);   // limitar para evitar la "espiral de la muerte"
    input.poll();                                  // leer input UNA vez por frame
    while (accumulator >= DT) {
        match.update(DT);                          // física determinista
        accumulator -= DT;
    }
    render(accumulator / DT);                      // alpha para interpolar posiciones (opcional)
}
```

- Ventaja extra: **física determinista**, lo que hace posibles tests reproducibles, repeticiones y (en el futuro) multijugador online.
- ⚠️ `IsKeyPressed` vale `true` un solo frame de render. Si en ese frame hay 0 o 2 pasos de física, la pulsación se pierde o se duplica. Hay que **guardarla en un buffer** ("salto pedido") y consumirla en el siguiente paso de física.

### 8.2 ¿Física propia o Box2D?

| | Física propia | Box2D v3 |
|---|---|---|
| Complejidad | Baja: círculos + rectángulos | Media: API nueva, unidades en metros |
| Control del "feel" arcade | Total | Hay que pelearse con ella |
| Puntos | — | Cuenta como requisito secundario |
| Testeable | Muy fácil | Posible |

**Recomendación:** física propia en el Hito 1 (el juego solo necesita círculo-círculo y círculo-rectángulo). Box2D solo si el equipo quiere los puntos extra y hay tiempo.

### 8.3 Modelo físico

- **Balón:** círculo con velocidad, gravedad, restitución (~0.7–0.8), fricción del aire leve y velocidad angular (para dibujar el giro y el efecto).
- **Jugador:** *cabeza* (círculo) + *cuerpo* (rectángulo o círculo) + *pie* (rectángulo o cápsula que rota al chutar). El jugador es **cinemático** frente al balón (masa infinita): empuja al balón pero el balón no lo mueve.
- **Escenario:** suelo, paredes, techo invisible, **larguero** y postes (rectángulos). La red es una zona de gol.

### 8.4 Colisión círculo–círculo (balón contra cabeza)

```cpp
struct Body { Vector2 pos, vel; float radius, invMass; };   // invMass = 0 → inamovible

void resolveCircleCircle(Body& a, Body& b, float restitution)
{
    Vector2 d = Vector2Subtract(b.pos, a.pos);
    float dist = Vector2Length(d);
    float penetration = a.radius + b.radius - dist;
    float invSum = a.invMass + b.invMass;
    if (penetration <= 0.0f || dist == 0.0f || invSum == 0.0f) return;

    Vector2 n = Vector2Scale(d, 1.0f / dist);            // normal de a hacia b

    // 1) Separar para que no queden solapados
    a.pos = Vector2Subtract(a.pos, Vector2Scale(n, penetration * a.invMass / invSum));
    b.pos = Vector2Add     (b.pos, Vector2Scale(n, penetration * b.invMass / invSum));

    // 2) Impulso solo si se están acercando
    float vn = Vector2DotProduct(Vector2Subtract(b.vel, a.vel), n);
    if (vn > 0.0f) return;
    float j = -(1.0f + restitution) * vn / invSum;
    a.vel = Vector2Subtract(a.vel, Vector2Scale(n, j * a.invMass));
    b.vel = Vector2Add     (b.vel, Vector2Scale(n, j * b.invMass));
}
```

Como la cabeza tiene `invMass = 0`, el balón sale rebotado **y además** hereda la velocidad del jugador. Así los cabezazos en carrera y en salto funcionan solos.

### 8.5 Colisión círculo–rectángulo (larguero, postes, suelo)

```cpp
// Punto del rectángulo más cercano al centro del círculo
Vector2 closest = { Clamp(c.x, r.x, r.x + r.width), Clamp(c.y, r.y, r.y + r.height) };
Vector2 d = Vector2Subtract(c, closest);
float dist2 = Vector2LengthSqr(d);
if (dist2 < radius * radius && dist2 > 0.0f) {
    float dist = sqrtf(dist2);
    Vector2 n = Vector2Scale(d, 1.0f / dist);
    ball.pos = Vector2Add(ball.pos, Vector2Scale(n, radius - dist));       // sacar
    float vn = Vector2DotProduct(ball.vel, n);
    if (vn < 0) ball.vel = Vector2Subtract(ball.vel, Vector2Scale(n, (1 + e) * vn)); // reflejar
}
// Caso raro: el centro ya está DENTRO del rectángulo (dist2 == 0) → empujar por el lado de menor penetración
```

### 8.6 El chute

Opción sencilla y con buen tacto:

1. Al chutar, el pie rota de 0° a ~70° en ~80 ms (`DrawRectanglePro` para dibujarlo).
2. Durante esa ventana ("frames activos"), si la hitbox del pie toca el balón, se aplica un impulso **fijo** en la dirección del chute (hacia arriba y hacia la portería rival) escalado por la carga: `ball.vel = dirección * lerp(minFuerza, maxFuerza, carga)`.
3. Un solo impacto por chute (flag `hasHit`) para no aplicar el impulso varios frames seguidos.
4. **Parry:** si el balón venía rápido hacia el jugador y el chute empezó hace menos de ~100 ms → multiplicador ×1.5, hitstop y destello.

### 8.7 Efecto (efecto Magnus simplificado)

```cpp
// spin > 0 → curva hacia arriba si va a la derecha, etc.
Vector2 perp = { -ball.vel.y, ball.vel.x };
ball.vel = Vector2Add(ball.vel, Vector2Scale(perp, MAGNUS_K * ball.spin * DT));
ball.spin *= powf(SPIN_DAMPING, DT);   // el giro se pierde con el tiempo
```

### 8.8 Problemas típicos y soluciones

| Problema | Causa | Solución |
|---|---|---|
| El balón atraviesa el larguero | Velocidad alta (tunneling) | Paso fijo a 120 Hz y **subpasos** (`n = ceil(velocidad * DT / (radio * 0.5))`) |
| El balón queda "pegado" en la cabeza | Separación incompleta | Corrección de posición (paso 1 del apartado 8.4) |
| El balón rebota eternamente en el suelo | Rebotes diminutos | Si `|vel.y| < umbral` → `vel.y = 0` |
| El balón se queda quieto sobre la cabeza del rival | Equilibrio perfecto | Añadir un pequeño empuje lateral aleatorio o un temporizador que lo deje caer |
| El balón se atasca en el techo de la portería | Geometría | Hacer el larguero redondeado (círculo en la punta) |
| Resultados distintos en cada PC | `deltaTime` variable | Paso fijo (8.1) |

---

## 9. Animaciones: cómo hacerlas y de dónde sacarlas

### 9.1 Hito 1: animación procedural (sin sprites)

Sin sprites se pueden conseguir animaciones con mucha vida usando solo matemáticas:

| Técnica | Uso | Cómo |
|---|---|---|
| **Squash & stretch** | Aterrizar, rebote del balón | Escalar el radio en X e Y en sentidos opuestos (`DrawEllipse`) según la velocidad o el impacto |
| **Easing / tweening** | Pie al chutar, menús, marcador | Funciones de easing (`EaseOutBack`, `EaseOutElastic`...) → [reasings.h](https://github.com/raysan5/raylib/blob/master/examples/others/reasings.h) o <https://easings.net> |
| **Balanceo en reposo (idle)** | Respiración del personaje | `offsetY = sinf(GetTime() * 3) * 2` |
| **Ojos que siguen al balón** | Personalidad | Pupila = centro del ojo + dirección normalizada al balón × radio pequeño |
| **Parpadeo** | Personalidad | Cada 2–5 s aleatorios, dibujar el ojo como línea durante 100 ms |
| **Rotación del balón** | Sensación de velocidad | `angle += vel.x / radius * dt` (rodar sin deslizar) y dibujar los gajos rotados |
| **Estelas** | Tiros fuertes | Buffer de las últimas N posiciones dibujadas con alfa decreciente |
| **Partículas** | Gol, choques, césped | Pool de partículas (ver sección 13) |

```cpp
// Tween mínimo: se usa para cualquier valor animado
struct Tween {
    float from, to, duration, t = 0.0f;
    float (*ease)(float) = [](float x) { return x; };
    float value() const { return from + (to - from) * ease(fminf(t / duration, 1.0f)); }
    void update(float dt) { t += dt; }
    bool done() const { return t >= duration; }
};

float easeOutBack(float x) { const float c1 = 1.70158f, c3 = c1 + 1; return 1 + c3 * powf(x - 1, 3) + c1 * powf(x - 1, 2); }
```

### 9.2 Hitos posteriores: sprites y *sprite sheets*

**Cómo animar un sprite sheet en raylib:**

```cpp
struct Animation {
    int row;          // fila del sheet (idle, correr, saltar, chutar...)
    int frameCount;
    float fps;
    bool loop = true;
};

struct Animator {
    const Texture2D* sheet;
    int frameW, frameH;
    Animation current;
    float time = 0.0f;

    void play(const Animation& a) { if (a.row != current.row) { current = a; time = 0; } }
    void update(float dt) { time += dt; }

    int frame() const {
        int f = (int)(time * current.fps);
        return current.loop ? f % current.frameCount : std::min(f, current.frameCount - 1);
    }

    void draw(Vector2 pos, bool flipX, float scale) const {
        Rectangle src = { (float)(frame() * frameW), (float)(current.row * frameH),
                          (float)(flipX ? -frameW : frameW), (float)frameH };   // ancho negativo = espejo
        Rectangle dst = { pos.x, pos.y, frameW * scale, frameH * scale };
        DrawTexturePro(*sheet, src, dst, { dst.width / 2, dst.height }, 0.0f, WHITE); // origen: pies
    }
};
```

- Para pixel art: `SetTextureFilter(tex, TEXTURE_FILTER_POINT)` y escalar por **enteros** (×2, ×3), o se verá borroso.
- Para los dos jugadores: el mismo sprite **espejado** con ancho negativo y **tintado** con `DrawTexturePro(..., tint)` para diferenciar equipos sin duplicar arte.
- **Personajes "por partes"** (cabeza, cuerpo y pie en texturas separadas, animados con rotaciones y tweens) = estilo del Head Soccer original. Se necesita **mucho menos arte** que con animaciones fotograma a fotograma y se reutiliza el código procedural del Hito 1. **Recomendado.**

### 9.3 De dónde sacar arte (revisar SIEMPRE la licencia)

| Fuente | Qué hay | Licencia típica |
|---|---|---|
| [Kenney.nl](https://kenney.nl/assets) | Sprites, UI, fuentes, **audio**, packs deportivos. Coherente y de calidad | **CC0** (uso libre, sin atribución) ⭐ |
| [OpenGameArt.org](https://opengameart.org) | De todo, calidad variable | CC0, CC-BY, CC-BY-SA, GPL (mirar cada asset) |
| [itch.io, game assets gratis](https://itch.io/game-assets/free) | Packs de pixel art y personajes animados | Variable; leer la página de cada pack |
| [CraftPix, freebies](https://craftpix.net/freebies/) | Personajes, fondos, UI | Licencia propia (permite uso en juegos, no redistribuir los assets sueltos) |
| [Game-icons.net](https://game-icons.net) | Iconos para power-ups | CC-BY 3.0 |
| [Google Fonts](https://fonts.google.com) | Fuentes (con ñ y tildes) | OFL |

**Herramientas para hacer nuestro propio arte:**

| Herramienta | Uso | Precio |
|---|---|---|
| [LibreSprite](https://libresprite.github.io) | Pixel art y animación (fork libre de Aseprite) | Gratis |
| [Aseprite](https://www.aseprite.org) | El estándar para pixel art; exporta sprite sheets + JSON | De pago, o gratis si se compila desde el código fuente |
| [Piskel](https://www.piskelapp.com) | Pixel art en el navegador, exporta sheets | Gratis |
| [Krita](https://krita.org) | Ilustración, fondos | Gratis |
| [Inkscape](https://inkscape.org) | Vectorial: cabezas y fondos limpios, estilo cartoon | Gratis |
| [rTexPacker](https://raylibtech.itch.io/rtexpacker) | Empaquetar sprites en un atlas (del autor de raylib) | Gratis / de pago |

### 9.4 Audio

| Fuente o herramienta | Uso |
|---|---|
| [jsfxr](https://sfxr.me) / [ChipTone](https://sfbgames.itch.io/chiptone) | Generar efectos retro (chute, rebote, silbato) en segundos. Libres |
| [Kenney Audio](https://kenney.nl/assets?q=audio) | Efectos CC0 |
| [Freesound.org](https://freesound.org) | Sonidos reales (público, silbato, red). **Mirar la licencia CC de cada uno** |
| [BeepBox](https://www.beepbox.co) | Componer música chiptune en el navegador |
| [Audacity](https://www.audacityteam.org) | Cortar, normalizar y exportar a `.ogg` |
| [OpenGameArt, música](https://opengameart.org/art-search-advanced?field_art_type_tid%5B%5D=12) | Música con licencia libre |

> 📌 **Obligatorio para nosotros:** crear un `CREDITS.md` con **cada asset externo, su autor, su URL y su licencia**. Así cumplimos las licencias CC-BY y lo agradecerá el profesor ("documentar y justificar").

---

## 10. Gestor de recursos (requisito obligatorio)

Objetivo: **no cargar dos veces el mismo recurso** y liberar todo al final.

```cpp
// engine/ResourceManager.hpp
#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

class ResourceManager {
public:
    static ResourceManager& instance() { static ResourceManager rm; return rm; }

    Texture2D& texture(const std::string& path) {
        auto it = textures_.find(path);
        if (it == textures_.end())
            it = textures_.emplace(path, LoadTexture(path.c_str())).first;   // se carga solo la primera vez
        return it->second;
    }
    Sound& sound(const std::string& path) { /* igual */ }
    Font&  font (const std::string& path, int size) { /* clave = path + "#" + size */ }

    // Llamar ANTES de CloseAudioDevice()/CloseWindow()
    void clear() {
        for (auto& [_, t] : textures_) UnloadTexture(t);
        for (auto& [_, s] : sounds_)   UnloadSound(s);
        for (auto& [_, f] : fonts_)    UnloadFont(f);
        textures_.clear(); sounds_.clear(); fonts_.clear();
    }

private:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    std::unordered_map<std::string, Texture2D> textures_;
    std::unordered_map<std::string, Sound>     sounds_;
    std::unordered_map<std::string, Font>      fonts_;
};
```

**Cuidados:**
- ⚠️ **No liberar en el destructor del singleton.** Los estáticos se destruyen *después* de `main`, cuando `CloseWindow()` ya ha destruido el contexto OpenGL, y eso puede dar un crash al salir. Hay que llamar a `clear()` explícitamente.
- ⚠️ `LoadTexture` **necesita una ventana ya creada** (contexto OpenGL).
- Comprobar errores: `IsTextureValid(t)` / `IsSoundValid(s)` (raylib 5.5) y usar una textura "fallback" (cuadro rosa) si falta un archivo, en lugar de dejar que el juego se rompa.
- **Rutas:** el ejecutable puede lanzarse desde otra carpeta. Usar `ChangeDirectory(GetApplicationDirectory())` al inicio para que `assets/` se encuentre siempre.
- Precargar los recursos de un estado en su `init()`, **nunca** dentro de `update` o `render`.
- Como alternativa al singleton, pasar el `ResourceManager&` por referencia a los estados (más testeable). Debatir en el equipo.

---

## 11. Inteligencia artificial del rival

### 11.1 Máquina de estados (base recomendada)

```
            balón en mi campo y cerca                 balón alto sobre mí
  DEFENDER ─────────────────────────► ATACAR ───────────────────────► REMATAR (saltar + chutar)
     ▲   ◄───────── balón en campo rival ─┘                                     │
     └───────────────────────────── después del remate ◄───────────────────────┘
```

- **DEFENDER:** colocarse entre el balón y la propia portería.
- **ATACAR:** colocarse *detrás* del balón (en el lado de su propia portería) y chutar cuando esté a distancia.
- **REMATAR:** si el balón va a caer cerca, calcular dónde y saltar a tiempo.

### 11.2 Predecir dónde cae el balón

Con gravedad constante, la trayectoria es una parábola. Hay que resolver `y(t) = y0 + vy·t + ½·g·t² = yObjetivo`:

```cpp
// Tiempo hasta que el balón llegue a la altura targetY (primera raíz positiva, bajando)
float timeToY(float y0, float vy, float g, float targetY) {
    float a = 0.5f * g, b = vy, c = y0 - targetY;
    float disc = b * b - 4 * a * c;
    if (disc < 0) return -1;
    return (-b + sqrtf(disc)) / (2 * a);   // con el eje Y hacia abajo (raylib) y g > 0
}
float landingX = ball.pos.x + ball.vel.x * t;   // + rebotes en paredes si se sale del campo
```

Alternativa más sencilla y robusta: **simular** la física del balón hacia delante N pasos (copiando el estado) con el mismo código del juego. Así los rebotes quedan incluidos.

### 11.3 Dificultad "honesta"

| Parámetro | Fácil | Normal | Difícil |
|---|---|---|---|
| Tiempo de reacción | 350 ms | 200 ms | 100 ms |
| Error en la predicción de caída | ±60 px | ±25 px | ±8 px |
| Probabilidad de fallar un chute | 30 % | 10 % | 3 % |
| Uso del súper tiro | Nunca | Aleatorio | En el mejor momento |

- Implementar el tiempo de reacción como un **retraso en lo que "ve" la IA** (una cola de estados pasados), no como un temporizador: así parece humana.
- La IA **genera `Action`s** (sección 6.3). Nunca manipula la física directamente.
- Opciones avanzadas (puntos extra): árbol de decisión o *behavior tree*, *utility AI*, *steering behaviors* (`arrive` para frenar suavemente al llegar a la posición).

---

## 12. Game feel ("juice")

Lo que más diferencia un juego de universidad "correcto" de uno que **da gusto jugar**. Casi todo es 🟢 barato:

| Técnica | Cuándo | Implementación |
|---|---|---|
| **Screen shake** | Gol, parry, súper tiro | `trauma` en [0,1] que decae; desplazamiento = `maxOffset * trauma² * ruido`. Aplicarlo en `Camera2D.offset` |
| **Hitstop** | Chute fuerte, parry | Congelar la física 50–100 ms (saltarse los `update`) y seguir renderizando |
| **Cámara lenta** | Gol, balón cerca de la línea | Multiplicar el `dt` de la física por 0.2–0.3 durante 0.5 s |
| **Destello (flash)** | Impacto | Dibujar el jugador en blanco 2–3 frames |
| **Partículas** | Chute, gol (confeti), césped al aterrizar | Pool de partículas |
| **Estela** | Balón rápido | Últimas N posiciones con alfa decreciente |
| **Zoom** | Gol | Tween de `camera.zoom` 1.0 → 1.15 → 1.0 hacia la portería |
| **Sonido con variación** | Todo | Pitch aleatorio ±10 % |
| **Anuncio de "¡GOL!"** | Gol | Texto con `EaseOutElastic` en la escala |
| **Público** | Gol, ocasión clara | Rectángulos en la grada que saltan + sonido de ovación |

📺 Charla imprescindible: *Juice it or lose it* (Martin Jonasson y Petri Purho): <https://www.youtube.com/watch?v=Fy0aCDmgnxg>
📺 *The art of screenshake* (Jan Willem Nijman, Vlambeer): <https://www.youtube.com/watch?v=AJdEqssNZ-U>

---

## 13. Optimización y rendimiento

Head Soccer es un juego ligero (pocos objetos) y el rendimiento **no debería ser un problema** si se evitan estos errores:

### 13.1 Reglas de oro

1. **Nunca cargar recursos dentro del bucle** (`LoadTexture` o `LoadSound` en `update` o `render`). Es el error de rendimiento más común en raylib. Usar el `ResourceManager` y cargar en `init()`.
2. **No reservar memoria en cada frame:** nada de `new` o `std::vector` creados por frame. Usar `reserve()` y pools.
3. **Pool de partículas:** un array fijo de, por ejemplo, 512 partículas con un flag `alive`, reutilizando las muertas.
   ```cpp
   struct Particle { Vector2 pos, vel; float life; Color color; };
   std::array<Particle, 512> particles{};   // life <= 0 → libre
   ```
4. **Batching:** raylib agrupa automáticamente las llamadas de dibujo *consecutivas* que usan la **misma textura**. Con sprites, meterlos en **un atlas** (una sola textura) y dibujar agrupado por textura. Las primitivas (`DrawCircle`...) usan todas la misma textura interna, así que también se agrupan bien.
5. **Texto:** `DrawText` / `DrawTextEx` son baratos, pero **generar strings con `TextFormat` o `std::string` en cada frame** crea basura. Para el marcador basta con regenerarlo cuando cambie.
6. **VSync o `SetTargetFPS`:** usar `FLAG_VSYNC_HINT` (suave, sin tearing) o `SetTargetFPS(60)`. No hace falta más de 60 FPS de render con física fija a 120 Hz.
7. **`UpdateMusicStream` en cada frame**, o la música se corta (no es optimización, pero parece un bug de rendimiento).
8. **Compilar en Release** (`-O2`) para las pruebas de rendimiento y para la entrega. Debug puede ir ×5 más lento.
9. **Evitar `sqrtf` innecesarias:** comparar distancias al cuadrado (`Vector2LengthSqr`, `Vector2DistanceSqr`).
10. **Fondos estáticos** (gradas, cielo, líneas del campo): dibujarlos **una vez** a un `RenderTexture2D` y pintar esa textura en cada frame en lugar de cientos de primitivas.

### 13.2 Medir antes de optimizar

- `DrawFPS(10, 10)` en modo debug.
- Medir un bloque de código: `double t0 = GetTime(); ...; double ms = (GetTime() - t0) * 1000;`
- Profilers: `perf` y [Hotspot](https://github.com/KDAB/hotspot) en Linux, el Profiler de Visual Studio en Windows, [Tracy](https://github.com/wolfpld/tracy) (muy bueno y fácil de integrar).
- Detectar fugas y errores de memoria: compilar con `-fsanitize=address,undefined` en Linux. **Muy recomendable en la CI.**

### 13.3 Aviso para quien use WSL

Con WSLg, las ventanas grandes o en pantalla completa se quedan en ~35 FPS por la capa de composición, **no por nuestro código**. Para medir el rendimiento real hay que probar con la **compilación nativa de Windows**.

---

## 14. Idiomas (i18n)

Requisito: inglés y español como mínimo. Solución simple y suficiente:

```
assets/lang/es.txt          assets/lang/en.txt
menu.play=Jugar             menu.play=Play
menu.options=Opciones       menu.options=Options
match.goal=¡GOL!            match.goal=GOAL!
gameover.winner=Gana {0}    gameover.winner={0} wins
```

```cpp
class Localization {
public:
    bool load(const std::string& lang);                // lee "clave=valor" por líneas
    const std::string& tr(const std::string& key) const; // si falta la clave, devuelve la propia clave (se detecta en pantalla)
private:
    std::unordered_map<std::string, std::string> strings_;
};
```

- **Ningún texto visible escrito directamente en el código:** siempre `loc.tr("clave")`.
- Guardar el idioma elegido en `config.ini`. Por defecto se puede detectar el del sistema o usar inglés.
- Fuente con tildes y ñ (sección 7.2).
- **Test automático:** comprobar que `es.txt` y `en.txt` tienen **exactamente las mismas claves**. Es fácil y cuenta para los 10 tests.

---

## 15. Compilación sin Makefiles

El Hito 1 prohíbe **escribir Makefiles**. Opciones:

| Opción | Pros | Contras |
|---|---|---|
| **CMake + Ninja** (recomendado) | Estándar de la industria, Linux y Windows, descarga raylib sola (FetchContent), perfecto para la CI | Hay que aprender algo de CMake |
| Premake / xmake / Meson | Más sencillos | Menos documentación y ejemplos con raylib |
| Scripts (`build.sh` / `build.bat`) | Sin dependencias | Mantener dos scripts, compilación lenta, sin compilación incremental |

> ⚠️ **Confirmar con el profesor** que CMake está permitido (con Ninja no se genera ningún Makefile). Si no lo está, usar scripts.

**`CMakeLists.txt` de ejemplo:**

```cmake
cmake_minimum_required(VERSION 3.20)
project(HeadSoccer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(raylib)

# Lógica pura: la usan el juego y los tests
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS src/core/*.cpp src/engine/*.cpp)
add_library(game_core STATIC ${CORE_SRC})
target_include_directories(game_core PUBLIC src)
target_link_libraries(game_core PUBLIC raylib)   # raymath y tipos

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS src/game/*.cpp)
add_executable(headsoccer src/main.cpp ${GAME_SRC})
target_link_libraries(headsoccer PRIVATE game_core)
if (MSVC)
    target_compile_options(game_core PRIVATE /W4)
else()
    target_compile_options(game_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copiar assets junto al ejecutable
add_custom_command(TARGET headsoccer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:headsoccer>/assets)

# Tests
enable_testing()
file(GLOB TEST_SRC CONFIGURE_DEPENDS tests/*.cpp)
add_executable(tests ${TEST_SRC})
target_link_libraries(tests PRIVATE game_core)
add_test(NAME tests COMMAND tests)
```

**Compilar:**

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/headsoccer
ctest --test-dir build --output-on-failure
```

**Dependencias en Linux** (para compilar raylib):

```bash
sudo apt install build-essential cmake ninja-build git \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libasound2-dev
```

**Windows:** Visual Studio 2022 (con "Desarrollo para el escritorio con C++", que incluye CMake y Ninja) o MSYS2/MinGW. Con MSVC, añadir `set_target_properties(headsoccer PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)` para que no se abra la consola en la release.

---

## 16. Testing (≥ 10 tests)

**Librería recomendada:** [doctest](https://github.com/doctest/doctest) (un solo header, muy rápida de compilar) o [Catch2 v3](https://github.com/catchorg/Catch2) (con FetchContent). Justificar la elección en la documentación.

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "core/Match.hpp"

TEST_CASE("Un gol en la portería derecha suma un punto al jugador 1") {
    Match m;
    m.ball().pos = m.rightGoalCenter();
    m.update(1.0f / 120.0f);
    CHECK(m.score(0) == 1);
    CHECK(m.score(1) == 0);
}
```

**Ideas de tests** (todos de lógica, sin ventana, así que funcionan en la CI):

1. Un balón en caída libre acelera según la gravedad configurada.
2. Un balón que choca con el suelo rebota con la restitución esperada.
3. Un balón que golpea el larguero no entra en la portería.
4. Un balón que cruza la línea de gol suma un punto al equipo correcto.
5. Tras un gol, el balón y los jugadores vuelven a sus posiciones iniciales.
6. El partido termina al acabar el tiempo o al llegar al límite de goles.
7. Con empate al final del tiempo se activa el gol de oro (si lo implementamos).
8. Un jugador no puede salir de los límites del campo.
9. Un jugador no puede saltar en el aire (salvo con el power-up de doble salto).
10. El chute cargado al 100 % es más fuerte que al 0 %.
11. Un chute solo golpea el balón una vez (flag `hasHit`).
12. El parry se activa solo dentro de la ventana de tiempo.
13. La predicción de caída de la IA coincide con la simulación real (±tolerancia).
14. Los ficheros de idioma `es` y `en` tienen las mismas claves.
15. `Localization::tr` devuelve la clave si no existe la traducción.
16. `StateMachine`: push de Pausa conserva el Partido debajo; pop lo reanuda.
17. `StateMachine`: reemplazar un estado deja la pila con el mismo tamaño.
18. La física es determinista: dos simulaciones con la misma entrada dan el mismo estado final.
19. Con `invMass = 0` la cabeza no se mueve al chocar con el balón.
20. Las velocidades muy pequeñas se anulan (el balón acaba parado).

---

## 17. CI/CD con GitHub Actions y releases

**`.github/workflows/ci.yml`**: compila y ejecuta los tests en Linux y Windows en cada push y cada PR. Al crear un tag `v*`, publica la release con los ejecutables.

```yaml
name: CI
on:
  push:
    branches: [main, develop]
    tags: ['v*']
  pull_request:

jobs:
  build:
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, windows-latest]
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@v4

      - name: Dependencias Linux
        if: runner.os == 'Linux'
        run: |
          sudo apt-get update
          sudo apt-get install -y ninja-build libx11-dev libxrandr-dev libxinerama-dev \
            libxcursor-dev libxi-dev libgl1-mesa-dev libasound2-dev

      - uses: ilammy/msvc-dev-cmd@v1          # entorno MSVC para Ninja en Windows
        if: runner.os == 'Windows'

      - name: Configurar
        run: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
      - name: Compilar
        run: cmake --build build
      - name: Tests
        run: ctest --test-dir build --output-on-failure

      - name: Empaquetar
        shell: bash
        run: |
          mkdir -p dist/HeadSoccer
          cp -r assets dist/HeadSoccer/
          cp build/headsoccer* dist/HeadSoccer/ 2>/dev/null || true
          cd dist && 7z a ../HeadSoccer-${{ runner.os }}.zip HeadSoccer

      - uses: actions/upload-artifact@v4
        with:
          name: HeadSoccer-${{ runner.os }}
          path: HeadSoccer-${{ runner.os }}.zip

  release:
    if: startsWith(github.ref, 'refs/tags/v')
    needs: build
    runs-on: ubuntu-latest
    permissions:
      contents: write
    steps:
      - uses: actions/download-artifact@v4
        with: { merge-multiple: true }
      - uses: softprops/action-gh-release@v2
        with:
          files: HeadSoccer-*.zip
          generate_release_notes: true
```

**Publicar una versión:**

```bash
git checkout main && git pull
git tag -a v0.1.0 -m "Hito 1 - Alpha"
git push origin v0.1.0     # → la CI compila, testea y crea la release con los zips
```

Recomendable también: un job con `-fsanitize=address,undefined` y otro de formato con `clang-format --dry-run --Werror` (añadir un `.clang-format` al repo para que todos formateen igual).

---

## 18. Gestión del repositorio: ramas, bugtracking y versionado

### 18.1 Estrategia de ramas (propuesta: **Gitflow simplificado**)

```
main      ●───────────────●──────────────────●     ← solo versiones entregables (tags vX.Y.Z)
           \             / \                /
develop     ●──●──●──●──●───●──●──●──●──●──●      ← integración: siempre compila
               \  /  \   /     \    /
feature/*       ●●    ●●●       ●●●●              ← una rama por issue: feature/12-tiro-cargado
                                                   ← fix/34-balon-atraviesa-larguero
```

- `main`: protegida, solo recibe merges desde `develop` al cerrar un hito. Cada merge lleva un **tag SemVer**.
- `develop`: protegida, se entra por **Pull Request** con al menos 1 revisión de un compañero y la CI en verde.
- `feature/<n>-<desc>` y `fix/<n>-<desc>`: salen de `develop`, con el número de la issue en el nombre.
- `hotfix/<desc>`: sale de `main` si hay un bug crítico en una release entregada; se mergea en `main` y en `develop`.

**Justificación (para el documento del Hito 1):** somos 3–4 personas con entregas por hitos cerrados. Tener `main` = "lo entregado" y `develop` = "lo que estamos integrando" nos permite entregar siempre algo que compila (si no compila, es un 0) mientras seguimos desarrollando. Las ramas por issue enlazan el código con el bugtracking, y las PR obligan a revisar el código entre todos, lo que también sirve para demostrar la participación de cada miembro. No usamos Gitflow completo (ramas `release/*`) porque para un equipo pequeño es burocracia innecesaria. **GitHub Flow** (solo `main` + ramas) sería la alternativa más ligera; la descartamos porque no separa lo entregado de lo que está en desarrollo.

**Configurar en GitHub:** Settings → Branches → protection rules en `main` y `develop`: requerir PR, 1 aprobación y la CI en verde.

### 18.2 Commits

[Conventional Commits](https://www.conventionalcommits.org/es/): `feat: tiro cargado`, `fix: balón atraviesa larguero (#34)`, `docs:`, `test:`, `refactor:`, `ci:`. Escribir `Closes #34` en la PR cierra la issue sola al hacer merge.

### 18.3 Bugtracking: **GitHub Issues + GitHub Projects**

- **Issues:** cada tarea, bug o idea es una issue.
- **Etiquetas:** `bug`, `feature`, `docs`, `test`, `ci`, `prioridad:alta/media/baja`, `área:física/IA/gráficos/audio/motor`, `profesor` (para los tickets de revisión).
- **Milestones:** `Hito 1 - Alpha`, `Hito 2`, `Hito 3`, con la fecha de entrega. Se ve el progreso en %.
- **Project (tablero Kanban):** Backlog → To do → In progress → In review → Done, con automatizaciones al abrir o cerrar issues y PRs.
- **Plantillas** en `.github/ISSUE_TEMPLATE/`: `bug_report.md` (pasos para reproducir, comportamiento esperado y obtenido, SO, versión) y `feature_request.md`.

**Justificación:** está integrado en el mismo sitio que el código, las PR y la CI (sin herramientas externas ni cuentas extra), enlaza commits, ramas y PR con las issues automáticamente y es gratis. Además, el profesor enviará su revisión **como tickets**, así que tener las issues bien organizadas es parte de la evaluación. Alternativas descartadas: Jira (demasiado pesado para 4 personas y separado del repo) y Trello (no se enlaza con el código).

### 18.4 Versionado semántico (SemVer)

`MAJOR.MINOR.PATCH`. Mientras el juego no esté "terminado" usamos `0.x.y`:

| Versión | Significado |
|---|---|
| `v0.1.0` | Hito 1: Alpha |
| `v0.1.1` | Corrección de bugs sobre la Alpha |
| `v0.2.0` | Hito 2 |
| `v1.0.0` | Entrega final |

Mantener un `CHANGELOG.md` ([Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/)). Mostrar la versión en el menú (con `project(... VERSION)` en CMake y `configure_file` para generar un `Version.hpp`).

---

## 19. Requisitos secundarios (+2 puntos)

| Requisito | Encaje en Head Soccer | Coste | Recomendación |
|---|---|---|---|
| **Multijugador local** | Natural: 1 contra 1 en el mismo PC | 🟢 | ✅ Sí, desde el Hito 1 |
| **Mandos** | Ideal para 1 contra 1 (evita el ghosting) | 🟢 | ✅ Sí |
| **IA** (árboles de decisión, steering) | Necesaria para el modo de un jugador | 🟡 | ✅ Sí |
| **Interfaz de debug (ImGui)** | Ajustar la física en vivo (gravedad, restitución...), ver hitboxes, estado de la IA | 🟡 | ✅ Muy útil para desarrollar: [rlImGui](https://github.com/raylib-extras/rlImGui) |
| **Logging (spdlog)** | Registrar goles, estados y errores al cargar recursos | 🟢 | ✅ Fácil con FetchContent: [spdlog](https://github.com/gabime/spdlog) |
| **ECS (EnTT)** | Posible (partículas, power-ups), pero hay pocas entidades | 🔴 | ⚠️ Solo si alguien quiere aprenderlo; complica la arquitectura |
| **Box2D** | Posible, pero perdemos control del feel arcade | 🔴 | ⚠️ Opcional (ver 8.2) |
| **Funcionalidades innovadoras** | Estadios con reglas, torneo roguelite, repeticiones | 🟡 | ✅ Es nuestra "mejora" |

Cada librería extra se documenta: **qué es, por qué la usamos y cómo se integra** (requisito del Hito 1).

---

## 20. Reparto de roles y hoja de ruta

### 20.1 Roles (4 personas; con 3, se fusionan Audio/UI y Gráficos)

| Rol | Responsabilidades |
|---|---|
| **Motor / infraestructura** | CMake, CI, releases, `StateMachine`, `ResourceManager`, i18n, input y mandos |
| **Física y mecánicas** | Balón, colisiones, chute, parry, efecto, reglas del partido, tests de física |
| **IA y modos de juego** | IA del rival, dificultad, torneo, power-ups, estadios |
| **Gráficos, audio y UI** | Render procedural, menús, HUD, partículas, juice, sonido, música, créditos |

Todos: tests de su parte, revisar las PR de los demás y documentar aquí.

### 20.2 Hoja de ruta del Hito 1 (hasta el 28/10/2026)

| Semana | Objetivo |
|---|---|
| 1 | Arreglar la plantilla (sección 5), CMake y CI en verde en Linux y Windows, ramas protegidas, etiquetas y tablero. GDD aprobado por el equipo |
| 2 | Campo, porterías, jugadores que se mueven y saltan, balón con física y rebotes |
| 3 | Chute y goles, marcador, tiempo, reinicio tras gol, pantalla de Game Over con reinicio. IA básica (seguir el balón) |
| 4 | Tiro cargado + parry, pulido (juice básico), ≥ 10 tests, documentación (GDD, ramas, bugtracking), release `v0.1.0` |

### 20.3 Después del Hito 1 (orientativo)

- **Hito 2:** sprites + gestor de recursos, menús completos, i18n, audio, mandos, IA con dificultad, estadios.
- **Hito 3:** torneo roguelite, repeticiones, power-ups, ImGui y spdlog, pulido y balance.

---

## 21. Recursos y referencias

### raylib
- Chuleta: <https://www.raylib.com/cheatsheet/cheatsheet.html>
- Ejemplos: <https://www.raylib.com/examples.html>. Muy útiles: `core_2d_camera`, `core_window_letterbox` (resolución virtual), `textures_sprite_anim`, `shapes_easings_*`, `audio_sound_multi`, `core_input_gamepad`.
- Wiki: <https://github.com/raysan5/raylib/wiki>
- Discord oficial de raylib (muy activo para dudas).
- raylib-extras (rlImGui, plantillas): <https://github.com/raylib-extras>

### Programación de videojuegos
- *Game Programming Patterns* (Robert Nystrom), gratis online: <https://gameprogrammingpatterns.com>. Capítulos: **State**, **Game Loop**, **Update Method**, **Object Pool**, **Command** (input/IA/replays), **Singleton** (y por qué evitarlo).
- *Fix Your Timestep!*: <https://gafferongames.com/post/fix_your_timestep/>
- Física de impulsos 2D (Randy Gaul): <https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t>
- Easings: <https://easings.net>
- *Real-Time Collision Detection* (Christer Ericson), la referencia para colisiones.
- Red Blob Games (matemáticas para juegos, muy visual): <https://www.redblobgames.com>

### Game feel
- *Juice it or lose it*: <https://www.youtube.com/watch?v=Fy0aCDmgnxg>
- *The art of screenshake*: <https://www.youtube.com/watch?v=AJdEqssNZ-U>
- *Math for Game Programmers: Juicing Your Cameras With Math* (GDC, Squirrel Eiserloh): trauma y screen shake con ruido.

### Assets
Ver las secciones 9.3 y 9.4. **Registrar todo en `CREDITS.md`.**

---

## Registro de decisiones

| Fecha | Decisión | Motivo |
|---|---|---|
| _pendiente_ | Concepto final del juego (sección 3) | |
| _pendiente_ | Sistema de compilación (CMake vs scripts) | Confirmar con el profesor |
| _pendiente_ | Física propia vs Box2D | |
| _pendiente_ | Librería de tests (doctest vs Catch2) | |
