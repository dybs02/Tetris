#ifndef __GAME_H__
#define __GAME_H__

#include "primlib.h"
#include <stdlib.h>

typedef struct Game_Piece {
    int top_x;
    int top_y;
    int mid_x;
    int mid_y;
    int kind;
    int rotation;
} Game_Piece;

void set_mid_of_piece(Game_Piece*);
void set_rand_piece(Game_Piece*);
void game_init();
void set_initial_pos(Game_Piece*);
void blit_falling_to_board();
int check_piece_reached_bottom(Game_Piece*);
void draw_falling_piece();
void draw_next_piece();
void draw_board();
void draw_blocks();
void draw_all();
void draw_lost_screen();
void move_down();
int move_left();
int move_right();
int rotate();
void drop();
void check_controls();
void check_rows();
int is_game_lost();
void game_loop();

#endif /* __GAME_H__ */
