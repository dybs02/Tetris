#include <stdlib.h>
#include <time.h>
#include "primlib.h"
#include "pieces.inl"


#define HEIGHT 20
#define WIDTH 10
#define TOP_Y 100
#define SIZE ((gfx_screenHeight() - TOP_Y) / HEIGHT)
#define LEFT_X ((gfx_screenWidth() / 2) - (WIDTH * SIZE / 2))
#define NEXT_LEFT_X ((LEFT_X / 2) - SIZE * 4)
#define NEXT_TOP_Y ((gfx_screenHeight() / 2) - SIZE * 2)
#define DELAY 10
#define FALL_DELAY 500
#define CONTROLS_DELAY 10

typedef struct Game_Piece {
    int top_x;
    int top_y;
    int mid_x;
    int mid_y;
    int kind;
    int rotation;
} Game_Piece;

Game_Piece falling_piece;
Game_Piece next_piece;
int board[WIDTH][HEIGHT] = {0};
int piece_fall_delay = 0;

void set_mid_of_piece(Game_Piece *piece) {
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];

            if (block_val == 2) {
                piece->mid_x = x_index;
                piece->mid_y = y_index;
                return;
            }
        }
    }
}

void set_rand_piece(Game_Piece *new_piece) {
    new_piece->top_x = (WIDTH / 2) - 2;
    new_piece->top_y = 0;
    new_piece->kind = rand() % 7;
    new_piece->rotation = 0;
    set_mid_of_piece(new_piece);
}

void game_init() {
    srand(time(NULL));

    set_rand_piece(&falling_piece);
    set_rand_piece(&next_piece);
}

void set_initial_pos(Game_Piece* piece) {
    int initial_x = WIDTH / 2;
    int initial_y = 0;

    piece->top_x = initial_x - piece->mid_x;
    piece->top_y = initial_y - piece->mid_y;
}

void blit_falling_to_board() {
    Game_Piece *piece = &falling_piece;
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];
            if (block_val) {
                board[piece->top_x + x_index][piece->top_y + y_index] = 1;
            }
        }
    }
    falling_piece = next_piece;
    set_rand_piece(&next_piece);
    set_initial_pos(&falling_piece);
}

int check_piece_reached_bottom(Game_Piece *piece) {
    for (int y_index = 3; y_index >= 0; --y_index) {
        for (int x_index = 0; x_index < 4; ++x_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];

            if (block_val && (piece->top_y + y_index >= HEIGHT - 1 || piece->top_x + x_index >= WIDTH)) {
                return 1;
            }
            if (block_val && board[piece->top_x + x_index][piece->top_y + y_index + 1]) {
                return 1;
            }
        }
    }
    return 0;
}

void draw_falling_piece() {
    Game_Piece *piece = &falling_piece;
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];
            if (block_val) {
                gfx_filledRect(
                        LEFT_X + (SIZE * (piece->top_x + x_index)),
                        TOP_Y + (SIZE * (piece->top_y + y_index)),
                        LEFT_X + (SIZE * (piece->top_x + x_index + 1)),
                        TOP_Y + (SIZE * (piece->top_y + y_index + 1)),
                        block_val == 2 ? GREEN : YELLOW);
            }
        }
    }
}

void draw_next_piece() {
    Game_Piece *piece = &next_piece;
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];
            if (block_val) {
                gfx_filledRect(
                        NEXT_LEFT_X + (SIZE * (piece->top_x + x_index)),
                        NEXT_TOP_Y + (SIZE * (piece->top_y + y_index)),
                        NEXT_LEFT_X + (SIZE * (piece->top_x + x_index + 1)),
                        NEXT_TOP_Y + (SIZE * (piece->top_y + y_index + 1)),
                        YELLOW);
            }
        }
    }
}

void draw_board() {
    gfx_rect(
            LEFT_X,
            TOP_Y,
            LEFT_X + (WIDTH * SIZE),
            TOP_Y + (HEIGHT * SIZE),
            WHITE);
}

void draw_blocks() {
    for (int x_index = 0; x_index < WIDTH; ++x_index) {
        for (int y_index = 0; y_index < HEIGHT; ++y_index) {
            if (board[x_index][y_index]) {
                gfx_filledRect(
                LEFT_X + (SIZE * x_index),
                TOP_Y + (SIZE * y_index),
                LEFT_X + (SIZE * (x_index + 1)),
                TOP_Y + (SIZE * (y_index + 1)),
                YELLOW);
            }
        }
    }
}

void draw_all() {
    gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
    draw_next_piece();
    draw_board();
    draw_blocks();
    draw_falling_piece(&falling_piece);
    gfx_updateScreen();
    SDL_Delay(DELAY);
}

void draw_lost_screen() {
    gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
    draw_next_piece();
    draw_board();
    draw_blocks();
    draw_falling_piece(&falling_piece);
    gfx_textout((gfx_screenWidth()/2) - 10, 50, "Game Lost !", RED);
    gfx_updateScreen();
    SDL_Delay(DELAY);
}

void move_down() {
    piece_fall_delay += DELAY;

    if (piece_fall_delay % FALL_DELAY < DELAY) {
        piece_fall_delay = 0;
        falling_piece.top_y += 1;
    }

    if (check_piece_reached_bottom(&falling_piece)) {
        blit_falling_to_board();
    }
}

int move_left() {
    Game_Piece *piece = &falling_piece;
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];

            if (block_val && piece->top_x + x_index == 0) {
                return 0;
            }else if (block_val) {
                piece->top_x -= 1;
                return 1;
            }

        }
    }
    return 0;
}

int move_right() {
    Game_Piece *piece = &falling_piece;
    for (int x_index = 3; x_index >= 0; --x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];

            if (block_val && piece->top_x + x_index == WIDTH - 1) {
                return 0;
            }else if (block_val) {
                piece->top_x += 1;
                return 1;
            }

        }
    }
    return 0;
}

int rotate() {
    Game_Piece rotated_piece;
    rotated_piece = falling_piece;
    rotated_piece.rotation = (rotated_piece.rotation + 1) % 4;

    set_mid_of_piece(&rotated_piece);
    int x_change = falling_piece.mid_x - rotated_piece.mid_x;
    int y_change = falling_piece.mid_y - rotated_piece.mid_y;
    rotated_piece.top_x += x_change;
    rotated_piece.top_y += y_change;

    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[rotated_piece.kind][rotated_piece.rotation][x_index][y_index];

            if (block_val) {
                if (rotated_piece.top_x + x_index < 0 || rotated_piece.top_x + x_index > WIDTH - 1) {
                    return 0;
                }
                if (check_piece_reached_bottom(&rotated_piece)) {
                    return 0;
                }
            }
        }
    }

    falling_piece = rotated_piece;
    return 1;
}

void drop() {
    while (!check_piece_reached_bottom(&falling_piece)) {
        falling_piece.top_y += 1;
    }
    blit_falling_to_board();
}

void check_controls() {

    static int space_pressed = 0;
    if (gfx_isKeyDown(SDLK_SPACE) && !space_pressed) {
        space_pressed = 1;
        rotate();
    }else if (!gfx_isKeyDown(SDLK_SPACE)) {
        space_pressed = 0;
    }

    static int down_pressed = 0;
    if (gfx_isKeyDown(SDLK_DOWN) && !down_pressed) {
        down_pressed = 1;
        drop();
    }else if (!gfx_isKeyDown(SDLK_DOWN)) {
        down_pressed = 0;
    }

    static int left_pressed_delay = CONTROLS_DELAY;
    if (gfx_isKeyDown(SDLK_LEFT)) {
        left_pressed_delay -= 1;
        if (left_pressed_delay == 0) {
            move_left();
            left_pressed_delay = CONTROLS_DELAY;
        }
    }

    static int right_pressed_delay = CONTROLS_DELAY;
    if (gfx_isKeyDown(SDLK_RIGHT)) {
        right_pressed_delay -= 1;
        if (right_pressed_delay == 0) {
            move_right();
            right_pressed_delay = CONTROLS_DELAY;
        }
    }
}

void check_rows() {
    for (int y_index = HEIGHT-1; y_index > 0; --y_index) {

        int row_sum = 0;
        for (int x_index = 0; x_index < WIDTH; ++x_index) {
            row_sum += board[x_index][y_index];
        }

        if (row_sum == WIDTH) {
            for (int row = y_index; row > 0; --row) {
                for (int column = 0; column < WIDTH; ++column) {
                    board[column][row] = board[column][row - 1];
                }

            }
            check_rows();
        }
    }
}

int is_game_lost() {
    Game_Piece* piece = &falling_piece;
    for (int x_index = 0; x_index < 4; ++x_index) {
        for (int y_index = 0; y_index < 4; ++y_index) {
            int block_val = pieces[piece->kind][piece->rotation][x_index][y_index];

            if (block_val && piece->top_y + y_index >= 0 && board[piece->top_x + x_index][piece->top_y + y_index]) {
                return 1;
            }
        }
    }
    return 0;
}

void game_loop() {
    while (!is_game_lost()) {

        move_down();
        check_controls();
        check_rows();
        draw_all();

        if (gfx_isKeyDown(SDLK_ESCAPE)) break;
    }

    draw_lost_screen();
    gfx_getkey();
}
