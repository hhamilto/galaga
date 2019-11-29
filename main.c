#include <curses.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>
#include <sys/time.h>

// gcc main.c -lncurses -o galaga

int getrandom_int(){
	uint random;
	int bytesRead = getrandom(&random, sizeof random, 0);
	if (bytesRead == -1) {
		printf("Error reading from getrandom\n");
		return -1;
	}
	return (int) (random % INT_MAX);
}

char ship[][9] = {
	{' ',' ',' ',' ','A',' ',' ',' ',' '},
	{' ',' ',' ','/','o','\\',' ',' ',' '},
	{'*',' ','|','_','_','_','|',' ','*'},
	{'|','=','|','X','|','X','|','=','|'},
};
/*
	A
   /o\
* |___| *
|=|X|X|=|
*/

#define STATIONARY 0
#define RIGHT 1
#define LEFT 2

int usDiff(struct timeval now, struct timeval then)
{
	int secDiff = now.tv_sec - then.tv_sec;
	int ansInUSecs = secDiff * 1000 * 1000;
	ansInUSecs -= then.tv_usec;
	ansInUSecs += now.tv_usec;
	return ansInUSecs;
}

typedef struct {
	int x;
	int y;
} position;

position bullets[100];
int numBullets = 0;

void pushBullet(int x, int y) {
	bullets[numBullets].x = x;
	bullets[numBullets].y = y;
	numBullets++;
}

int main(void)
{
	int maxlines, maxcols;



	/* initialize curses */

	initscr();
	cbreak();
	noecho();

	curs_set(FALSE);
	clear();

	nodelay(stdscr, TRUE);

	/* initialize triangle */

	maxlines = LINES - 1;
	maxcols = COLS - 1;


	/* initialize yi,xi with random values */


	/* done */

	// mvaddstr(maxlines, 0, "Press any key to quit");

	char str[100];

	int i = 0;
	int x = maxcols/2;

	struct timeval now;
	struct timeval lastInputUSec;
	struct timeval lastMovementUSec;
	struct timeval lastBulletMovement;
	gettimeofday(&now, NULL);
	gettimeofday(&lastInputUSec, NULL);
	gettimeofday(&lastMovementUSec, NULL);
	gettimeofday(&lastBulletMovement, NULL);
	lastInputUSec = now;
	lastMovementUSec = now;
	int delay = 0;
	int usecsSinceLastInput = 0;
	int direction = STATIONARY;

		sprintf(str, "lol");
	while (true) {
		gettimeofday(&now, NULL);
		char c = getch();
		if ( c != -1) {
			mvaddch(maxlines/2, maxcols/2, c);
			lastInputUSec = now;
		} else { 
			i++;
		}
		usecsSinceLastInput = usDiff(now, lastInputUSec);
		if (usDiff(now, lastInputUSec) > 35000) {
			direction = STATIONARY;
		}
		if (c == 'a') {
			direction = LEFT;
			sprintf(str, "left              ");
		}
		if (c == 'd') {
			direction = RIGHT;
			sprintf(str, "r9gt                ");
		}

		int shipLines = sizeof(ship)/sizeof(ship[0]);

		if (usDiff(now, lastBulletMovement) > 10 * 1000) {

			for (int i = 0; i < numBullets; i++) {
				mvaddch(bullets[i].y, bullets[i].x, ' ');
			}
			for (int i = 0; i < numBullets; i++) {
				bullets[i].y--;
			}

			gettimeofday(&lastBulletMovement, NULL);
		}

		if (c == ' ') {

			pushBullet(x+4,maxlines-shipLines-1);
			sprintf(str, "bullet %d          ", numBullets);
		}

		if (usDiff(now, lastMovementUSec) > 15 * 1000 && direction != STATIONARY) {
			for (size_t i = 0; i < shipLines; i++){
				for (size_t j = 0; j < sizeof(ship[i])/sizeof(ship[i][0]); j++){
					mvaddch(maxlines+i-shipLines, x+j, ' ');
				};
			}
			if (direction == LEFT) {
				x--;
			}
			if (direction == RIGHT) {
				x++;
			}
			sprintf(str, "moving          ");
			lastMovementUSec = now;
		}

		// sprintf(str, "usec since last input: %d  ", usDiff(now, lastMovementUSec) );

		for (size_t i = 0; i < shipLines; i++){
			for (size_t j = 0; j < sizeof(ship[i])/sizeof(ship[i][0]); j++){
				mvaddch(maxlines+i-shipLines, x+j, ship[i][j]);
			};
		}

		for (int i = 0; i < numBullets; i++) {

			sprintf(str, "printing bullet");
			mvaddch(bullets[i].y, bullets[i].x, '^');
		}
		mvaddstr(maxlines, 0, str);
		// mvaddstr(maxlines, 0, str);
		// mvaddch(maxlines/2, x, '.');
		// mvaddch(maxlines/2, x+1, '.');
		// mvaddch(maxlines/2+1, x+1, '.');
		// mvaddch(maxlines/2+1, x, '.');
	}

	/*



	A
   /o\
* |___| *
|=|X|X|=|



	*/

	refresh();

	getch();
	endwin();

	exit(0);
}



// int main(void)
// {
//	 long iter;
//	 int yi, xi;
//	 int y[3], x[3];
//	 int index;
//	 int maxlines, maxcols;

//	 /* initialize curses */

//	 initscr();
//	 cbreak();
//	 noecho();

//	 clear();

//	 /* initialize triangle */

//	 maxlines = LINES - 1;
//	 maxcols = COLS - 1;

//	 y[0] = 0;
//	 x[0] = 0;

//	 y[1] = maxlines;
//	 x[1] = maxcols / 2;

//	 y[2] = 0;
//	 x[2] = maxcols;

//	 mvaddch(y[0], x[0], '0');
//	 mvaddch(y[1], x[1], '1');
//	 mvaddch(y[2], x[2], '2');

//	  initialize yi,xi with random values 

//	 yi = getrandom_int() % maxlines;
//	 xi = getrandom_int() % maxcols;

//	 mvaddch(yi, xi, '.');

//	 /* iterate the triangle */

//	 for (iter = 0; iter < ITERMAX; iter++) {
//		 index = getrandom_int() % 3;

//		 yi = (yi + y[index]) / 2;
//		 xi = (xi + x[index]) / 2;

//		 mvaddch(yi, xi, '*');
//		 refresh();
//	 }

//	 /* done */

//	 mvaddstr(maxlines, 0, "Press any key to quit");

//	 refresh();

//	 getch();
//	 endwin();

//	 exit(0);
// }