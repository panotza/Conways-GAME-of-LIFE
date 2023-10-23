#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// #define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define SCREEN_ROW 20
#define SCREEN_COL 80

int a[SCREEN_ROW][SCREEN_COL], b[SCREEN_ROW][SCREEN_COL];
int (*g_screens)[SCREEN_ROW][SCREEN_COL] = &a;
int (*g_next_screens)[SCREEN_ROW][SCREEN_COL] = &b;

void term_clear()
{
    if (write(STDOUT_FILENO, "\x1b[2J", 4) == -1)
    {
        printf("cannot write to stdout\n");
        exit(1);
    }
}

void term_set_cursor(int row, int col)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", row, col);
    if (write(STDOUT_FILENO, buf, strlen(buf)) == -1)
    {
        printf("cannot write to stdout\n");
        exit(1);
    }
}

void init()
{
    for (int r = 0; r < SCREEN_ROW; r++)
    {
        for (int c = 0; c < SCREEN_COL; c++)
        {
            (*g_screens)[r][c] = rand() % 2 == 0;
        }
    }
}

int calc_next_gen(int r, int c)
{
    int neighbor_cnt = 0;
    int is_alive = (*g_screens)[r][c];
    // (-1, -1) (-1, 0) (-1, 1)
    // ( 0, -1) ( 0, 0) ( 0, 1)
    // ( 1, -1) ( 1, 0) ( 1, 1)

    // wrapping = (n + offset + MAX) % MAX
    // rows = 20, cols = 80
    // (-1, -1) > (19, 79)
    // (-1, 0) > (19, 0)
    // (-1, 1) > (19, 1)
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            int rr = (r + i + SCREEN_ROW) % SCREEN_ROW;
            int cc = (c + j + SCREEN_COL) % SCREEN_COL;
            if (rr > SCREEN_ROW - 1)
            {
                printf("overflow rr %d\n", rr);
                exit(1);
            }
            if (cc > SCREEN_COL - 1)
            {
                printf("overflow cc %d\n", cc);
                exit(1);
            }
            neighbor_cnt += (*g_screens)[rr][cc];
        }
    }
    neighbor_cnt -= (*g_screens)[r][c];

    // Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    // Any live cell with two or three live neighbours lives on to the next generation.
    // Any live cell with more than three live neighbours dies, as if by overpopulation.
    // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    return is_alive ? (neighbor_cnt == 2 || neighbor_cnt == 3) : neighbor_cnt == 3;
}

void tick()
{
    // calculate next screen
    for (int r = 0; r < SCREEN_ROW; r++)
    {
        for (int c = 0; c < SCREEN_COL; c++)
        {
            (*g_next_screens)[r][c] = calc_next_gen(r, c);
        }
    }

    // swap screen
    int(*tmp)[SCREEN_ROW][SCREEN_COL] = g_screens;
    g_screens = g_next_screens;
    g_next_screens = tmp;
}

void render()
{
    term_clear();
    term_set_cursor(1, 1);
    for (int r = 0; r < SCREEN_ROW; r++)
    {
        for (int c = 0; c < SCREEN_COL; c++)
        {
            if ((*g_screens)[r][c])
                printf("o");
            else
                printf(".");
        }
        printf("\n");
    }
}

int main()
{
    srand(time(NULL));

    init();
    while (1)
    {
        tick();
        render();
        usleep(1000 * 100);
    }
    return 0;
}
