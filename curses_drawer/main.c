#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"

typedef struct _Pt{
	int x;
	int y;
	struct _Pt *next;
} Pt;

extern void get_points();

char characterToSave;
jsmntok_t t[10000]; /* We expect no more than 128 tokens */
int num_t=0;
int present_view = 1;
char present_char[10];
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
	} else if (mode == 1){
		mvwaddstr(win, 0, 51,"MODE: CLICK     ");
	} else if (mode == 3) {
		Pt* iter = Head;	
		while(iter) {
			mvwaddch(win, iter->y, iter->x, ACS_BLOCK);
			iter=iter->next;
		}
		mvwaddstr(win, 0, 51,present_char);
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
void main_show_loop(WINDOW *win) {
	int c, choice = 0, val;
	int movement_started = 0;
	int delete_started = 0;
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
			case 'n':
				delete_all();
				erase();
				present_view += 1;
				get_points();
				draw_screen(win, 3);
				wrefresh(win);
				break;
			case 'q':
				return;
			default:
				fprintf(fp,"Got event %d\n",c);
				break;	
		}
		fflush(fp);
	}
}

char *final_buf;
char *read_all(FILE *fp) {
	char *buf_hold[30];
	char *iter_buf, *buf;
	int num_read, idx=0,i;
	do {
		buf = (char*) malloc(1000);
		num_read = fread(buf, 1, 1000, fp) ;
		printf("%d\n", num_read);
		buf_hold[idx] = buf;
		idx++;
	} while (num_read == 1000);

	final_buf = (char*)malloc(((idx-1)*1000) + num_read + 1);
	printf("final_buf: %d\n",((idx-1)*1000) + num_read + 1);
	iter_buf = final_buf;
	for (i = 0; i < (idx-1); i++) {
		memcpy((void*)iter_buf, (void*)buf_hold[i],1000);
		iter_buf+= 1000;
	}
	memcpy(iter_buf, buf, num_read);
	iter_buf += num_read;
	*iter_buf = 0;
	for (i = 0; i < idx; i++) {
		free(buf_hold[i]);
	}
	return final_buf;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void read_json(char *fname) {
	FILE *fp;
	int i, depth;
	int r;
	jsmn_parser p;
	char *JSMN_STRING;

	jsmn_init(&p);

	fp = fopen(fname,"r");
	JSMN_STRING=read_all(fp);
	printf("%s\n",JSMN_STRING);
	r = jsmn_parse(&p, JSMN_STRING, strlen(JSMN_STRING), t, 
		sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return;
	}
	num_t = r;
}

int get_number(int idx) {
  char num_buf[5];
	memcpy(num_buf, final_buf + t[idx].start, t[idx].end - t[idx].start);
	num_buf[t[idx].end - t[idx].start] = 0;
	//printf("%d,%d,%s,%c%c\n", t[idx].start, t[idx].end, num_buf,*(final_buf+t[idx].start),*(final_buf+t[idx].end));
	return atoi(num_buf);
}

int find_points_in_sample(int parent) {
	int i,x,y;
	for (i = parent; i < num_t; i++) {
		if (t[i].type == JSMN_ARRAY && t[i].parent == parent) {
			if (t[i+1].type != JSMN_PRIMITIVE || t[i+2].type != JSMN_PRIMITIVE ) {
					printf("FAILING PRIMITIVE CHECK\n");
					return -1;
			}
			x = get_number(i+1);
			y = get_number(i+2);
			allocate(x,y);
		}
	}
}

void find_value_of_sample(int idx) {
	present_char[0] = *(final_buf + t[idx].start);
	present_char[1] = ' ';
	sprintf(present_char+2,"%d", present_view);
}

void get_points() {
	int array_depth[3];
	int parent = 0;
	int sample_num = 0,i = 0, sample_start;

again:
	for (i = 0; i < num_t; i++) {
		if (t[i].parent == 0) {
			sample_num++;
		}	
		if (sample_num == present_view) {
			break;
		}
	}
	if (i == num_t) {
		present_view = 1;
		sample_num = 0;
		goto again;
	}
	sample_start = i;
	
	for (i = sample_start; i < num_t; i++) {
		if (t[i].parent == sample_start) {
			if (t[i].type == JSMN_ARRAY) {
				find_points_in_sample(i);
			} else if (t[i].type == JSMN_STRING) {
				find_value_of_sample(i);
			}
		}
	}	
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("%s [read|save] <character>\n",argv[0]);
		return 1;
	}

	if (strcmp(argv[1],"save") == 0) {
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		refresh();
		mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED | REPORT_MOUSE_POSITION, NULL);

		// makes the terminal report mouse movements.
		printf("\033[?1003h\n"); 

		draw_screen(stdscr, 0);
		characterToSave = argv[2][0];
		main_loop(stdscr);
		endwin();
	} else {
		read_json(argv[2]);
		get_points();

		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		refresh();


		draw_screen(stdscr, 3);
		main_show_loop(stdscr);
		endwin();
	}
	return 0;
}
