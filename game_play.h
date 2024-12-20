#pragma once

#include "civ.h"

struct PlayerData_List init_players();

bool coord_under_player_control(int row, int col, struct PlayerData_List players);

void print_tiles_buildable(struct MapData map);

void print_cities(struct PlayerData_List players);

int count_players(struct PlayerData_List players);

enum TileType get_type(struct MapData map, int row, int col);

struct Tile_Coord_List generate_city_area(int row, int col, struct MapData *map, int player_id, int city_id);

struct PlayerData_Node *find_player(int player_id, struct PlayerData_List players);

struct Buildable_Structure *create_none_structure();

struct Buildable_Structure *create_beta_structure();

struct Buildable_Structure *create_build_structre(enum DistrictType type, int build_id, struct Tile_Coord coord);

void create_city(int row, int col, int player_id, struct PlayerData_List players, struct MapData *map);

void harvest_food_production(struct MapData *map, struct PlayerData_List players);

bool coord_in_list(struct Tile_Coord_List coord_list, int row, int col);

struct Tile_Coord_List get_available_coords_settler(struct MapData map, struct City city);

struct Tile_Coord_List get_available_aquaduct(struct MapData map, struct City city);

bool city_has_district(struct City city, enum DistrictType targetdist);

struct Tile_Coord_List get_available_coords(struct MapData map, struct City mycity, enum DistrictType type);

struct Tile_Coord get_ith_coord(struct Tile_Coord_List list, int index);

void make_move(int player_id, struct PlayerData_List players, struct MapData *map);

void make_all_moves(struct PlayerData_List players, struct MapData *map);

int get_num_district(struct City_List cities, enum DistrictType type);

int win_condition_naive(struct PlayerData_List players, struct MapData *map);

void place_initial_cities(struct MapData *map, struct PlayerData_List *players);




