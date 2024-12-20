#ifndef _CIVILIZATION_STRUCTS_
#define _CIVILIZATION_STRUCTS_

#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>   
#include <sys/time.h> 
#include <time.h>     
#include <stdint.h>
#include <string.h> 

#define tile_cols 60
#define tile_rows 60
#define sub_tech_tree_size 5
#define num_players 4
#define map_seed 147

//note to self: touch this and lose a finger
#define default_int -1

//Enums needed for Map Data
enum BonusType{
none_bonus,
science,
faith,
production,
housing,
food
};

enum DistrictType{
none_district,
campus,
industrial_zone,
holy_site,
aquaduct,
mine,
farm,
city
};

//Thus 0-2 are not buildable
enum TileType {
ocean,
lake,
mountain,
river,
flat,
hill,
};

enum ResourceType {
none,
iron,
niter,
aluminum,
uranium
};

struct Tile_Coord {
    int x;
    int y;
};

struct Tile_Coord_Node {
    struct Tile_Coord data;
    struct Tile_Coord_Node* next;
};

struct Tile_Coord_List {
    struct Tile_Coord_Node* head;
    int length;
};

struct District{
    int current_level;
    int max_level;
    int production_cost_to_upgrade;
    enum DistrictType district_type;
};

//Note this is used in both Map and Playerdata (to see if city is stil producing something)
struct Buildable_Structure{
    bool is_empty;
    struct District district;
    enum BonusType bonus_type;
    int bonus_amount;
    int production_cost;
    int production_spent;
    bool completed;
    int estimated_turns_until_completion; 
    struct Tile_Coord coordinate;
    int build_id; //if we are upgrading a district we keep this in the completed list and create a copy in the current prod that will replace this
};

struct Buildable_Structure_Node {
    struct Buildable_Structure data;
    struct Buildable_Structure_Node* next;
};

struct Buildable_Structure_List {
    struct Buildable_Structure_Node* head;
    int length;
};

struct TileData{
    struct Tile_Coord coordinate;
    enum TileType tiletype;
    enum ResourceType resource;
    int civ_id_controlling;// -1 if no civ controlling
    int city_id_controlling;
    int production;
    int food;
    struct Buildable_Structure* buildable_structure;
};

struct MapData {
    int cols;
    int rows;
    //bool occupied_tiles[tile_rows][tile_cols];
    struct TileData tiles[tile_rows][tile_cols];
};

//We now begin structs needed for playerdata

struct Tech_Data{
char *tech_bonus_name;
int science_to_complete;
int science_paid;
};

struct TechTree{//Contains two arrays of tech tree nodes
    int current_rocketry_tech_node;
    int current_uranium_tech_node;
    struct Tech_Data rocketry_nodes[sub_tech_tree_size];     
    struct Tech_Data uranium_nodes[sub_tech_tree_size];  
};

struct City{
    char* city_name;
    int city_id;
    struct Tile_Coord city_center_coord;
    int housing;
    int population;
    int food_per_tern;
    int food_accumulated;
    int settlers_produced; //every settler you make increases the prod cost of creating another settler
    bool focus_food;
    bool focus_production;
    struct Tile_Coord_List worked_tiles;
    struct Tile_Coord_List tiles_under_controll; //An array of the tiles that are under this city's controll
    struct Buildable_Structure current_structure_in_production; //The structure this city is currently putting its production towards
    struct Buildable_Structure_List built_structures; //Linked list of structures that have been produced in this city
};

struct City_Node {
    struct City data;
    struct City_Node* next;
};

struct City_List{//Linked list all cities from player
    struct City_Node* head;
    int length;
};

struct PlayerData{
    int player_id;
    char* civ_name;
    struct City_List cities;
    struct TechTree* techtree;
    int total_faith;
    int faith_per_turn;
    int science_per_turn;
    int production_per_turn;
};
 
struct PlayerData_Node {
    struct PlayerData data;
    struct PlayerData_Node* next;
};

struct PlayerData_List{//Linked list of all players
    struct PlayerData_Node* head;
    int length;
};

struct GameState {
int turn_count;
int turn_limit;
int player_turn;
struct MapData mymap;    
struct PlayerData_List players;
}; 

//calculate the size of a given GameState including all pointed to memory
size_t size_of_civ_state(const struct GameState * civ_state){
    //start with no size
    size_t civ_size = 0;

    //add the base size of a GameState struct
    civ_size += sizeof(struct GameState);
    //for each tile
    for(int i = 0; i < civ_state->mymap.rows; i++){
        for(int j = 0; j < civ_state->mymap.cols; j++){
            const struct TileData* cur = &(civ_state->mymap.tiles[i][j]);
            //if there exists a buldable structure
            if(cur->buildable_structure != NULL){
                //add the size of a buildable structure
                civ_size += sizeof(struct Buildable_Structure);
            }
        }
    }

    //for each player in the player list
    struct PlayerData_Node * travel = civ_state->players.head;
    //while a player exists
    while(travel != NULL){
        //add the base size of a player
        civ_size += sizeof(struct PlayerData_Node);
        //add the length of the player name
        civ_size += ((strlen(travel->data.civ_name) + 1) * sizeof(char));
        
        
        
        if(travel->data.techtree != NULL){
            //add the base size of a tech tree
            civ_size += sizeof(struct TechTree);

            //for each node of research in each tech tree
            for(int i = 0; i < sub_tech_tree_size; i++){
                //add the length of the tech name
                civ_size += ((strlen(travel->data.techtree->rocketry_nodes[i].tech_bonus_name) + 1) * sizeof(char));
                civ_size += ((strlen(travel->data.techtree->uranium_nodes[i].tech_bonus_name) + 1) * sizeof(char));
            }
        }
        //for each city the player owns
        struct City_Node * city_travel = travel->data.cities.head;
        //while a city exists
        while(city_travel != NULL){
            //add the base size of a city node
            civ_size += sizeof(struct City_Node);
            struct City visiting = city_travel->data;
            //add the length of the city name
            civ_size += ((strlen(visiting.city_name) + 1) * sizeof(char));

            //for each tile being worked in the city
            struct Tile_Coord_Node* worked = visiting.worked_tiles.head;
            //while a tile exists
            while(worked != NULL){
                //add the size of a tile node
                civ_size += sizeof(struct Tile_Coord_Node);

                //go to the next tile node
                worked = worked->next;
            }  

            //for each tile under city controll
            struct Tile_Coord_Node* controlled = visiting.tiles_under_controll.head;
            //while a tile exists
            while(controlled != NULL){
                //add the size of a tile node
                civ_size += sizeof(struct Tile_Coord_Node);

                //go to the next tile node
                controlled = controlled->next;
            }

            //for each buildable structure in the built list
            struct Buildable_Structure_Node* building = visiting.built_structures.head;
            //while there exists a buildable structure
            while(building != NULL){
                //add the size of a buildable structure
                civ_size += sizeof(struct Buildable_Structure_Node);

                //go to the next building
                building = building->next;
            }

            //go to the next city
            city_travel = city_travel->next;
        }

        //go to the next player
        travel = travel->next;
    }

    //return the calculated size
    return civ_size;
}

uint8_t * game_state_to_byte_array(const struct GameState * civ_state){
    uint8_t * byte_array = calloc(1, size_of_civ_state(civ_state));
    size_t pos = 0;

    memcpy(byte_array, civ_state, sizeof(struct GameState));
    pos += sizeof(struct GameState);

    for(int i = 0; i < civ_state->mymap.rows; i++){
        for(int j = 0; j < civ_state->mymap.cols; j++){
            const struct TileData* cur = &(civ_state->mymap.tiles[i][j]);
            //if there exists a buldable structure
            if(cur->buildable_structure != NULL){
                //copy the data into the byte array
                memcpy(byte_array + pos, cur->buildable_structure, sizeof(struct Buildable_Structure));
                pos += sizeof(struct Buildable_Structure);
            }
        }
    }

    //for each player in the player list
    struct PlayerData_Node * travel = civ_state->players.head;
    //while a player exists
    while(travel != NULL){
        //add  player
        memcpy(byte_array + pos, travel, sizeof(struct PlayerData_Node));
        pos += sizeof(struct PlayerData_Node);
        //add the player name
        char * civ_name = travel->data.civ_name;
        memcpy(byte_array + pos, civ_name, (strlen(civ_name) + 1) * sizeof(char));
        pos += ((strlen(civ_name) + 1) * sizeof(char));
        
        
        
        if(travel->data.techtree != NULL){
            //add the tech tree
            memcpy(byte_array + pos, travel->data.techtree, sizeof(struct TechTree));
            pos += sizeof(struct TechTree);

            //for each node of research in each tech tree
            for(int i = 0; i < sub_tech_tree_size; i++){
                //add the length of the tech name
                char * rocket_tech = travel->data.techtree->rocketry_nodes[i].tech_bonus_name;
                char * uranium_tech =travel->data.techtree->uranium_nodes[i].tech_bonus_name;

                memcpy(byte_array + pos, rocket_tech, (strlen(rocket_tech) + 1) * sizeof(char));
                pos += ((strlen(rocket_tech) + 1) * sizeof(char));

                memcpy(byte_array + pos, uranium_tech, (strlen(uranium_tech) + 1) * sizeof(char));
                pos += ((strlen(uranium_tech) + 1) * sizeof(char));
            }
        }
        //for each city the player owns
        struct City_Node * city_travel = travel->data.cities.head;
        //while a city exists
        while(city_travel != NULL){
            //add the city node
            memcpy(byte_array + pos, city_travel, sizeof(struct City_Node));
            pos += sizeof(struct City_Node);
            struct City visiting = city_travel->data;
            //add the city name
            memcpy(byte_array + pos, visiting.city_name, (strlen(visiting.city_name) + 1) * sizeof(char));
            pos += ((strlen(visiting.city_name) + 1) * sizeof(char));

            //for each tile being worked in the city
            struct Tile_Coord_Node* worked = visiting.worked_tiles.head;
            //while a tile exists
            while(worked != NULL){
                //add the tile node
                memcpy(byte_array + pos, worked, sizeof(struct Tile_Coord_Node));
                pos += sizeof(struct Tile_Coord_Node);

                //go to the next tile node
                worked = worked->next;
            }  

            //for each tile under city controll
            struct Tile_Coord_Node* controlled = visiting.tiles_under_controll.head;
            //while a tile exists
            while(controlled != NULL){
                //add the tile node
                memcpy(byte_array + pos, controlled, sizeof(struct Tile_Coord_Node));
                pos += sizeof(struct Tile_Coord_Node);

                //go to the next tile node
                controlled = controlled->next;
            }

            //for each buildable structure in the built list
            struct Buildable_Structure_Node* building = visiting.built_structures.head;
            //while there exists a buildable structure
            while(building != NULL){
                //add the buildable structure
                memcpy(byte_array + pos, building, sizeof(struct Buildable_Structure_Node));
                pos += sizeof(struct Buildable_Structure_Node);

                //go to the next building
                building = building->next;
            }

            //go to the next city
            city_travel = city_travel->next;
        }

        //go to the next player
        travel = travel->next;
    }
    return byte_array;
}

void print_game_state(struct GameState * game){
    printf("Player turn: %d\nTurn count: %d\nTurn limit: %d\n", game->player_turn, game->turn_count, game->turn_limit);
    for(int i = 0; i < game->mymap.rows; i++){
        for(int j = 0; j < game->mymap.cols; j++){
            struct TileData * tile = &(game->mymap.tiles[i][j]);    
            if(tile->buildable_structure != NULL){
                printf("%d", tile->buildable_structure->district.district_type);
            }
            printf("|%d,%d,%d,%d| ", tile->food, tile->production, tile->resource, tile->tiletype);
            }
        printf("\n");
    }
    printf("Length %d\n", game->players.length);
    struct PlayerData_Node * player_travel = game->players.head;
    while(player_travel != NULL){
        struct PlayerData data = player_travel->data;
        printf("Player Name : %s, fpt %d, pid %d, ppt %d, spt %d, tf %d\n", data.civ_name, 
                data.faith_per_turn, data.player_id, data.production_per_turn, data.science_per_turn, data.total_faith);
        printf("Num Cities: %d\n", data.cities.length);

        struct City_Node * city_travel = data.cities.head;
        while(city_travel != NULL){
            printf("Name: %s, ID %d", city_travel->data.city_name, city_travel->data.city_id);
            //printf(" %p\n", city_travel->data.built_structures.head);
            struct Buildable_Structure_Node * trav = city_travel->data.built_structures.head;
            while(trav != NULL){
                printf("%d -> ",trav->data.district.district_type);
                trav = trav->next;
            }
            printf("\n");
            /*
            struct Tile_Coord_Node * work = city_travel->data.worked_tiles.head;
            while(work != NULL){
                printf("|%d,%d|", work->data.x, work->data.y);
                work = work->next;
            }
            struct Tile_Coord_Node * owned = city_travel->data.tiles_under_controll.head;
            printf("Owned");
            while(owned != NULL){
                printf("|%d,%d|", owned->data.x, owned->data.y);
                owned = owned->next;
            */
            city_travel = city_travel->next;
        }
        player_travel = player_travel->next;
    }
}

//yes officer this function right here
struct GameState * byte_array_to_game_state(uint8_t * byte_array, size_t size){
    struct GameState * game = calloc(1, sizeof(struct GameState));
    size_t pos = 0;

    memcpy(game, byte_array, sizeof(struct GameState));
    pos += sizeof(struct GameState);

    for(int i = 0; i < game->mymap.rows; i++){
        for(int j = 0; j < game->mymap.cols; j++){
            struct TileData* cur = &(game->mymap.tiles[i][j]);
            //if there exists a buldable structure
            if(cur->buildable_structure != NULL){
                struct Buildable_Structure * building = calloc(1, sizeof(struct Buildable_Structure));
                cur->buildable_structure = building;
                //copy the data into the byte array
                memcpy(cur->buildable_structure, byte_array + pos, sizeof(struct Buildable_Structure));
                pos += sizeof(struct Buildable_Structure);
                
            }
        }
    }

    bool is_head_of_list = true;
    //for each player in the player list
    struct PlayerData_Node * travel = game->players.head;
    if(travel != NULL){
        struct PlayerData_Node * head = calloc(1, sizeof(struct PlayerData_Node));
        game->players.head = head;
        memcpy(game->players.head, byte_array + pos, sizeof(struct PlayerData_Node));
        pos += sizeof(struct PlayerData_Node);

        travel = head;
        struct PlayerData_Node * previous = travel;
        while(travel != NULL){

            if(!is_head_of_list){
                struct PlayerData_Node * moving = calloc(1, sizeof(struct PlayerData_Node));
                previous->next = moving;
                memcpy(moving, byte_array + pos, sizeof(struct PlayerData_Node));
                pos += sizeof(struct PlayerData_Node);
                travel = moving;
            } else {
                is_head_of_list = false;
            }
            int civ_name_length = (strlen((char *)(byte_array + pos)) + 1) * sizeof(char);
            char * civ_name = calloc(1, civ_name_length);
            travel->data.civ_name = civ_name;

            memcpy(civ_name, byte_array + pos, civ_name_length);
            pos += civ_name_length;

            if(travel->data.techtree != NULL){
                struct TechTree * tech = calloc(1, sizeof(struct TechTree));
                travel->data.techtree = tech;

                memcpy(tech, byte_array + pos, sizeof(struct TechTree));
                pos += sizeof(struct TechTree);

                for(int i = 0; i < sub_tech_tree_size; i++){
                    char * rocket_tech_name = calloc(1, (strlen((char *)(byte_array + pos)) + 1) * sizeof(char));
                    memcpy(rocket_tech_name, byte_array + pos, (strlen((char *)(byte_array + pos)) + 1) * sizeof(char));
                    pos += (strlen((char *)(byte_array + pos)) + 1) * sizeof(char);
                    tech->rocketry_nodes[i].tech_bonus_name = rocket_tech_name;

                    char * uranium_tech_name = calloc(1, (strlen((char *)(byte_array + pos)) + 1) * sizeof(char));
                    memcpy(uranium_tech_name, byte_array + pos, (strlen((char *) (byte_array + pos)) + 1) * sizeof(char));
                    pos += (strlen((char *)(byte_array + pos)) + 1)* sizeof(char);
                    tech->uranium_nodes[i].tech_bonus_name = uranium_tech_name;
                }
            }

            struct City_Node * city_travel = travel->data.cities.head;

            if(city_travel != NULL){
                struct City_Node * head = calloc(1, sizeof(struct City_Node));
                travel->data.cities.head = head;
                memcpy(head, byte_array + pos, sizeof(struct City_Node));
                pos += sizeof(struct City_Node);

                city_travel =  head;

                struct City_Node * prev = head;
                bool is_head_of_city_list = true;
                while(city_travel != NULL){
                    if(!is_head_of_city_list){
                        struct City_Node * cur = calloc(1, sizeof(struct City_Node));
                        memcpy(cur, byte_array + pos, sizeof(struct City_Node));
                        pos += sizeof(struct City_Node);

                        city_travel = cur;
                        prev->next = city_travel;
                    } else {
                        is_head_of_city_list = false;
                    }

                    char * city_name = calloc(1, (strlen((char *)(byte_array + pos)) + 1) * sizeof(char));
                    memcpy(city_name, byte_array + pos, (strlen((char *)(byte_array + pos)) + 1) * sizeof(char));
                    pos += (strlen((char *)(byte_array + pos)) + 1) * sizeof(char);

                    city_travel->data.city_name = city_name;


                    struct Tile_Coord_Node * worked = city_travel->data.worked_tiles.head;
                    if(worked != NULL){
                        struct Tile_Coord_Node * head = calloc(1, sizeof(struct Tile_Coord_Node));
                        memcpy(head, byte_array + pos, sizeof(struct Tile_Coord_Node));
                        pos += sizeof(struct Tile_Coord_Node);

                        city_travel->data.worked_tiles.head = head;
                        worked = head;
                        struct Tile_Coord_Node * prev_tile_worked = worked;

                        bool head_of_worked_list = true;
                        while(worked != NULL){
                            if(!head_of_worked_list){
                                struct Tile_Coord_Node * cur = calloc(1, sizeof(struct Tile_Coord_Node));
                                memcpy(cur, byte_array + pos, sizeof(struct Tile_Coord_Node));
                                pos += sizeof(struct Tile_Coord_Node);
                                worked = cur;
                                prev_tile_worked->next = worked;
                            } else {
                                head_of_worked_list = false;
                            }
                            prev_tile_worked = worked;
                            worked = worked->next;
                        }
                    }

                    struct Tile_Coord_Node * owned = city_travel->data.tiles_under_controll.head;
                    if(owned != NULL){
                        struct Tile_Coord_Node * head = calloc(1, sizeof(struct Tile_Coord_Node));
                        memcpy(head, byte_array + pos, sizeof(struct Tile_Coord_Node));
                        pos += sizeof(struct Tile_Coord_Node);

                        city_travel->data.tiles_under_controll.head = head;
                        owned = head;
                        struct Tile_Coord_Node * prev_tile_owned = owned;

                        bool head_of_owned_list = true;
                        while(owned != NULL){
                            if(!head_of_owned_list){
                                struct Tile_Coord_Node * cur = calloc(1, sizeof(struct Tile_Coord_Node));
                                memcpy(cur, byte_array + pos, sizeof(struct Tile_Coord_Node));
                                pos += sizeof(struct Tile_Coord_Node);
                                owned = cur;
                                prev_tile_owned->next = owned;
                            } else {
                                head_of_owned_list = false;
                            }
                            prev_tile_owned = owned;
                            owned = owned->next;
                        }
                    }
                    
                    struct Buildable_Structure_Node * building = city_travel->data.built_structures.head;
                    if(building != NULL){
                        struct Buildable_Structure_Node * head = calloc(1, sizeof(struct Buildable_Structure_Node));
                        memcpy(head, byte_array + pos, sizeof(struct Buildable_Structure_Node));
                        pos += sizeof(struct Buildable_Structure_Node);

                        city_travel->data.built_structures.head = head;
                        building = head;
                        struct Buildable_Structure_Node * prev_building = building;
                        bool head_of_building_list = true;
                        while(building != NULL){
                            if(!head_of_building_list){
                                struct Buildable_Structure_Node * cur = calloc(1, sizeof(struct Buildable_Structure_Node));
                                memcpy(cur, byte_array + pos, sizeof(struct Buildable_Structure_Node));
                                pos += sizeof(struct Buildable_Structure_Node);

                                building = cur;
                                prev_building->next = cur;
                            } else {
                                head_of_building_list = false;
                            }

                            prev_building = building;
                            building = building->next;
                        }
                    }

                    prev = city_travel;
                    city_travel = city_travel->next;
                }
            }
            previous = travel;
            travel = travel->next;
        }
        
    }
    return game;
}

int pos_in_list(struct PlayerData_List * list, int player_id){
    struct PlayerData_Node * cur = list->head;
    for(int i = 0; i < list->length && cur != NULL; i++){
        if(cur->data.player_id == player_id){
            return i + 1;
        }
        cur = cur->next;
    }
    return default_int;
}

int player_id_from_pos(struct PlayerData_List * list, int pos_in_list){
    if(pos_in_list < 0 || pos_in_list >= list->length){
        return default_int;
    }

    int travel_pos = 0;
    struct PlayerData_Node * traverse = list->head;
    while(traverse != NULL){
        if(travel_pos == pos_in_list){
            return traverse->data.player_id;
        }
        traverse = traverse->next;
    }

    return default_int;
}

struct City* find_city(struct City_List * list, int city_id){
    struct City_Node * list_traverse = list->head;
    while(list_traverse != NULL){
        if(list_traverse->data.city_id == city_id){
            return &(list_traverse->data);
        }
        list_traverse = list_traverse->next;
    }
    return NULL;
}

struct City* find_city_in_all(struct PlayerData_List * list, int city_id){
    struct PlayerData_Node * list_traverse = list->head;
    while(list_traverse != NULL){
        struct City* finding = find_city(&(list_traverse->data.cities), city_id);
        if(finding != NULL){
            return finding;
        }
        list_traverse = list_traverse->next;
    }
    return NULL;
}
#endif