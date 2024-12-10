#include <string.h>
#include <ncurses.h>
#include "civ.h"
#include "game_initialize.c"

#define max_height_displayed 90 
#define max_width_displayed 30
#define TECH_WINDOW_SIZE 9
#define TILE_WIDTH 5
#define TILE_HEIGHT 5
#define NUM_PLAYER_COLORS 2
#define PLAYER_COLOR_OFFSET 1

bool colors_initialized = false;

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
    SCIENCE_DISPLAY,
    OCEAN_TILE,
    LAKE_TILE,
    MOUNTAIN_TILE,
    RIVER_TILE,
    FLAT_TILE,
    HILL_TILE,
    FOOD
};

void setup_colors(){
    if(colors_initialized) return;

    init_pair(COMPLETED_RESEARCH, COLOR_GREEN, COLOR_BLACK);
    init_pair(CURRENT_RESEARCH, COLOR_CYAN, COLOR_BLACK);
    init_pair(FAITH_DISPLAY, COLOR_CYAN, COLOR_BLACK);
    init_pair(PRODUCTION_DISPLAY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SCIENCE_DISPLAY, COLOR_MAGENTA, COLOR_BLACK);

    init_color(8, 0, 0, 500);//ocean blue
    init_color(9, 0, 0, 1000);//lake blue
    init_color(10, 750, 750, 1000);//river blue
    init_color(11, 500, 1000, 500);//flat green
    init_color(12, 1000, 500, 0);//hill brown
    init_color(13, 500, 500, 500);//mountain grey
    init_color(14, 965, 855, 395);

    init_pair(OCEAN_TILE, 8, COLOR_BLACK); //ocean
    init_pair(LAKE_TILE, 9, COLOR_BLACK); //lake
    init_pair(RIVER_TILE, 10, COLOR_BLACK);//river
    init_pair(FLAT_TILE, 11, COLOR_BLACK);//flat
    init_pair(HILL_TILE, 12, COLOR_BLACK);//hill
    init_pair(MOUNTAIN_TILE, 13, COLOR_BLACK);//mountain

    init_pair(FOOD, 14, COLOR_BLACK);

    colors_initialized = true;
}

void setup_windows(struct Windows* displays){
    initscr();               // Initialize ncurses
    start_color();           // Initialize color functionality
    raw();                   // Disable line buffering
    noecho();                // Don't echo user input
    curs_set(0);

    displays->resource_display = newwin(3, COLS, 0, 0);
    displays->main_display = newwin((LINES - 3 - TECH_WINDOW_SIZE), COLS, 3, 0);
    displays->tech_display = newwin(TECH_WINDOW_SIZE, COLS, (LINES - TECH_WINDOW_SIZE), 0);
    displays->top_row = 0;
    displays->leftmost_column = 0;

    setup_colors();
}

void draw_tech(WINDOW* tech_display, struct TechTree* tech){
    for(int i = 0, j = 1; i < sub_tech_tree_size; j += (9 + strlen(tech->rocketry_nodes[i++].tech_bonus_name))){
        //printing rocketry tech
        struct Tech_Data node = tech->rocketry_nodes[i];
        char* printing = node.tech_bonus_name;


        //print the middle of the tech
        mvwprintw(tech_display, 2, j, ".");
        //if the tech is completed
        if(node.science_paid >= node.science_to_complete){
            wattron(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        }
        //if the tech is the currently researching one
        if(i == tech->current_rocketry_tech_node){
            wattron(tech_display, COLOR_PAIR(CURRENT_RESEARCH));
        }

        //print the name of a tech
        mvwprintw(tech_display, 2, j + 2, "%s", printing);

        //turn the colors off
        wattroff(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        wattroff(tech_display, COLOR_PAIR(CURRENT_RESEARCH));
        
        //print the right end of the box
        mvwprintw(tech_display, 2, j + 3 + strlen(printing), ".");
        //if not the last element in the subtech 'tree'
        if(i < sub_tech_tree_size - 1){
        mvwprintw(tech_display, 2, j + 5 + strlen(printing), "-->");
        }
        
        //print the top and bottom dots surrounding the tech
        for(int borderx = 0; borderx < strlen(printing) + 4; borderx++){
            mvwprintw(tech_display, 1, borderx + j, ".");
            mvwprintw(tech_display, 3, borderx + j, ".");
        }
    }
        
     for(int i = 0, j = 1; i < sub_tech_tree_size; j += (9 + strlen(tech->uranium_nodes[i++].tech_bonus_name))){
        //printing uranium tech
        struct Tech_Data node = tech->uranium_nodes[i];
        char* printing = node.tech_bonus_name;


        //print the middle of the tech
        mvwprintw(tech_display, 2 + 4, j, ".");
        //if the tech is completed
        if(node.science_paid >= node.science_to_complete){
            wattron(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        }
        //if the tech is the currently researching one
        if(i == tech->current_rocketry_tech_node){
            wattron(tech_display, COLOR_PAIR(CURRENT_RESEARCH));
        }

        //print the name of a tech
        mvwprintw(tech_display, 2 + 4, j + 2, "%s", printing);

        //turn the colors off
        wattroff(tech_display, COLOR_PAIR(COMPLETED_RESEARCH));
        wattroff(tech_display, COLOR_PAIR(CURRENT_RESEARCH));

        //print the right end of the box
        mvwprintw(tech_display, 2 + 4, j + 3 + strlen(printing), ".");
        //if not the last element in the tech 'tree'
        if(i < sub_tech_tree_size - 1){
        mvwprintw(tech_display, 2 + 4, j + 5 + strlen(printing), "-->");
        }
        
        //print the bottom and top dots surrounding the tech
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

int pos_in_list(struct PlayerData_List * list, int player_id){
    struct PlayerData_Node * cur = list->head;
    for(int i = 0; i < list->length && cur != NULL; i++){
        if(cur->data.player_id == player_id){
            return i + 1;
        }
        cur = cur->next;
    }
    return -1;
}

int terrain_color_num(int terrain_num){
    switch (terrain_num)
    {
    case ocean:
        return OCEAN_TILE;
    case lake:
        return LAKE_TILE;
    case mountain:
        return MOUNTAIN_TILE;
    case river:
        return RIVER_TILE;
    case hill:
        return HILL_TILE;
    case flat:
        return FLAT_TILE;
    default:
        return default_int;
    }
}

char terrain_char(int terrain_num){
    switch (terrain_num)
    {
    case ocean:
        return 'o';
    case lake:
        return 'l';
    case mountain:
        return 'm';
    case river:
        return 'r';
    case hill:
        return 'h';
    case flat:
        return 'f';
    default:
        return default_int;
    }
}

void draw_edge(WINDOW* map_display, int win_block_row, int win_block_col, struct MapData*map, int map_row, int map_col, struct PlayerData_List* players, bool horizontal){
   
    if(map_row >= tile_rows){
        return;
    }

    if(map_col >= tile_cols){
        return;
    }

    int player_a_id = default_int;
    int player_b_id = default_int;
    

    if(horizontal){
        //if the tile above the edge is in bounds
        if((map_row - 1) >= 0){
            //set the player a id to the player id in the above tile
            player_a_id = map->tiles[(map_row - 1)][map_col].civ_id_controlling;
        } 
        //if the tile below the edge is in bounds
        if((map_row) < tile_cols){
            //set the player id to the player id in the below tile
            player_b_id = map->tiles[map_row][map_col].civ_id_controlling;
        }
        
        

        //if the id's are the same (either no civ or the same civ shares the border)
        if(player_a_id == player_b_id){
            //make the edge dim
            wattron(map_display, A_DIM);
            
        } else {
            wattron(map_display, A_BOLD);
        }

        int player_a_color = (pos_in_list(players, player_a_id) % NUM_PLAYER_COLORS) + PLAYER_COLOR_OFFSET;
        int player_b_color = (pos_in_list(players, player_b_id) % NUM_PLAYER_COLORS) + PLAYER_COLOR_OFFSET;
        //if a is a civ and b is not
        if(player_a_id != default_int && player_b_id == default_int){
            //turn on a's color
            wattron(map_display, COLOR_PAIR(player_a_color));
        }


        //if b is a civ and a is not
        if(player_a_id == default_int && player_b_id != default_int){
            //turn on b's color
            wattron(map_display, COLOR_PAIR(player_b_color));
        }

        //for the middle spaces of the box edge
        for(int i = 1; i < TILE_WIDTH - 1; i++){
                //if both a and b are civs that are different
                if(player_a_id != default_int && player_b_id != default_int && (player_a_id != player_b_id)){
                    //alternate every square
                    if(i % 2){
                        //turn on a's color
                        wattron(map_display, COLOR_PAIR(player_a_color));
                    } else {
                        //turn on b's color
                        wattron(map_display, COLOR_PAIR(player_b_color));
                    }
                }
                //print out the correct color '-'
                mvwprintw(map_display, 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + i + (win_block_col * (TILE_WIDTH - 1)), "-");

                //if both a and b are civs that are different
                if(player_a_id != default_int && player_b_id != default_int && (player_a_id != player_b_id)){
                    //turn off both a and b's color
                    wattroff(map_display, COLOR_PAIR(player_a_color));
                    wattroff(map_display, COLOR_PAIR(player_b_color));
                }
        }

        wattroff(map_display, A_DIM);
        wattroff(map_display, A_BOLD);
        wattroff(map_display, COLOR_PAIR(player_a_color));
        wattroff(map_display, COLOR_PAIR(player_b_color));

    } else {
        if((map_col - 1) >= 0){
            player_a_id = map->tiles[map_row][(map_col - 1)].civ_id_controlling;
        }
        if((map_col) < tile_cols){
            player_b_id = map->tiles[map_row][map_col].civ_id_controlling;
        }

        int player_a_color = (pos_in_list(players, player_a_id) % NUM_PLAYER_COLORS) + PLAYER_COLOR_OFFSET;
        int player_b_color = (pos_in_list(players, player_b_id) % NUM_PLAYER_COLORS) + PLAYER_COLOR_OFFSET;

        

        if(player_a_id == player_b_id){
            wattron(map_display, A_DIM);
        } else {
            wattron(map_display, A_BOLD);
        }

        if(player_a_id != default_int && player_b_id == default_int){
            wattron(map_display, COLOR_PAIR(player_a_color));
        }

        if(player_a_id == default_int && player_b_id != default_int){
            wattron(map_display, COLOR_PAIR(player_b_color));
        }

        
        for(int i = 1; i < TILE_HEIGHT - 1; i++){
            int two_player_color;
                if(player_a_id != default_int && player_b_id != default_int && (player_a_id != player_b_id)){
                    if(i % 2){
                        wattron(map_display, COLOR_PAIR(player_a_color));
                    } else {
                        wattron(map_display, COLOR_PAIR(player_b_color));
                    }
                }
                mvwprintw(map_display, 1 + i + (win_block_row * (TILE_HEIGHT - 1)), 1 + (win_block_col * (TILE_WIDTH - 1)), "|");

                if(player_a_id != default_int && player_b_id != default_int && (player_a_id != player_b_id)){
                    wattroff(map_display, COLOR_PAIR(player_a_color));
                    wattroff(map_display, COLOR_PAIR(player_b_color));
                }
        }

        wattroff(map_display, A_DIM);
        wattroff(map_display, A_BOLD);
        wattroff(map_display, COLOR_PAIR(player_a_color));
        wattroff(map_display, COLOR_PAIR(player_b_color));
    }
}

void draw_tile(WINDOW* map_display, int win_block_row, int win_block_col, struct MapData* map, int map_row, int map_col, struct PlayerData_List* players){
    if(map_col >= tile_cols || map_col < 0){
        return;
    }
    if(map_row >= tile_rows || map_row < 0){
        return;
    }

    //left edge of tile
    draw_edge(map_display, win_block_row, win_block_col, map, map_row, map_col, players, false);
    //right edge of tile
    draw_edge(map_display, win_block_row , win_block_col + 1, map, map_row, map_col + 1, players, false);
    //top edge of tile
    draw_edge(map_display, win_block_row, win_block_col, map, map_row, map_col, players, true);
    //bottom edge of tile
    draw_edge(map_display, win_block_row + 1, win_block_col, map, map_row + 1, map_col, players, true);
    struct TileData cur_tile = map->tiles[map_row][map_col];
    int tilecolor = terrain_color_num(cur_tile.tiletype);
    char tilechar = terrain_char(cur_tile.tiletype);

    wattron(map_display, COLOR_PAIR(tilecolor));
    wattron(map_display, A_DIM);

    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    if(false){

    } 
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    wattroff(map_display, COLOR_PAIR(tilecolor));
    
    wattron(map_display, COLOR_PAIR(PRODUCTION_DISPLAY));
    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%d", cur_tile.production);
    wattroff(map_display, COLOR_PAIR(PRODUCTION_DISPLAY));

    wattron(map_display, COLOR_PAIR(FOOD));
    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%d", cur_tile.food);
    wattroff(map_display, COLOR_PAIR(FOOD));

    wattroff(map_display, A_DIM);

    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%d", map->tiles[map_row][map_col].civ_id_controlling);
}

void draw_map(WINDOW* map_display, int top_row, int leftmost_column, struct MapData* map, struct PlayerData_List* players){
    //int cells = map_display->_maxx;
    //map_display->_maxy;
    for(int i = 0; i < (map_display->_maxx - 2)/(TILE_HEIGHT - 1) + 1; i++){
        for(int j = 0; j < (map_display->_maxy - 2)/(TILE_WIDTH - 1) + 1; j++){
            draw_tile(map_display, j, i, map, top_row + j, leftmost_column + i, players);
        }
    }
}

void update_all(struct Windows* displays, struct PlayerData* player, struct MapData* map, struct PlayerData_List* players){
    
    
    draw_tech(displays->tech_display, player->techtree);
    draw_resources(displays->resource_display, player);
    draw_map(displays->main_display, displays->top_row, displays->leftmost_column, map, players);
    
    wborder(displays->tech_display, '|', '|', '-', '-', '/', '\\', '\\', '/');
    wborder(displays->resource_display, '|', '|', '-', '-', '*', '*', '*', '*');
    wborder(displays->main_display, '|', '|', '-', '-', '/', '\\', '\\', '/');

    refresh();

    wrefresh(displays->tech_display);
    wrefresh(displays->resource_display);
    wrefresh(displays->main_display);
}

int main(){
    //initscr();
    //raw();
    //start_color();
    struct MapData  map_a = map_initialize_default();
    struct MapData * map = calloc(1, sizeof(struct MapData)); 
    map->cols = tile_cols;
    map->rows = tile_rows;
    
    for(int i = 0; i < map_a.rows; i++){
        for(int j = 0; j < map->cols; j++){
            struct TileData* tile = &(map_a.tiles[i][j]);
            //tile->tiletype = hill;
            tile->civ_id_controlling = j >= tile_cols/2 ? 0:1;
        }   
    }

    struct PlayerData_List* players = calloc(1, sizeof(struct PlayerData_List));

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
    struct PlayerData* player2 = calloc(1, sizeof(struct PlayerData));
    player2->player_id = 1;
    player->techtree = tech;
    struct PlayerData_Node* player_node = calloc(1, sizeof(struct PlayerData_Node));
    struct PlayerData_Node* player_node2 = calloc(1, sizeof(struct PlayerData_Node));
    player_node->data = *player;
    player_node->next = player_node2;
    player_node2->data = *player2;
    player_node2->next = NULL;
    players->head = player_node;
    players->length = 2;
    struct Windows* displays = malloc(sizeof(struct Windows));
    setup_windows(displays);
    update_all(displays, player, &map_a, players);
    
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
