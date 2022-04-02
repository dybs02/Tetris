#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include "game.h"

int main(int argc, char *argv[]) {
    if (gfx_init())
        exit(3);

    game_init();

    game_loop();


    return 0;
}
