#include <string.h>
#include <ncurses.h>
#include "civ.h"

#define max_height_displayed 90
#define max_width_displayed 30
#define TECH_WINDOW_SIZE 9
struct Windows{
    WINDOW* main_display;
    WINDOW* tech_display;
    WINDOW* resource_display;
    int top_row;
    int leftmost_column;
};

enum Colors {
    COMPLETED_RESEARCH = 1,
    CURRENT_RESEARCH,
    FAITH_DISPLAY,
    PRODUCTION_DISPLAY,
    SCIENCE_DISPLAY
};

void setup_windows(struct Windows* displays){

    displays->resource_display = newwin(3, COLS, 0, 0);
    displays->main_display = newwin((LINES - 3 - TECH_WINDOW_SIZE), COLS, 3, 0);
    displays->tech_display = newwin(TECH_WINDOW_SIZE, COLS, (LINES - TECH_WINDOW_SIZE), 0);
    displays->top_row = 0;
    displays->leftmost_column = 0;

    //setup the color pairs
    init_pair(COMPLETED_RESEARCH, COLOR_GREEN, COLOR_BLACK);
    init_pair(CURRENT_RESEARCH, COLOR_CYAN, COLOR_BLACK);
    init_pair(FAITH_DISPLAY, COLOR_CYAN, COLOR_BLACK);
    init_pair(PRODUCTION_DISPLAY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SCIENCE_DISPLAY, COLOR_MAGENTA, COLOR_BLACK);
}

void draw_tech(WINDOW* tech_display, struct TechTree* tech){
    for(int i = 0, j = 1; i < sub_tech_tree_size; j += (9 + strlen(tech->rocketry_nodes[i++].tech_bonus_name))){
        struct Tech_Data node = tech->rocketry_nodes[i];
        char* printing = node.tech_bonus_name;


        //print the middle of the tech
        mvwprintw(tech_display, 2, j, ".");
        if(node.science_paid >= node.science_to_complete){
            wattron(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        }
        if(i == tech->current_rocketry_tech_node){
            wattron(tech_display, COLOR_PAIR(CURRENT_RESEARCH));
        }

        mvwprintw(tech_display, 2, j + 2, "%s", printing);

        wattroff(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        wattroff(tech_display, COLOR_PAIR(CURRENT_RESEARCH));

        mvwprintw(tech_display, 2, j + 3 + strlen(printing), ".");
        if(i < sub_tech_tree_size - 1){
        mvwprintw(tech_display, 2, j + 5 + strlen(printing), "-->");
        }
        

        for(int borderx = 0; borderx < strlen(printing) + 4; borderx++){
            mvwprintw(tech_display, 1, borderx + j, ".");
            mvwprintw(tech_display, 3, borderx + j, ".");
        }
    }
        
     for(int i = 0, j = 1; i < sub_tech_tree_size; j += (9 + strlen(tech->uranium_nodes[i++].tech_bonus_name))){
        struct Tech_Data node = tech->uranium_nodes[i];
        char* printing = node.tech_bonus_name;


        //print the middle of the tech
        mvwprintw(tech_display, 2 + 4, j, ".");
        if(node.science_paid >= node.science_to_complete){
            wattron(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        }
        if(i == tech->current_rocketry_tech_node){
            wattron(tech_display, COLOR_PAIR(CURRENT_RESEARCH));
        }

        //print the name of a tech
        mvwprintw(tech_display, 2 + 4, j + 2, "%s", printing);

        //turn the colors off
        wattroff(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        wattroff(tech_display, COLOR_PAIR(CURRENT_RESEARCH));

        mvwprintw(tech_display, 2 + 4, j + 3 + strlen(printing), ".");
        if(i < sub_tech_tree_size - 1){
        mvwprintw(tech_display, 2 + 4, j + 5 + strlen(printing), "-->");
        }
        

        for(int borderx = 0; borderx < strlen(printing) + 4; borderx++){
            mvwprintw(tech_display, 1 + 4, borderx + j, ".");
            mvwprintw(tech_display, 3 + 4, borderx + j, ".");
        }
    }
    
}

void draw_resources(WINDOW* resource_display, struct PlayerData* player){
    wattron(resource_display, COLOR_PAIR(FAITH_DISPLAY));
    mvwprintw(resource_display, 1, 1, "Faith: %d, %d/turn", player->total_faith, player->faith_per_turn);
    wattroff(resource_display, COLOR_PAIR(FAITH_DISPLAY));

    wattron(resource_display, COLOR_PAIR(PRODUCTION_DISPLAY));
    mvwprintw(resource_display, 1, resource_display->_maxx/2 - 8, "Production: %d/turn", player->production_per_turn);
    wattroff(resource_display, COLOR_PAIR(PRODUCTION_DISPLAY));

    wattron(resource_display, COLOR_PAIR(SCIENCE_DISPLAY));
    mvwprintw(resource_display, 1, resource_display->_maxx - 18, "Science: %d/turn", player->science_per_turn);
    wattroff(resource_display, COLOR_PAIR(SCIENCE_DISPLAY));
}

void draw_tile(WINDOW* map_display, int win_block_row, int win_block_col, struct MapData* map, int map_row, int map_col){
    int box_width = 5;
    int box_height = 5;
 
    for(int i = 0; i < box_width; i++){

        mvwprintw(map_display, 1 + (win_block_row * (box_height - 1)), 1 + i + (win_block_col * (box_width - 1)), "-");
        //mvwprintw(map_display, ((win_block_row + 1) * (box_height), 1 + i + (win_block_col * (box_width - 1)), "#");
    }
    for(int i = 1; i < (box_height - 1); i++){

        mvwprintw(map_display, 1 + i + (win_block_row * (box_height - 1)), 1 + (win_block_col * (box_width - 1)), "|");
        //mvwprintw(map_display, 1 + i + (win_block_row * box_height), ((win_block_col + 1) * box_width), "#");

    }

    
}

void draw_map(WINDOW* map_display, int top_row, int leftmost_column, struct MapData* map){
    //int cells = map_display->_maxx;
    //map_display->_maxy;
    for(int i = 0; i < (map_display->_maxx - 2)/4 + 1; i++){
        for(int j = 0; j < (map_display->_maxy - 2)/4 + 1; j++){
            draw_tile(map_display, j, i, NULL, 1, 1);
        }
    }
}

void update_all(struct Windows* displays, struct PlayerData* player, struct MapData* map){
    
    
    draw_tech(displays->tech_display, player->techtree);
    draw_resources(displays->resource_display, player);
    draw_map(displays->main_display, displays->top_row, displays->leftmost_column, map);
    
    wborder(displays->tech_display, '|', '|', '-', '-', '/', '\\', '\\', '/');
    wborder(displays->resource_display, '|', '|', '-', '-', '*', '*', '*', '*');
    wborder(displays->main_display, '|', '|', '-', '-', '/', '\\', '\\', '/');

    refresh();

    wrefresh(displays->tech_display);
    wrefresh(displays->resource_display);
    wrefresh(displays->main_display);
}
int main(){
    initscr();
    raw();
    start_color();

    struct TechTree* tech = calloc(1, sizeof(struct TechTree));
    tech->current_rocketry_tech_node = 3;
    tech->current_uranium_tech_node = 0;
    
    for(int i = 0; i < sub_tech_tree_size; i++){
        tech->rocketry_nodes[i].tech_bonus_name = "Rockets";
        tech->rocketry_nodes[i].science_to_complete = 10;
        tech->rocketry_nodes[i].science_paid = 12 - i;
        tech->uranium_nodes[i].tech_bonus_name = "Uranium";
        tech->uranium_nodes[i].science_to_complete = 10;
        tech->uranium_nodes[i].science_paid = 12 - i;
    }

    struct PlayerData* player = calloc(1, sizeof(struct PlayerData));
    player->techtree = tech;

    struct Windows* displays = malloc(sizeof(struct Windows));
    setup_windows(displays);
    update_all(displays, player, NULL);
    
    /*
    WINDOW *windowmap = newwin(10, 10, 10, 10);
    wborder(windowmap, '|', '|', '-', '-', '+', '+', '+', '+');
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    for(int j = 0; j < 100; j++){
        if(!(j % 4)){
        for(int i = 0; i < 101; i++){
            if(i % 2){
                wattron(windowmap, COLOR_PAIR(1));
                mvwprintw(windowmap,0, 0, ".");
                mvwprintw(windowmap,1, 1, ".");
                wattroff(windowmap ,COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
                mvprintw(j, i, ".");
                attroff(COLOR_PAIR(2));
            }
         }
        } else {
            for(int i = 0; i < 21; i++){
                if((j + i) % 2){
                    attron(COLOR_PAIR(1));
                    mvprintw(j, i * 5, ".");
                    attroff(COLOR_PAIR(1));
                } else {
                    attron(COLOR_PAIR(2));
                    mvprintw(j, i * 5, ".");
                    attroff(COLOR_PAIR(2));
                }
            }
        }
    }
    
    refresh();
    wrefresh(windowmap);
    */
    getch();


    endwin();
}
