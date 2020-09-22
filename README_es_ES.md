# CHEATERex
Fork de [sm64pc/sm64ex](https://github.com/sm64pc/sm64ex) con parches adicionales incluidos
Para poder ser usado como plantilla por modders, hackers y por gente tramposa.

**no se subirá ningún archivo que tenga derechos de autor**. 
Ejecuta `./extract_assets.py --clean && make clean` o `make distclean` para eliminar contenido originado del ROM.

## Trucos adicionales
  * Muerte Instantanea/Salir del Nivel Gatillo L + Gatillo R + Boton A + Boton B
  * Mantenga presionado Boton A para flotar
  * Gravedad de la Luna
  * Modificador de velocidad corriendo
    -Lento x.75
    -Más lento x.5
    -Rápido x2
    -Más rápido ~ x3
  * Sin barrera de muerte
  * Todos los saltos más altos
  * Mostrar la velocidad, portado por ferris the crab(io12)
  * ¿Levitación en T-pose?
  * Tocadiscos con seleccion de musica
  * Obtener el final de manera rápida
    - mientras esté activo, si se obtienen 120 estrellas, la cinemática del final será mostrada
  * Herir a Mario = Gatillo L + Botón A
    - Quemadura
    - Choque eléctrico
    - Fijar la energía de Mario a uno HP0
  * Usar el cañón donde sea = Gatillo L + Botón C arriba
    - Spawnea un cañón para ser usado por el jugador
    *nota: Si se usa en aguas profundas, el jugador será disparado a la superficie 
  * Salto de Pared automatico
    - mientras este activo, el jugador hará un Salto de Pared automaticamente con solo tocar una pared
    - Manten el boton A para aumentar la altura
    *nota: Chocaras con la pared si no es posible hacer un Salto de Pared
  * Aparecer un caparazón v2 = Gatillo L + Gatillo R
    - si estas bajo el agua, no aparecerá el caparazón 
  * Aparecer una Bob-omba = Gatillo L + Botón B
  * Spamba v2 (Spawnea actores common0)= Gatillo L + Gatillo Z
  * Nado Rápido v3
      - La velocidad máxima bajo el agua ha sido duplicada, ahora con burbujas
  * Trucos de las gorras por JAGSTAX
    - QUITARTE la gorra y QUITAR la música de las gorras incluidos
  * Port de GateGuy del patch de Kaze Emanuar para hacer BLJ en donde sea
  * Truco para jugar como algún personaje (WIP), muchas gracias a ferris the crab(io12)
    - Bob-omba enemiga
    - Bob-omba aliada
    - Goomba
    - Amp
    - Chuckya
    - FlyGuy

## Nuevas funciones
 * X, Y, y el D PAD fueron agregados (revisa `sm64.h` para los defines)
 * 60 FPS por Emil incluido
 * "Leave Ending Screen" por GateGuy incluido
 * "Exit to Main Menu" por Adya incluido
 * "50 Coin 1UP" por Keanine incluido
 * "Odyssey Moveset" por PeachyPeach
    -"Tighter Controls" por Keanine incluido
    -"Go to Level Select" por $4Y$ incluido
 * "Time Trials" por GateGuy y PechyPeach incluido
 * "Stay In Level After Star v2" por GateGuy
 * "Menu C button support" por engima907
 * "Bugfixes" por JAGSTAX
 * "DynOS" por PeachyPeach
 * Y por supuesto, "CHEATERv7" por $4Y$, yo mismo

## Como agregar los trucos/mods
 * Usa `src/game/mario_cheats.c` para el código
   - todos y cada uno de los archivos `actions` tienen una función de solo una linea
   - Úsalo con `mario_cheats.h` para agregar nuevas funcionalidades
 * Usa `src/game/cheats_menu.h` para las opciones del menú
   - Agregua `TEXT_OPT_<NOMBRE>` a `optsCheatsStr`
   - Si estás haciendo una lista, necesitas crear un array en  `TEXT_OPT`
     al igual que uno para los strings (ve el archivo si necesitas un ejemplo)
 * Usa `include/text_cheats_strings.h.in` para decirle al juego que imprimir
   - Ejemplo: `#define TEXT_OPT_HOLA _("Hola")`
   - debes hacerlo para las versiones US y JP
   - JP solo usa mayúsculas
   - Ejemplos de botones `_("[A]")` o `_("[C]<")` aunque no se puede el gatillo L 
 * Usa `src/game/options_menu.c` para agregarlo a `static struct Option optCheats`
 * Al agregar nuevos archivos, el `Makefile` probablemente necesita ser modificado
   - La información Externa localizada en `build/res/base.zip` es manejada por los archivos `Makefile` y `Makefile.split`
 * [tutorial](https://youtu.be/VIB2qKiR8fI)

# siéntete libre de preguntar o mandar una solicitud 
