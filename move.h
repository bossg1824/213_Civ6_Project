#pragma once

#include "civ.h"

//what the user has changed the city from what it would be doing since last turn
struct city_change{
    int city_id;
    struct Buildable_Structure build;
};

//linked list node for changes in city production
struct city_change_node{
    struct city_change data;
    struct city_change_node * next;
};

//linked list for changes in city production
struct city_change_list{
    struct city_change_node * head;
    int length;
};

//what the player has done for their entire turn
struct move{
    int cur_player_id;
    int next_player_id;
    int turn;
    int research; //0 for rocketry //1 for uranium
    struct city_change_list * city_changes;
};

//linked list node for stack of moves made
struct move_node{
    struct move data;
    struct move_node * next;
};

//stack for moves, could potentially be used to undo, probably won't be though
struct move_stack{
    struct move_node * head;
    int length;
};

//treats this list also like a stack because it does not matter
void add_change(struct city_change_list * list, int city_id, struct Buildable_Structure build);

void destroy_change_list(struct city_change_list * list);

struct city_change pop_change(struct city_change_list * list);

struct city_change_list * create_change_list();

void add_move(struct move_stack * stack, struct move data);

void destroy_move_stack(struct move_stack * stack);

struct move_stack * create_move_stack();

struct move move_pop(struct move_stack * stack);

struct move move_peek(struct move_stack * stack);

void update_game(struct GameState * game, struct move update);