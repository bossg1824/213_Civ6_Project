#include <string.h>
#include <ncurses.h>
#include "civ.h"
//#include "game_initialize.c"
 
#define max_height_displayed 90 
#define max_width_displayed 30
#define TECH_WINDOW_SIZE 9
#define TILE_WIDTH 5
#define TILE_HEIGHT 5
#define NUM_PLAYER_COLORS 2
#define PLAYER_COLOR_OFFSET 1

bool colors_initialized = false;
bool curses_initialized = false;

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
    FOOD,
    IRON_GRAY,
    NITER_WHITE,
    ALUMINUM_GRAY,
    URANIUM_GREEN,
    CAMPUS_COLOR,
    AQUEDUCT_COLOR,
    MINE_COLOR,
};

void setup_colors(){
    if(colors_initialized) return;

    

    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_GREEN, 100, 700, 100);
    init_color(COLOR_CYAN, 0, 900, 900);
    init_color(COLOR_YELLOW, 900, 650, 0);
    init_color(COLOR_MAGENTA, 900, 0, 900);
    init_color(8, 0, 0, 500);//ocean blues
    init_color(9, 0, 0, 1000);//lake blue
    init_color(10, 750, 750, 1000);//river blue
    init_color(11, 500, 1000, 500);//flat green
    init_color(12, 1000, 500, 0);//hill brown
    init_color(13, 500, 500, 500);//mountain grey
    init_color(14, 965, 855, 395);//food
    init_color(15, 830, 840, 850);//Iron gray
    init_color(16, 688, 867, 965);//Niter
    init_color(17, 516, 527, 535);//Aluminum gray
    init_color(18, 234, 1000, 286);//Uranium green
    init_color(19, 800, 0, 800);//Campus color
    init_color(20, 300, 600, 300);//Aqueduct color
    init_color(21, 600, 600, 900);//Mine color

    init_pair(COMPLETED_RESEARCH, COLOR_GREEN, COLOR_BLACK);
    init_pair(CURRENT_RESEARCH, COLOR_CYAN, COLOR_BLACK);
    init_pair(FAITH_DISPLAY, COLOR_CYAN, COLOR_BLACK);
    init_pair(PRODUCTION_DISPLAY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SCIENCE_DISPLAY, COLOR_MAGENTA, COLOR_BLACK);

    init_pair(OCEAN_TILE, 8, COLOR_BLACK); //ocean
    init_pair(LAKE_TILE, 9, COLOR_BLACK); //lake
    init_pair(RIVER_TILE, 10, COLOR_BLACK);//river
    init_pair(FLAT_TILE, 11, COLOR_BLACK);//flat
    init_pair(HILL_TILE, 12, COLOR_BLACK);//hill
    init_pair(MOUNTAIN_TILE, 13, COLOR_BLACK);//mountain

    init_pair(FOOD, 14, COLOR_BLACK);

    init_pair(IRON_GRAY, 15, COLOR_BLACK);
    init_pair(NITER_WHITE, 16, COLOR_BLACK);
    init_pair(ALUMINUM_GRAY, 17, COLOR_BLACK);
    init_pair(URANIUM_GREEN, 18, COLOR_BLACK);
    init_pair(CAMPUS_COLOR, 19, COLOR_BLACK);
    init_pair(AQUEDUCT_COLOR, 20, COLOR_BLACK);
    init_pair(MINE_COLOR, 21, COLOR_BLACK);

    colors_initialized = true;
}

void setup_curses(){
    if(curses_initialized) return;
    initscr();               // Initialize ncurses
    start_color();           // Initialize color functionality
    raw();                   // Disable line buffering
    noecho();                // Don't echo user input
    curs_set(0);
    keypad(stdscr, true);
    mousemask(BUTTON1_PRESSED, NULL);
}

void setup_windows(struct Windows* displays){
    setup_curses();
    
    displays->resource_display = newwin(3, COLS, 0, 0);
    displays->main_display = newwin((LINES - 3 - TECH_WINDOW_SIZE), COLS, 3, 0);
    displays->tech_display = newwin(TECH_WINDOW_SIZE, COLS, (LINES - TECH_WINDOW_SIZE), 0);
    displays->top_row = 0;
    displays->leftmost_column = 0;

    
    setup_colors();
}

void destroy_windows(struct Windows* displays){
    delwin(displays->resource_display);
    delwin(displays->main_display);
    delwin(displays->tech_display);
}

void draw_tech(WINDOW* tech_display, struct TechTree* tech){
    if(tech == NULL){
        return;
    }
    
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

int player_color_num(struct PlayerData_List * players, int player_id){
    int pos = pos_in_list(players, player_id);
    if(pos == default_int){
        return default_int;
    }
    return (pos % NUM_PLAYER_COLORS) + PLAYER_COLOR_OFFSET;
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
        return 'p';
    default:
        return default_int;
    }
}

int resource_color_num(int resource_num){
    switch (resource_num)
    {
        case none_bonus:
            return default_int;
        case iron:
            return IRON_GRAY;
        case niter:
            return NITER_WHITE;
        case aluminum:
            return ALUMINUM_GRAY;
        case uranium:
            return URANIUM_GREEN;
        default:
            return default_int;
    }
}

char resource_char(int resource_num){
    switch (resource_num)
    {
        case none_bonus:
            return default_int;
        case iron:
            return 'I';
        case niter:
            return 'N';
        case aluminum:
            return 'A';
        case uranium:
            return 'U';
        default:
            return default_int;
    }
}

int district_color_num(struct TileData * tile, struct PlayerData_List* players){
    if(tile->buildable_structure == NULL){
        return default_int;
    }

    switch((tile->buildable_structure->district.district_type)){
        case none_district:
            return default_int;
        case campus:
            return CAMPUS_COLOR;
        case industrial_zone:
            return PRODUCTION_DISPLAY;
        case holy_site:
            return FAITH_DISPLAY;
        case aquaduct:
            return AQUEDUCT_COLOR;
        case mine:
            return MINE_COLOR;
        case farm:
            return FOOD;
        case city:
            return player_color_num(players, tile->civ_id_controlling);
        default:
            return default_int;
    }
}

char district_char(struct TileData * tile){
    if(tile->buildable_structure == NULL){
        return default_int;
    }

    switch((tile->buildable_structure->district.district_type)){
        case none_district:
            return default_int;
        case campus:
            return 'C';
        case industrial_zone:
            return 'I';
        case holy_site:
            return 'H';
        case aquaduct:
            return 'a';
        case mine:
            return 'm';
        case farm:
            return 'f';
        case city:
            return '^';
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

        int player_a_color = player_color_num(players, player_a_id);
        int player_b_color = player_color_num(players, player_b_id);
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

        int player_a_color = player_color_num(players, player_a_id);
        int player_b_color = player_color_num(players, player_b_id);

        

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
    //if the given map column is out of bounds end this function
    if(map_col >= tile_cols || map_col < 0){
        return;
    }
    //if the given map row is out of bounds end this function
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

    //storing the tile we are drawing
    struct TileData cur_tile = map->tiles[map_row][map_col];
    //getting color and char to display cur tile terrain
    int tilecolor = terrain_color_num(cur_tile.tiletype);
    char tilechar = terrain_char(cur_tile.tiletype);

    //setting the color to the color of terrain to print out and making it dim
    wattron(map_display, COLOR_PAIR(tilecolor));
    wattron(map_display, A_DIM);

    //printing the tiles that are always the terrain specifiers
    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%c", tilechar);
    
    //getting what the resource is
    int res_color_num = resource_color_num(cur_tile.resource); 
    //if there is a resource
    if(res_color_num != default_int){
        //make the color the resource color and make it not dim
        wattron(map_display, COLOR_PAIR(res_color_num));
        wattroff(map_display, A_DIM);
    } 
    //get the char for the resource
    char tile_res = resource_char(cur_tile.resource);
    //print the resource segment of the tile
    mvwprintw(map_display, 1 + 3 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%c", tile_res != default_int ? tile_res : tilechar);

    //if there was a resource
    if(res_color_num != default_int){
        //change color and dim attributes back to the defaul terrain vals
        wattroff(map_display, COLOR_PAIR(res_color_num));
        wattron(map_display, A_DIM);
        wattron(map_display, COLOR_PAIR(tilecolor));
    }

    //getting what is built on the tile
    int district_col_num = district_color_num(&(cur_tile), players);
    char dist_char = district_char(&(cur_tile));
    if(district_col_num != default_int){
        wattron(map_display, COLOR_PAIR(district_col_num));
        wattroff(map_display, A_DIM);
        if(dist_char == '^'){
            wattron(map_display, A_BOLD);
        }
    }

    mvwprintw(map_display, 1 + 2 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 2 + (win_block_col * (TILE_WIDTH - 1)), "%c", dist_char != default_int ? dist_char : tilechar);

    if(district_col_num != default_int){
        wattroff(map_display, COLOR_PAIR(district_col_num));
        wattroff(map_display, A_BOLD);
        wattron(map_display, A_DIM);
    }
    //turn of the default tile color
    wattroff(map_display, COLOR_PAIR(tilecolor));
    
    //turn on the production color and print the val to the left corner of the tile
    wattron(map_display, COLOR_PAIR(PRODUCTION_DISPLAY));
    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 1 + (win_block_col * (TILE_WIDTH - 1)), "%d", cur_tile.production);
    //turn off the production color
    wattroff(map_display, COLOR_PAIR(PRODUCTION_DISPLAY));

    //turn on the food color and print the val to the right corner of the tile
    wattron(map_display, COLOR_PAIR(FOOD));
    mvwprintw(map_display, 1 + 1 + (win_block_row * (TILE_HEIGHT - 1)), 1 + 3 + (win_block_col * (TILE_WIDTH - 1)), "%d", cur_tile.food);
    //turn off the food color
    wattroff(map_display, COLOR_PAIR(FOOD));

    //turn off the dimming
    wattroff(map_display, A_DIM);

    
}

void draw_map(WINDOW* map_display, int top_row, int leftmost_column, struct MapData* map, struct PlayerData_List* players){
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

int display_loop(struct PlayerData * player, struct MapData* map, struct PlayerData_List * players){

    struct Windows* displays = malloc(sizeof(struct Windows));
    setup_windows(displays);
    update_all(displays, player, map, players);
   
   int topmost;
   int leftmost;
   bool running = true;
   MEVENT mouse_event;
   while (running)
   {
    switch (getch())
    {
    case 'q':
        running = false;
        break;
    case 'w':
    case KEY_UP:
        if(displays->top_row > 0){
            displays->top_row--;
        }
        break;
        
    case 'a':
    case KEY_LEFT:
        if(displays->leftmost_column > 0){
            displays->leftmost_column--;
        }
        break;
    case 's':
    case KEY_DOWN:
        if(displays->top_row < tile_rows - (displays->main_display->_maxy/(TILE_HEIGHT - 1))){
            displays->top_row++;
        }
        break;
    case 'd':
    case KEY_RIGHT:
        if(displays->leftmost_column < tile_cols - (displays->main_display->_maxx/(TILE_WIDTH - 1))){
            displays->leftmost_column++;
        }
        break;
    case KEY_RESIZE:
        topmost = displays->top_row;
        leftmost = displays->leftmost_column;
        
        destroy_windows(displays);
        setup_windows(displays);

        displays->top_row = topmost;
        displays->leftmost_column = leftmost;
        break;
    case KEY_MOUSE:
        if(getmouse(&mouse_event) == OK){
            if(mouse_event.bstate & BUTTON1_PRESSED){
                //if pressing on the info display
                if(mouse_event.y <= 3){
                    //do nothing
                    break;
                } 
                //if clicking in the tech window
                if(mouse_event.y >= LINES - TECH_WINDOW_SIZE){
                    break;
                }
                //if clicking on the main diplsay
                if(mouse_event.y > 3 && mouse_event.y < LINES - TECH_WINDOW_SIZE && mouse_event.x > 1 && mouse_event.x < COLS){
                    //setup space for storing a tile coordinate
                    struct Tile_Coord * tile_location = calloc(1, sizeof(struct Tile_Coord));
                    //calculate the x and y coords of the tile
                    tile_location->x = displays->leftmost_column + (mouse_event.x - 1)/(TILE_WIDTH - 1);
                    tile_location->y = displays->top_row + (mouse_event.y - 3)/(TILE_HEIGHT - 1);
                    //if clicked beyond the tiles of the board act like you clicked the edge of the board instead
                    if(tile_location->x >= tile_cols){
                        tile_location->x = tile_cols - 1;
                    }
                    if(tile_location->y >= tile_rows){
                        tile_location->y = tile_rows - 1;
                    }
                    break;
                }
            }
        }
    default:
        break;
    }

    update_all(displays, player, map, players);
   }
   

    destroy_windows(displays);
    free(displays);
    endwin();
    return 1;
}