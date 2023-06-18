#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>

#define Y1 4
#define X1 5
#define Y2 (Y1 + 7)
#define X2 (X1 + 7 * 2)

#define Y_MID (Y1 + 3)
#define X_MID (X1 + 3 * 2)


/* ------ SCREEN ------ */

void init_screen(void) {
    initscr() ;
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    clear() ;
    noecho() ;
    cbreak() ;
    keypad(stdscr, TRUE) ;
    curs_set(2) ;
    
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
}

void cleanup_screen(void) {
    attrset(0);
    endwin();
}

/* ------ BOARD ------ */

void rectangle(int y1, int x1, int y2, int x2) {
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void init_board(char board[8][8]) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        board[i][j] = '.';
      }
    }
    board[3][3] = 'w';
    board[4][4] = 'w';
    board[3][4] = 'b';
    board[4][3] = 'b';
}

void print_board(char board[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == '.') {
                mvaddch(Y1 + i, X1 + j * 2, '.');
            } else if (board[i][j] == 'b') {
                attron(COLOR_PAIR(1));
                mvaddch(Y1 + i, X1 + j * 2, 'b');
                attroff(COLOR_PAIR(1));
            } else {
                mvaddch(Y1 + i, X1 + j * 2, 'w');
            }
        }
    }
    rectangle(Y1 - 1, X1 - 2, Y2 + 1, X2 + 2);
}

/* ------ GAME ------ */

int is_legal_move(int y, int x, char ch, char board[8][8]) {
    if (board[y - Y1][(x - X1) / 2] != '.')
        return 0;

    // Check if any adjacent opponent's disc can be flipped
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -2; dx <= 2; dx += 2) {
            if (dy == 0 && dx == 0)
                continue;

            int cy = y + dy;
            int cx = x + dx;
            int flipped = 0;

            while (cy >= Y1 && cy <= Y2 && cx >= X1 && cx <= X2 && board[cy - Y1][(cx - X1) / 2] != '.') {
                if (board[cy - Y1][(cx - X1) / 2] != ch)
                    flipped = 1;
                if (board[cy - Y1][(cx - X1) / 2] == ch)
                    break;
                cy += dy;
                cx += dx;
            }
            if (flipped == 1) return 1;
        }
    }
    return 0;
}

void make_move(int y, int x, char ch, char board[8][8]) {
    board[y - Y1][(x - X1) / 2] = ch;

    // Flip opponent's discs in all directions
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -2; dx <= 2; dx += 2) {
            if (dy == 0 && dx == 0)
                continue;

            int cy = y + dy;
            int cx = x + dx;
            int flipped = 0;

            while (cy >= Y1 && cy <= Y2 && cx >= X1 && cx <= X2 && board[cy - Y1][(cx - X1) / 2] != '.') {
                if (board[cy - Y1][(cx - X1) / 2] == ch) {
                    flipped = 1;
                    break;
                }
                cy += dy;
                cx += dx;
            }
            if (flipped) {
                cy -= dy;
                cx -= dx;

                while (cy != y || cx != x) {
                    board[cy - Y1][(cx - X1) / 2] = ch;
                    cy -= dy;
                    cx -= dx;
                }
            }
        }
    }
}

void receive_move(int conn_fd, char board[8][8]) {
    ssize_t s;
    
    if ((s = recv(conn_fd, board, 64, MSG_WAITALL)) == -1) {
        perror("Failed to receive data");
        exit(1);
    }
    if (s != 64) {
        perror("Received data does not match expected size");
    }
    print_board(board);
}

void get_input(char whoAmI, char board[8][8]) {
    int x = X_MID;
    int y = Y_MID;
    int c;
    int finished = 0;
    
    while (!finished && (c = getch())) {
        switch (c) {
            case KEY_UP:
                if (y > Y1) y--;
                break;
            case KEY_DOWN:
                if (y < Y2) y++;
                break;
            case KEY_LEFT:
                if (x > X1) x -= 2;
                break;
            case KEY_RIGHT:
                if (x < X2) x += 2;
                break;
            case ' ':
                if (is_legal_move(y, x, whoAmI, board)) {
                    make_move(y, x, whoAmI, board);
                    print_board(board);
                    finished = 1;
                } else {
                    move(Y2 + 3, X1 - 2);
                    clrtoeol();
                    printw("Illegal move!");
                }
                break;
            case 'q':
                cleanup_screen();
                exit(1);
            default:
                move(Y2 + 3, X1 - 2);
                clrtoeol();
                printw("Invalid input!");
                break ;
        }
        move(y, x) ;
        refresh() ;
    }
}

/* ------ PLAY ------ */

void play(int conn_fd, char whoAmI) {
    // setup game
    init_screen();
    
    move(Y1 - 3, X1 - 2);
    printw("      REVERSI      ");
    
    move(Y1, X2 + 5);
    
    attron(COLOR_PAIR(1));
    printw("Client's turn!");
    attroff(COLOR_PAIR(1));
    
    move(Y1 + 2, X2 + 5);
    printw("press <space> to place disk");
    move(Y1 + 4, X2 + 5);
    printw("press <q> to quit");

    char board[8][8];
    init_board(board);
    print_board(board);
    
    move(Y_MID, X_MID);
    refresh();
    
    // play as server
    if (whoAmI == 'w') {
        do {
            // receive client move
            receive_move(conn_fd, board);
            
            move(Y1, X2 + 5);
            clrtoeol();
            
            // attron(COLOR_PAIR(2));
            printw("Server's turn!");
            // attroff(COLOR_PAIR(2));
            
            move(Y_MID, X_MID);
            refresh();
            
            // send server move
            get_input(whoAmI, board);
            send(conn_fd, board, sizeof(board), 0);
            
            move(Y2 + 3, X1 - 2);
            clrtoeol();
            move(Y1, X2 + 5);
            clrtoeol();
            
            attron(COLOR_PAIR(1));
            printw("Client's turn!");
            attroff(COLOR_PAIR(1));
            
            move(Y_MID, X_MID);
            refresh();
            
        } while (1);
    }
    // play as client
    else {
        do {
            // send client move
            get_input(whoAmI, board);
            send(conn_fd, board, sizeof(board), 0);
            
            move(Y2 + 3, X1 - 2);
            clrtoeol();
            move(Y1, X2 + 5);
            clrtoeol();
            
            // attron(COLOR_PAIR(2));
            printw("Server's turn!");
            // attroff(COLOR_PAIR(2));
            
            move(Y_MID, X_MID);
            refresh();
            
            // receive server move
            receive_move(conn_fd, board);
            
            move(Y1, X2 + 5);
            clrtoeol();
            
            attron(COLOR_PAIR(1));
            printw("Client's turn!");
            attroff(COLOR_PAIR(1));
            
            move(Y_MID, X_MID);
            refresh();
            
        } while (1);
    }
    
    // cleanup
    cleanup_screen();
}
