#include<stdio.h>
#include<ncurses.h>
#include<unistd.h>

#define HEIGHT 25
#define WIDTH 80

int start_init(char **map);
void create_map(char **map);
void check(int i, int j, char **map);
void print(char **map);
void keyboard_handler(int c, char **map, unsigned int *delay);
void check_matrix(char **map);
int start_game(char **map);

int main() {
    int status = 0;                                             //  Error state indicator
    char data[WIDTH * HEIGHT];                                  //  Matrix init
    char *map[HEIGHT];
    for (int i = 0, j = 0; i < HEIGHT * WIDTH; j++, i += WIDTH) {
        map[j] = data + i;                                      //  Placing pointers
    }
    if (start_init(map)) {                                      //  Start init
        status = 1;                               //  In case of start init error -> status error
    } else {
        status = start_game(map);
    }
    endwin();                                                   //  Correct finish of ncurses lib
    return status;
}

int start_game(char **map) {
    int status = 0;
    unsigned int delay = 8;                                          //  Input waiting delay
    if (!freopen("/dev/tty", "r", stdin)) {                        //  Reopen stream
        status = 1;
    } else {
        initscr();                                              //  Init ncurses lib
        noecho();                                               //  Not print symbols onto terminal
        print(map);
        int c = 0;
        while (c != 81 && c != 113) {                           //  Till Q/q not pressed
            halfdelay(delay);                                   //  Set delay to getch
            c = getch();
            keyboard_handler(c, map, &delay);                   //  Process input
        }
    }
    return status;
}

int start_init(char **map) {        //  Alive cells start initialization
    int status = 0;
    int n, m;       // Cell's coordinates
    char c = ' ';       // End of input
    printf("\e[1;1H\e[2J");                                     //  Clear terminal
    create_map(map);                                            //  First state map create
    while (c != '\n') {
        if (!scanf("%d%d%c", &n, &m, &c) || n <= 0 || m <= 0 || n >= HEIGHT - 1 || m >= WIDTH - 1) {
            status = 1;
            break;
        } else {
            map[n][m] = 'l';
        }
    }
    return status;
}

int check_cell(int i, int j, char **map, char **copy) {         //  Checking cells around itself
    int count = 0;                                              //  Number of alive cells
    for (int _i = i - 1; _i <= i + 1; _i++) {
        for (int _j = j - 1, k1 = 0, k2 = 0; _j <= j + 1; _j++) {
            if (_i == i && _j == j) continue;
            k1 = _i;                                            //  Checked point coordinates
            k2 = _j;
            if (_i == HEIGHT - 1) k1 = 1;
            if (_j == WIDTH - 1) k2 = 1;
            if (_i == 0) k1 = HEIGHT - 1 - 1;
            if (_j == 0) k2 = WIDTH - 1 - 1;
            if (copy[k1][k2] == 'l') count++;                   //  If checked cell alive
        }
    }
    if (count == 3) {                            //  If 3 cells alive around, the checked cell is alive
        if (i != HEIGHT - 1 && j != WIDTH - 1)                  //  Board check
            map[i][j] = 'l';                                    //  Cell alive
    } else {
        if (count < 2 || count > 3) {          //  If 1 or 4+ cells alive around, the checked cell is dead
            if (i != HEIGHT - 1 && j != WIDTH - 1)              //  Board check
                map[i][j] = ' ';                                //  Cell dead
        }
    }
    return count;
}

void check_matrix(char **map) {                                 //  Call checkcell for every matrix cell
    char data[WIDTH * HEIGHT];
    char *copy[HEIGHT];
    for (int i = 0, j = 0; i < HEIGHT * WIDTH; j++, i += WIDTH) {
        copy[j] = data + i;                                     //  Place pointers
        for (int k = 0; k < WIDTH; k++) {
            copy[j][k] = map[j][k];                             //  Copy matrix
        }
    }
    for (int i = 1; i < HEIGHT - 1; i++) {                          //  Matrix check cycle
        for (int j = 1; j < WIDTH - 1; j++) {
            check_cell(i, j, map, copy);
        }
    }
}

void keyboard_handler(int c, char **map, unsigned int *delay) {       //  Input processing
    if (c == ERR) {                                             //  ERR when no input
        check_matrix(map);
        print(map);
    }
    if (c == 87 || c == 119) {                                  //  W/w input to increase speed
        (*delay) *= 0.5;
        check_matrix(map);
        print(map);
    } else {
        if (c == 83 || c == 115) {                              //  S/s input to decrease speed
            if (*delay == 0) *delay = 1;
            (*delay) /= 0.5;
        } else {
            //  check_matrix(map);
            //  print(map);
        }
    }
}

void create_map(char **map) {                                   //  Creates base map
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            check(i, j, map);
        }
    }
}

void check(int i, int j, char **map) {                          //  Places symbols
    if (i == 0 || i == 24) {
        map[i][j] = '-';
    } else {
        if (j == 0 || j == 79) {
            map[i][j] = '|';
        } else {
            map[i][j] = ' ';
        }
    }
}


void print(char **map) {                                        //  Draws map
    clear();                                                    //  Removes previous state
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (map[i][j] == 'l') {
                printw("*");
                continue;
            }
            printw("%c", map[i][j]);                            //  Places symbol into buffer
        }
        printw("\n");
    }
    refresh();                                                  //  Prints whole buffer onto terminal
}
