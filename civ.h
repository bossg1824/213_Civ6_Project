#ifndef _CIVILIZATION_STRUCTS_
#define _CIVILIZATION_STRUCTS_

#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>   
#include <sys/time.h> 
#include <time.h>     
#define tile_cols 30
#define tile_rows 30
#define sub_tech_tree_size 5
#define num_players 4
#define map_seed 147
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
            struct TileData* cur = &(civ_state->mymap.tiles[i][j]);
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
        civ_size += (strlen(travel->data.civ_name) * sizeof(char));
        
        
        
        if(travel->data.techtree != NULL){
            //add the base size of a tech tree
            civ_size += sizeof(struct TechTree);

            //for each node of research in each tech tree
            for(int i = 0; i < sub_tech_tree_size; i++){
                //add the length of the tech name
                civ_size += (strlen(travel->data.techtree->rocketry_nodes[i].tech_bonus_name) * sizeof(char));
                civ_size += (strlen(travel->data.techtree->uranium_nodes[i].tech_bonus_name) * sizeof(char));
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
            civ_size += (strlen(visiting.city_name) * sizeof(char));

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

#endif;