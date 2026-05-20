# ¿Alguna vez pensaste que morirías por amor?
### Dungeon Crawler — Parcial Final · C++11

---

## Integrantes
*(Completa aquí con los nombres del grupo)*

---

## Descripción
Juego de tipo dungeon-crawler 2D en consola inspirado en *Adventure* (Atari 2600, 1980).  
Controlas a **Eli**, quien debe recorrer 7 habitaciones, recoger 5 objetos narrativos y
esconderse en el armario final antes de ser atrapada por el **Novio abusivo** (y luego también
por su propia **Conciencia**).

---

## Historia

| Pantalla | Texto |
|---|---|
| Intro 1 | *"¿Alguna vez pensaste que morirías por amor?"* |
| Intro 2 | *"Pues Eli nunca llegó a pensar que un día el amor podría quitarle su vida."* |
| ¡Corre! | El juego comienza. El novio ya patrulla. |
| Celular  | Al leer el celular aparece la Conciencia como segundo enemigo. |
| Victoria | *"No, el amor no duele y no debería doler nunca."* |
| Derrota  | *"Has muerto. Y no de amor."* |

---

## Controles

| Tecla | Acción |
|---|---|
| W / A / S / D | Mover a Eli |
| E | Recoger objeto cercano |
| R | Leer / usar objeto del inventario |
| Q | Soltar objeto |

---

## Objetos coleccionables (5 en total)

| Símbolo | Objeto | Habitación |
|---|---|---|
| `?` | Carta del novio | ROOM_2 (media-izquierda) |
| `k` | Llave | ROOM_6 (media-derecha) |
| `i` | Vela | ROOM_2 |
| `p` | Celular | ROOM_4 (baño) |
| `!` | Carta final | ROOM_7 (final) |

---

## Leyenda del mapa

| Símbolo | Significado |
|---|---|
| `@` | Eli (jugadora) |
| `H` | Eli escondida en armario |
| `B` | Novio abusivo (enemigo 1) |
| `V` | Conciencia (enemigo 2, aparece tras leer el celular) |
| `#` | Pared |
| `.` | Suelo |
| `+` | Puerta |
| `C` | Armario |

---

## Compilación

### Con CMake (recomendado)
```bash
# Linux / macOS
mkdir build && cd build
cmake ..
make
./amor_game

# Windows (MinGW)
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
amor_game.exe
```

### Con Make
```bash
make
./amor_game
```

### Manual (g++)
```bash
g++ -std=c++11 -Wall -I. main.cpp src/*.cpp -o amor_game
./amor_game
```

---

## Estructura del repositorio

```
amor_game/
├── main.cpp              # Punto de entrada y bucle principal
├── CMakeLists.txt        # Configuración CMake
├── Makefile              # Makefile alternativo
├── README.md
├── include/
│   ├── types.h           # Constantes y structs base (Vec2, fases, IDs)
│   ├── map.h             # Módulo de mapa
│   ├── player.h          # Módulo del jugador
│   ├── enemy.h           # Módulo de enemigos
│   ├── items.h           # Módulo de ítems coleccionables
│   ├── input.h           # Módulo de entrada (cross-platform)
│   ├── renderer.h        # Módulo de renderizado ASCII
│   └── game.h            # Estado global y lógica de juego
└── src/
    ├── map.cpp
    ├── player.cpp
    ├── enemy.cpp
    ├── items.cpp
    ├── input.cpp
    ├── renderer.cpp
    └── game.cpp
```

---

## Requisitos cumplidos

- [x] **Lenguaje:** C++11 obligatorio
- [x] **Jugabilidad:** Personaje controlable, 7 habitaciones, colisiones funcionales
- [x] **Entidades:** 2 tipos de enemigos (Novio con persecución + Conciencia)
- [x] **Mapa:** Renderizado ASCII con jugador, enemigos e ítems
- [x] **Interacción:** Inventario de un solo espacio (recoger/soltar/leer), condiciones de victoria y derrota
- [x] **Arquitectura:** Múltiples archivos .h y .cpp, punteros en la lógica (Item*, Enemy*, Player*)
- [x] **Automatización:** CMake + Makefile
- [x] **Memoria:** Arreglos estáticos; **prohibido `new`/`delete` en game-loop** ✓

---

## Innovaciones
*(Completa aquí si agregas funcionalidades extra)*
