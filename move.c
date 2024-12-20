#pragma once

#include "civ.h"
#include "move.h"
#include "game_play.c"

struct city_change_list * create_change_list(){
    struct city_change_list * list = calloc(1, sizeof(struct city_change_list));
    return list;
}

void add_change(struct city_change_list * list, int city_id, struct Buildable_Structure build){
    struct city_change_node * new = calloc(1, sizeof(struct move_node));
    new->data.city_id = city_id;
    new->data.build = build;
    new->next = list->head;
    list->length++;
    return;
}

struct city_change pop_change(struct city_change_list * list){
    struct city_change returning;
    if(list->head == NULL){
        returning.city_id = default_int;
        return returning;
    }

    struct city_change_node * freeing = list->head;
    returning = freeing->data;
    
    list->head = freeing->next;
    list->length--;

    free(freeing);
    return returning;
}

void destroy_change_list(struct city_change_list * list){
    while(pop_change(list).city_id != default_int);
    free(list);
    return;
}

struct move_stack * create_move_stack(){
    struct move_stack * stack = calloc(1, sizeof(struct move_stack));
    return stack;
}

void add_move(struct move_stack * stack, struct move data){
    struct move_node * new = calloc(1, sizeof(struct move_node));
    new->data = data;
    new->next = stack->head;

    stack->head = new;
    stack->length++;
    return;
}

struct move move_peek(struct move_stack * stack){
    struct move returning;
    if(stack->head == NULL){
        returning.cur_player_id = default_int;
        return returning;
    }

    return stack->head->data;
}

struct move move_pop(struct move_stack * stack){
    struct move returning;
    if(stack->head == NULL){
        returning.cur_player_id = default_int;
        return returning;
    }

    struct move_node * freeing = stack->head;
    returning = freeing->data;

    stack->head = freeing->next;
    stack->length--;

    free(freeing);

    return returning;
}

void destroy_move_stack(struct move_stack * stack){
    while(move_pop(stack).cur_player_id != default_int);
    free(stack);
    return;
}

//takes a move and updates the game based on the move, if was the last move of the turn updates board for next turn
void update_game(struct GameState * game, struct move update){
    if(game->turn_count > update.turn){
        return;
    }

    if(pos_in_list(&(game->players), update.cur_player_id) <= pos_in_list(&(game->players), game->player_turn) 
                    && game->turn_count == update.turn){
                        return;
    }

    struct city_change changes;
    while((changes = pop_change(update.city_changes)).city_id != default_int){
        struct City * changing = find_city(&(find_player(update.cur_player_id,game->players)->data.cities), changes.city_id);
        changing->current_structure_in_production = changes.build;
    }

    if(pos_in_list(&(game->players), update.next_player_id) == 0){
        harvest_food_production(&(game->mymap), game->players);
    }
    return;
}