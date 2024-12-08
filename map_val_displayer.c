//clang ncursestest.c -o ncursestest -lncurses
#include <ncurses.h>
#include "game_initialize.c"
#include <stdlib.h> 
#include <stdio.h>

void ncursepoint(int row, int col, char* str, int color_pair)
{   
    attron(COLOR_PAIR(color_pair));    // Turn on color pair 1
    mvprintw(row, col, str);
    attroff(COLOR_PAIR(color_pair));
}

#include <ncurses.h>
int main() {

    struct MapData map_a = map_initialize_default();

    initscr();               // Initialize ncurses
    start_color();           // Initialize color functionality
    raw();                   // Disable line buffering
    noecho();                // Don't echo user input
    curs_set(0);             // Hide the cursor

    




    //Define color pairs
    init_color(8, 0, 0, 500);
    init_color(9, 0, 0, 1000);
    init_color(10, 750, 750, 1000);
    init_color(11, 500, 1000, 500);
    init_color(12, 1000, 500, 0);
    init_color(13, 500, 500, 500);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);//white
    init_pair(2, 8, COLOR_BLACK); //ocean
    init_pair(3, 9, COLOR_BLACK); //lake
    init_pair(4, 10, COLOR_BLACK);//river
    init_pair(5, 11, COLOR_BLACK);//flat
    init_pair(6, 12, COLOR_BLACK);//hill
    init_pair(7, 13, COLOR_BLACK);//mountain


    char user_info[] = "Displaying Map TileTypes\n";
    ncursepoint(0,0,user_info,1);
    for(int row = 0; row < map_a.rows; row++)
    {
        for(int col = 0; col < map_a.cols; col++)
        {
            char display_val[10];
            //change what to display
            sprintf(display_val, "%d", map_a.tiles[row][col].tiletype);
            int color_pair_val;
            if(map_a.tiles[row][col].tiletype == ocean)
            {
                color_pair_val = 2;
            }
            else if(map_a.tiles[row][col].tiletype == lake)
            {
                color_pair_val = 3;
            }
            else if(map_a.tiles[row][col].tiletype == river)
            {
                color_pair_val = 4;
            }
            else if(map_a.tiles[row][col].tiletype == flat)
            {
                color_pair_val = 5;
            }
            else if(map_a.tiles[row][col].tiletype == hill)
            {
                color_pair_val = 6;
            }
            else if(map_a.tiles[row][col].tiletype == mountain)
            {
                color_pair_val = 7;
            }
            ncursepoint(row+1,col * 2,display_val,color_pair_val);
        }
    }

    refresh();               
    getch();                  
    clear();

    char user_info2[] = "Displaying Map Food\n";
    ncursepoint(0,0,user_info2,1);
    for(int row = 0; row < map_a.rows; row++)
    {
        for(int col = 0; col < map_a.cols; col++)
        {
            char display_val[10];
            //change what to display
            sprintf(display_val, "%d", map_a.tiles[row][col].food);
            int color_pair_val;
            if(map_a.tiles[row][col].tiletype == ocean)
            {
                color_pair_val = 2;
            }
            else if(map_a.tiles[row][col].tiletype == lake)
            {
                color_pair_val = 3;
            }
            else if(map_a.tiles[row][col].tiletype == river)
            {
                color_pair_val = 4;
            }
            else if(map_a.tiles[row][col].tiletype == flat)
            {
                color_pair_val = 5;
            }
            else if(map_a.tiles[row][col].tiletype == hill)
            {
                color_pair_val = 6;
            }
            else if(map_a.tiles[row][col].tiletype == mountain)
            {
                color_pair_val = 7;
            }
            ncursepoint(row+1,col * 2,display_val,color_pair_val);
        }
    }

    refresh();               
    getch();                  
    clear();

    char user_info3[] = "Displaying Map Production\n";
    ncursepoint(0,0,user_info3,1);
    for(int row = 0; row < map_a.rows; row++)
    {
        for(int col = 0; col < map_a.cols; col++)
        {
            char display_val[10];
            //change what to display
            sprintf(display_val, "%d", map_a.tiles[row][col].production);
            int color_pair_val;
            if(map_a.tiles[row][col].tiletype == ocean)
            {
                color_pair_val = 2;
            }
            else if(map_a.tiles[row][col].tiletype == lake)
            {
                color_pair_val = 3;
            }
            else if(map_a.tiles[row][col].tiletype == river)
            {
                color_pair_val = 4;
            }
            else if(map_a.tiles[row][col].tiletype == flat)
            {
                color_pair_val = 5;
            }
            else if(map_a.tiles[row][col].tiletype == hill)
            {
                color_pair_val = 6;
            }
            else if(map_a.tiles[row][col].tiletype == mountain)
            {
                color_pair_val = 7;
            }
            ncursepoint(row+1,col * 2,display_val,color_pair_val);
        }
    }

    refresh();               
    getch();                  
    clear();

    char user_info4[] = "Displaying Map Resources\n";
    ncursepoint(0,0,user_info4,1);
    for(int row = 0; row < map_a.rows; row++)
    {
        for(int col = 0; col < map_a.cols; col++)
        {
            char display_val[10];
            //change what to display
            sprintf(display_val, "%d", map_a.tiles[row][col].resource);
            int color_pair_val;
            if(map_a.tiles[row][col].tiletype == ocean)
            {
                color_pair_val = 2;
            }
            else if(map_a.tiles[row][col].tiletype == lake)
            {
                color_pair_val = 3;
            }
            else if(map_a.tiles[row][col].tiletype == river)
            {
                color_pair_val = 4;
            }
            else if(map_a.tiles[row][col].tiletype == flat)
            {
                color_pair_val = 5;
            }
            else if(map_a.tiles[row][col].tiletype == hill)
            {
                color_pair_val = 6;
            }
            else if(map_a.tiles[row][col].tiletype == mountain)
            {
                color_pair_val = 7;
            }
            ncursepoint(row+1,col * 2,display_val,color_pair_val);
        }
    }

    refresh();               
    getch();                  
    clear();
    







    endwin(); //close ncurses

    return 0;
}