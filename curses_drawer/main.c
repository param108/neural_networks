#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _Pt{
	int x;
	int y;
	struct _Pt *next;
} Pt;


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
	fprintf(fp,"[");
	while(iter) {
		fprintf(fp,"[%d,%d]",iter->x, iter->y);
		if (iter->next != NULL) {
			fprintf(fp,",");
		}
		iter = iter->next;
	}
	fprintf(fp,"]\n");
	fclose(fp);
}


void main_loop(WINDOW *win) {
	int c, choice = 0, val;
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
				if(event.bstate & BUTTON1_CLICKED)
				{	
					allocate(event.x, event.y);
					mvaddch(event.y, event.x,ACS_BLOCK);
					fprintf(fp,"%d,%d\n",event.y, event.x);
					refresh(); 
				} else if (event.bstate & BUTTON3_CLICKED) {
					deallocate(event.x,event.y);
					mvaddch(event.y, event.x, ' '); 
				}
			}
			break;
			case 'q':
				delete_all();
				return;
			break;
			case 's':
				save();
				delete_all();
				erase();
				break;
		}
		fflush(fp);
	}
}

void main() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL);

	main_loop(stdscr);
	endwin();
}
