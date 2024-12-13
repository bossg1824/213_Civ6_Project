#ifndef _CIVILIZATION_STRUCTS_
#define _CIVILIZATION_STRUCTS_

#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>   
#include <sys/time.h> 
#include <time.h>     
#define tile_cols 60
#define tile_rows 60
#define sub_tech_tree_size 5
#define num_players 4
#define map_seed 147
#define default_int -1

//Enums needed for Map Data
enum BonusType{
science,
faith,
production,
housing,
food
};

enum DistrictType{
campus,
industrial_zone,
holy_site,
aquaduct,
mine,
farm
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
    struct District district;
    enum BonusType bonus_type;
    int bonus_amount;
    int production_cost;
    int production_spent;
    bool completed;
    int estimated_turns_until_completion; 
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
    struct Tile_Coord_List tiles_under_controll; //An array of the tiles that are under this city's controll
    struct Buildable_Structure current_structure_in_production; //The structure this city is currently putting its production towards
    struct Buildable_Structure_List built_structures; //Linked list of structures that have been produced in this city
    struct City* next;
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
#endif