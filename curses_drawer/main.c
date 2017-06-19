#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _Pt{
	int x;
	int y;
	struct _Pt *next;
} Pt;


char characterToSave;
Pt* Head = NULL;
void allocate(int x, int y) {
	if (Head == NULL) {
		Head = (Pt*)malloc(sizeof(Pt));
		Head->x = x;
		Head->y = y;
		Head->next = NULL;
	} else {
		Pt* iter = Head;
		bool found = FALSE;
		while (iter) {
			if (iter->x == x && iter->y == y) {
				return;
			}
			iter = iter->next;
		}

		iter = Head;
		while (iter->next) {
			iter = iter->next;
		}
		iter->next = (Pt*)malloc(sizeof(Pt));
		iter = iter->next;
		iter->x = x;
		iter->y = y;
		iter->next = NULL;
	}
}

void deallocate(int x, int y) {
	Pt *iter, *prev;
	if (Head == NULL) {
		return;
	}
	iter = Head;
	prev = Head;
	while(iter) {
		if (iter->x == x && iter->y == y) {
			if (iter == Head) {
				Head = iter->next;
				free(iter);
				iter = Head;
			} else {
				prev->next = iter->next;
				free(iter);
				iter = prev->next;
			}
		} else {
			prev = iter;
			iter = iter->next;
		}
	}
}

void delete_all() {
	Pt* iter = Head;
	while(iter) {
		Pt* prev = iter;
		iter = iter->next;
		free(prev);
	}
	Head = NULL;
}

void save() {
	FILE *fp;
	fp = fopen("saved.json","a");
	Pt* iter = Head;
	fprintf(fp,"[[");
	while(iter) {
		fprintf(fp,"[%d,%d]",iter->x, iter->y);
		if (iter->next != NULL) {
			fprintf(fp,",");
		}
		iter = iter->next;
	}
	fprintf(fp,"],\"%c\"],\n",characterToSave);
	fclose(fp);
}


void draw_screen(WINDOW *win, int mode) {
	int i,j;

	for (i = 0; i < 50; i++) {
		mvwaddch(win, i, 50, ACS_VLINE);
		mvwaddch(win, 50, i, ACS_HLINE);
	}
	mvwaddch(win, 50,50, ACS_LRCORNER);
	if (mode == 0) {
		mvwaddstr(win, 0, 51,"MODE: CONTINUOUS");
	} else {
		mvwaddstr(win, 0, 51,"MODE: CLICK     ");
	}
	wrefresh(win);
}

void main_loop(WINDOW *win) {
	int c, choice = 0, val;
	int movement_started = 0;
	int delete_started = 0;
	int mode = 0;
	FILE *fp;
	fp = fopen("log","w");
	if (fp == NULL) {
		return;
	}
	MEVENT event;
	while(1)
	{	c = getch();
		switch(c)
		{	
			case KEY_MOUSE:
			val = getmouse(&event);
			if (val == OK)
			{	/* When the user clicks left mouse button */
				fprintf(fp,"Got mouse event\n");
				fprintf(fp,"%#lx,%#lx\n",event.bstate, BUTTON1_CLICKED);
				if (event.x >= 50 || event.y >= 50) {
					break;
				}
				if(event.bstate & BUTTON1_CLICKED)
				{	
					allocate(event.x, event.y);
					mvaddch(event.y, event.x,ACS_BLOCK);
					if (mode == 0) {
						if (movement_started) {
							movement_started = 0;
						} else {
							movement_started = 1;
							delete_started = 0;
						}
					}
					refresh();
				} else if (event.bstate & BUTTON3_CLICKED) {
					deallocate(event.x,event.y);
					mvaddch(event.y, event.x, ' '); 
					if (mode == 0) {
						if (delete_started) {
							delete_started = 0;
						} else {
							delete_started = 1;
							movement_started = 0;
						}
					}

					refresh();
				} 
				if (event.bstate & REPORT_MOUSE_POSITION) {
					if (mode != 0) {
						break;
					}
					if (movement_started) {
						allocate(event.x, event.y);
						mvaddch(event.y, event.x,ACS_BLOCK);
						refresh();
					} else if (delete_started) {
						deallocate(event.x,event.y);
						mvaddch(event.y, event.x, ' '); 
						refresh();
					}	
				}
			}
			break;
			case 'q':
				delete_all();
				return;
			break;
			case 'e':
				delete_all();
				erase();
				draw_screen(win, mode);
				movement_started = 0;
				delete_started = 0;

				break;
			case 's':
				save();
				delete_all();
				erase();
				draw_screen(win, mode);
				movement_started = 0;
				delete_started = 0;
				break;
			case 'm':
				if (mode == 0) {
					mode = 1;
					movement_started = 0;
					delete_started = 0;
					mvwaddstr(win, 0, 51,"MODE: CLICK     ");
				} else {
					mode = 0;
					mvwaddstr(win, 0, 51,"MODE: CONTINUOUS");
				}
				break;
			default:
				fprintf(fp,"Got event %d\n",c);
				break;	
		}
		fflush(fp);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("%s <character>\n",argv[0]);
		return 1;
	}

	characterToSave = argv[1][0];
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED | REPORT_MOUSE_POSITION, NULL);

	// makes the terminal report mouse movements.
	printf("\033[?1003h\n"); 
	draw_screen(stdscr, 0);
	main_loop(stdscr);
	endwin();
	return 0;
}
