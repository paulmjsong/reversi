#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>

#define Y1 5
#define X1 5
#define Y2 (Y1 + 7)
#define X2 (X1 + 7 * 2)

#define Y_MID (Y1 + 3)
#define X_MID (X1 + 3 * 2)


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
}

void cleanup_screen(void) {
    endwin();
}

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

void print_board(char board[8][8]) {
//    start_color();
//    init_pair(1, COLOR_BLUE, COLOR_BLACK);
//    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    
    for (int i = Y1; i < Y1 + 8; i++) {
        for (int j = X1; j < X1 + 8 * 2; j+=2) {
            if (board[i][j] == '.') {
                mvaddch(i, j, '.');
            } else if (board[i][j] == 'b') {
//                attron(COLOR_PAIR(1));
                mvaddch(i, j, 'b');
            } else {
//                attron(COLOR_PAIR(2));
                mvaddch(i, j, 'w');
            }
        }
    }
//    attroff(COLOR_PAIR(1));
//    attroff(COLOR_PAIR(2));
    
    rectangle(Y1 - 1, X1 - 2, Y2 + 1, X2 + 2);
}

int flip(int y, int x, char ch, char board[8][8]) {
    if (board[y][x] == '.') return 0;
    if (board[y][x] == ch) return 0;
    
}

int is_legal_move(int y, int x, char ch, char board[8][8]) {
    y = (y - Y1);
    x = (x - X1) / 2;
    
    // check if position is occupied
    if (board[y][x] != '.')
        return 0;
    // check if move flips other disks
    int flipped = 0;
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (i == 0 && j == 0)
                continue;
            if (flip(i, j, ch, board)) {
                board[y][x] = ch;
                flipped = 1;
            }
        }
    }
    if (flipped)
        return 1;
    return 0;
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

void make_move(char whoAmI, char board[8][8]) {
    int x = X_MID;
    int y = Y_MID;
    int c;
    int finished = 0;
    
    while (!finished && (c = getch())) {
        switch (c) {
            case KEY_UP:
                if (y > Y1 + 1) y--;
                break;
            case KEY_DOWN:
                if (y < Y2 - 1) y++;
                break;
            case KEY_LEFT:
                if (x > X1 + 2) x -= 2;
                break;
            case KEY_RIGHT:
                if (x < X2 - 2) x += 2;
                break;
            case ' ':
                if (is_legal_move(y, x, whoAmI, board)) {
                    mvaddch(y, x, whoAmI);
                    finished = 1;
                } else {
                    move(Y2 + 3, X1 - 2);
                    clrtoeol();
                    printw("Illegal move!");
                }
                break;
            case 'q':
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

void play(int conn_fd, char whoAmI) {
    // setup game
    init_screen();
    
    move(Y1 - 3, X1 - 2);
    printw("REVERSI");
    move(Y2 + 3, X1 - 2);
    printw("REVERSI");

    char board[8][8] = {'.'};
    board[3][3] = 'w';
    board[4][4] = 'w';
    board[3][4] = 'b';
    board[4][3] = 'b';
    
    print_board(board);
    
    move(Y_MID, X_MID);
    refresh();
    
    // play as server
    if (whoAmI == 'w') {
        do {
            // receive client move
            receive_move(conn_fd, board);
            refresh();
            
            // send server move
            make_move(whoAmI, board);
            send(conn_fd, board, sizeof(board), 0);
            
        } while (1);
    }
    // play as client
    else {
        do {
            // send client move
            make_move(whoAmI, board);
            send(conn_fd, board, sizeof(board), 0);
            
            // receive server move
            receive_move(conn_fd, board);
            refresh();
            
        } while (1);
    }
    
    // cleanup
    cleanup_screen();
}
