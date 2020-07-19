#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

//height and width of the gameboard
#define HEIGHT 10
#define WIDTH 20

struct Board {
	int height;
	int width;
	char contents[HEIGHT + 1][WIDTH + 1];
} board = {HEIGHT, WIDTH, {}};

struct Snake {
	int length;
	int direction;
	int body[(HEIGHT - 2) * (WIDTH - 2)][2]; //maximum size of snake
} snake;

struct Apple {
	int y;
	int x;
} apple;

//declare functions
void config();
struct Snake snakeMove(struct Snake);
int input(int dir);
struct Board initBoard(struct Board);
struct Board clearBoard(struct Board);
struct Snake initSnake(struct Snake);
struct Apple initApple(struct Apple);
int lost(struct Board, struct Snake);
void render(struct Board);
void clearInput();

int main() {

	//set up game
	config();
	board = initBoard(board);
	snake = initSnake(snake);
	apple = initApple(apple);

	while(1) {
		//start timer
		int msec = 0, speed = 200;
		clock_t before = clock();

		int c = lost(board, snake);
		if (c == 1) {
			snake = initSnake(snake);
			apple = initApple(apple);
		}
		else if (c == 0) {
			return 0;
		}

		if (snake.body[0][0] == apple.y && snake.body[0][1] == apple.x) {
			apple = initApple(apple);
			snake.body[snake.length + 1][0] = snake.body[snake.length][0];
			snake.body[snake.length + 1][1] = snake.body[snake.length][1];
			snake.length++;
		}
		
		board = clearBoard(board);
		snake = snakeMove(snake);
		board.contents[apple.y][apple.x] = '*';
		render(board);

		//end timer
		do {
			clock_t difference = clock() - before;
			msec = difference * 1000 / CLOCKS_PER_SEC;
		} while (msec < speed);
	}
}

void config() {
	initscr(); //start ncurses
	cbreak(); //register all input
	noecho(); //turn off echo
	nodelay(stdscr, 1); //grab last input
	curs_set(0); //turn off cursor
	srand(time(NULL)); //seed rng
}

struct Snake snakeMove(struct Snake snake) {
	snake.direction = input(snake.direction);
	
	for (int k = snake.length; k >= 0; k--) {
		if (k == 0) {
			board.contents[snake.body[k][0]][snake.body[k][1]] = 'X';
			if (snake.direction == 0) {
				snake.body[k][1]--;
			}
			else if (snake.direction == 1) {
				snake.body[k][0]--;
			}
			else if (snake.direction == 2) {
				snake.body[k][1]++;
			}
			else if (snake.direction == 3) {
				snake.body[k][0]++;
			}
		}
		else {
			board.contents[snake.body[k][0]][snake.body[k][1]] = 'O';
			snake.body[k][0] = snake.body[k - 1][0];
			snake.body[k][1] = snake.body[k - 1][1];
		}
	}
	
	return snake;
}

int input(int dir) {
	int c;
	c = getch();
	
	if (c == 'a' && dir != 0 && dir != 2) {
		 dir = 0;
	}
	else if ((c == ',' || c == 'w') && dir != 1 && dir != 3) {
		dir = 1;
	}
	else if ((c == 'e' || c == 'd') && dir != 2 && dir != 0) {
		dir = 2;
	}
	else if ((c == 'o' || c == 's') && dir != 3 && dir != 1) {
		dir = 3;
	}
	else if (c == 'a' || c == ',' || c == 'w' || c == 'e' || c == 'd' || c == 'o' || c == 's') {
		dir = input(dir);
	}
	
	return dir;
}

struct Board initBoard(struct Board board) {
	for (int i = 0; i <= board.height; i++) {
		for (int j = 0; j <= board.width; j++) {
			if ((i == 0 || i == board.height) && (j == 0 || j == board.width)) {
				board.contents[i][j] = '+';
			}
			else if (i == 0 || i == board.height) {
				board.contents[i][j] = '-';
			}
			else if (j == 0 || j == board.width) {
				board.contents[i][j] = '|';
			}
			else {
				board.contents[i][j] = ' ';
			}
		}
	}
	
	return board;
}

struct Board clearBoard(struct Board board) {
	for (int i = 1; i <= board.height - 1; i++) {
		for (int j = 1; j <= board.width - 1; j++) {
			board.contents[i][j] = ' ';
		}
	}

	return board;
}

struct Snake initSnake(struct Snake snake) {
	for (int i = 1; i < (board.height - 2) * (board.width - 2); i++) {
		snake.body[i][0] = -1;
		snake.body[i][1] = -1;
	}

	//start position
	snake.length = 2;
	snake.direction = 2;
	snake.body[0][0] = 2;
	snake.body[0][1] = 3;
	snake.body[1][0] = 2;
	snake.body[1][1] = 2;
	snake.body[2][0] = 2;
	snake.body[2][1] = 1;

	return snake;	
}

struct Apple initApple(struct Apple apple) {
	apple.y = rand() % (board.height - 2) + 1;
	apple.x = rand() % (board.width - 2) + 1;
	for (int k = snake.length; k >= 0; k--) {
		if (snake.body[k][0] == apple.y && snake.body[k][1] == apple.x) {
			apple = initApple(apple); //try again if apple spawns on snake
			break;
		}
	}
	
	return apple;
}

//when player dies...
int lost(struct Board board, struct Snake snake) {
	for (int h = snake.length; h >= 1; h--) {
		if ((snake.body[h][0] == snake.body[0][0] && snake.body[h][1] == snake.body[0][1]) || snake.body[0][0] == 0 || snake.body[0][0] == board.height || snake.body[0][1] == 0 || snake.body[0][1] == board.width) {
			while(getch() != ERR) {
				getch();
			}
			endwin();
			printf("\e[1;1H\e[2J");
			board = clearBoard(board);
			
			for (int k = snake.length; k >= 0; k--) {
				if (k == 0) {
					board.contents[snake.body[k][0]][snake.body[k][1]] = 'X';
				}
				else {
					board.contents[snake.body[k][0]][snake.body[k][1]] = 'O';
				}
			}
			for (int i = 0; i <= board.height; i++) {
				for (int j = 0; j <= board.width; j++) {
					printf("%c", board.contents[i][j]);
				}
				printf("\n");
			}
			printf("\nYou lose!\nScore: %d\n\nRetry?\n", snake.length - 2);
			char retry;
			char clear;
			scanf(" %c%[^\n]", &retry, &clear);
			if (retry == 'y') {
				config();
				return 1;
			}
			return 0;
		}
	}
		
	return 2;
}

void render(struct Board board) {
	erase();
	for (int i = 0; i <= board.height; i++) {
		for (int j = 0; j <= board.width; j++) {
			printw("%c", board.contents[i][j]);
		}
		printw("\n");
	}
	refresh();
}