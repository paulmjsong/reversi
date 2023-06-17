#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>

void rectangle(int y1, int x1, int y2, int x2)
{
	mvhline(y1, x1, 0, x2-x1);
	mvhline(y2, x1, 0, x2-x1);
	mvvline(y1, x1, 0, y2-y1);
	mvvline(y1, x2, 0, y2-y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
}

void drawInitialStone(int y, int x){
	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);

	attron(COLOR_PAIR(1));
	mvprintw(y, x,"b");
	
	attron(COLOR_PAIR(2));
	mvprintw(y, x + 2,"w");

	attron(COLOR_PAIR(1));
	mvprintw(y + 1, x + 2,"b");

	attron(COLOR_PAIR(2));
	mvprintw(y + 1, x,"w");

	attroff(COLOR_PAIR(2));	
}

void serverDrawBoard(pthread_mutex_t *lock, int conn_fd){
        initscr() ;
        clear() ;

        noecho() ;
        cbreak() ;
        keypad(stdscr, TRUE) ;

        int row, col ;
        getmaxyx(stdscr, row, col) ;

        curs_set(2) ;
				
        int y1 = row / 3 - 5 ;	
        int x1 = col / 3 - 12 ;
        int y2 = row / 3 + 4 ;
        int x2 = col / 3 + 6 ;

	if((y1 % 2 != 0) && (x1 % 2 == 0) && (y2 % 2 == 0) && (x2 % 2 == 0)){
		y1++;
		x1++;
		y2++;
		x2++;
	}

	int rate_y = (row / 2) - (row / 3) ;
	int rate_x = (col / 2) - (col / 3) ;

	if(rate_x % 2 == 0) rate_x++;

	y1 = y1 + rate_y ;
	x1 = x1 + rate_x ;
	y2 = y2 + rate_y ;
	x2 = x2 + rate_x ;

        rectangle(y1, x1, y2, x2) ;
	int h = y2 - y1 + rate_y;
	int w = x2 - x1 + rate_x;
	int y = y1 - rate_y + (h / 2) + (y2 - y1) / 2;
	int x = x1 - rate_x + (w / 2) + (x2 - x1) / 3;
	
	drawInitialStone( (y1 + ( y2 - y1 ) / 2),  (x1 - 1 + ( ( x2 - x1 ) / 2 ) ) );

	if(x % 2 != 0)
		x+=3;
	x = x1 - 1 + (x2 - x1) / 2;
	
//	mvprintw(row-3, 0, "(y1, x1) : (%d, %d)\t(y2, x2) : (%d, %d)\n", y1, x1, y2, x2);
//	mvprintw(row-2, 0, "y : %d x : %d\n", y, x); 
//	mvprintw(row-1, 0, "Server\n");
	
	move(y, x);
        refresh() ;

        int c ;
        c = getch();
	do{
		// 키 입력
                switch (c) { 
                        case KEY_UP:
                                if( y > y1 + 1 ) y-- ;
                                break ;
                        
                        case KEY_DOWN:
                                if( y < y2 - 1 ) y++ ;
                                break ;

                        case KEY_LEFT:
				if( x > x1 + 2 ) x -= 2 ;
                                break ;

                        case KEY_RIGHT:
                                if( x < x2 - 2 ) x += 2 ;
                                break ;

                        default:
                                if (isalpha(c)) {	
					char ch = mvinch(y, x) & A_CHARTEXT ;
					if( ch == ' ' ){
                                        	printw("%c", c) ;
						char msg[256];
						sprintf(msg, "%d %d %c", y, x, c);
						c == 9999 ;
						send(conn_fd, msg, strlen(msg), 0);
					}
                                }
                                break ;
                }

		// 커서 변화
		int s ;
		char buf[256] ;
//		mvprintw(row-3, 0, "(y1, x1) : (%d, %d)\t(y2, x2) : (%d, %d)\n", y1, x1, y2, x2);
//		mvprintw(row-2, 0, "y : %d x : %d\n", y, x); 
//		mvprintw(row-1, 0, "Server\n");


		if ( c == 9999 ){
			if( (s = recv(conn_fd, buf, 255, 0)) > 0 ){
				int y, x;
				char ch;
				sscanf(buf, "%d %d %c", &y, &x, &ch);
				mvprintw(y, x, "%c", ch);
			}		
		}

        	move(y, x) ;
        	refresh() ;
	
	}while(c = getch());

	endwin() ;
}

void clientDrawBoard(pthread_mutex_t *lock, int conn_fd){
        initscr() ;
        clear() ;

        noecho() ;
        cbreak() ;
        keypad(stdscr, TRUE) ;

        int row, col ;
        getmaxyx(stdscr, row, col) ;

        curs_set(2) ;
				
        int y1 = row / 3 - 5 ;	
        int x1 = col / 3 - 12 ;
        int y2 = row / 3 + 4 ;
        int x2 = col / 3 + 6 ;

	if((y1 % 2 != 0) && (x1 % 2 == 0) && (y2 % 2 == 0) && (x2 % 2 == 0)){
		y1++;
		x1++;
		y2++;
		x2++;
	}

	int rate_y = (row / 2) - (row / 3) ;
	int rate_x = (col / 2) - (col / 3) ;

	if(rate_x % 2 == 0) rate_x++;

	y1 = y1 + rate_y ;
	x1 = x1 + rate_x ;
	y2 = y2 + rate_y ;
	x2 = x2 + rate_x ;

        rectangle(y1, x1, y2, x2) ;
	int h = y2 - y1 + rate_y;
	int w = x2 - x1 + rate_x;
	int y = y1 - rate_y + (h / 2) + (y2 - y1) / 2;
	int x = x1 - rate_x + (w / 2) + (x2 - x1) / 3;
	
	drawInitialStone( (y1 + ( y2 - y1 ) / 2),  (x1 - 1 + ( ( x2 - x1 ) / 2 ) ) );

	if(x % 2 != 0)
		x+=3;
	x = x1 - 1 + (x2 - x1) / 2;
	
//	mvprintw(row-3, 0, "(y1, x1) : (%d, %d)\t(y2, x2) : (%d, %d)\n", y1, x1, y2, x2);
//	mvprintw(row-2, 0, "y : %d x : %d\n", y, x); 
//	mvprintw(row-1, 0, "Client\n");

	move(y, x);
        refresh() ;



	int s ;
	char buf[256] ;
	
	if( (s = recv(conn_fd, buf, 255, 0)) > 0 ){
		char ch;
		sscanf(buf, "%d %d %c", &y, &x, &ch);
		mvprintw(y, x, "%c", ch);

		move(y, x);
		refresh();
	}

	int c ;
    	while (c = getch()) {
	        switch (c) {
            		case KEY_UP:
                		if( y > y1 + 1 ) y-- ;
                		break ;
            		case KEY_DOWN:
                		if( y < y2 - 1 ) y++ ;
                		break ;
            		case KEY_LEFT:
                		if( x > x1 + 2 ) x -= 2 ;
                		break ;
            		case KEY_RIGHT:
                		if( x < x2 - 2 ) x += 2 ;
                		break ;
            		default:
                		if (isalpha(c)) {
                    			char ch = mvinch(y, x) & A_CHARTEXT ;
                    			if( ch == ' ' ) {
                                        	printw("%c", c) ;
						char msg[256];
						sprintf(msg, "%d %d %c", y, x, c);
						c = 9999;
						send(conn_fd, msg, strlen(msg), 0);
                    			}
                		}
                		break ;
        	}
		if ( c == 9999 ){
			if( (s = recv(conn_fd, buf, 255, 0)) > 0 ){
				int y, x;
				char ch;
				sscanf(buf, "%d %d %c", &y, &x, &ch);
				mvprintw(y, x, "%c", ch);
			}		
		}

//		mvprintw(row-3, 0, "(y1, x1) : (%d, %d)\t(y2, x2) : (%d, %d)\n", y1, x1, y2, x2);
//		mvprintw(row-2, 0, "y : %d x : %d\n", y, x); 
//		mvprintw(row-1, 0, "Client\n");

        	move(y, x) ;
        	refresh() ;


    	}

	endwin() ;
}


