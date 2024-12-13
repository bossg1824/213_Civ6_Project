#include "civ.h"
#include "map_val_displayer.c"



struct PlayerData_List init_players()
{
    struct PlayerData_List players;
    players.head = NULL;
    players.length = 0;
    for(int i = 0; i < num_players;i++)
    {
        struct PlayerData_Node player_node;
        struct PlayerData player;
        player.player_id = i;
        char player_name[20];
        sprintf(player_name,"Civ%d",i);
        player.civ_name = player_name;
        player.techtree = NULL;
        struct City_List cities;
        cities.head = NULL;
        cities.length = 0;
        player.cities = cities;
        player.total_faith = 0;
        player.faith_per_turn = 0;
        player.science_per_turn = 0;
        player.production_per_turn = 0;
        player_node.data = player;
        player_node.next = NULL;
        if(players.head == NULL)
        {
            players.head = &player_node;
        }
        else
        {
            players.head->next = &player_node;
        }
        players.length++;
    }
    return players;
}

bool coord_under_player_control(int row, int col, struct PlayerData_List players)
{
    struct PlayerData_Node *current_player_node = players.head;
    while(current_player_node != NULL)
    {
        struct City_List cities = current_player_node->data.cities;
        struct City_Node *city = cities.head;
        while(city != NULL)
        {
            struct Tile_Coord_List coords = city->data.tiles_under_controll;
            struct Tile_Coord_Node *cord_node = coords.head;
            while(cord_node != NULL)
            {
                struct Tile_Coord coord = cord_node->data;
                if(coord.y == row & coord.x == col)
                {
                    return true;
                }
                cord_node = cord_node->next;
            }
            city = city->next;
        }
        current_player_node= current_player_node->next;
    }
    return false;
}


struct PlayerData find_player(int player_id, struct PlayerData_List players)
{
    struct PlayerData_Node *current_player_node = players.head;
    while(current_player_node != NULL)
    {
        int current_id = current_player_node->data.player_id;
        if(current_id == player_id)
        {
            return current_player_node->data;
        }
        current_player_node= current_player_node->next;
    }
    perror("COULD NOT FIND PLAYER WITH SPECIFIED ID");
    struct PlayerData bad;
    return bad;
}

//Todo
bool create_city(int row, int col, int player_id, struct PlayerData_List players, int *city_id)
{
    int city_radius = 2;
    struct PlayerData current_player_data = find_player(player_id, players);
    struct City_Node new_city;
    new_city.next = NULL;
    struct City city_data;
    city_data.city_id = *city_id;
    (*city_id)++;

    //current_player_data.cities;
    return false;
}

int main() {
    struct MapData map_a = map_initialize_default();
    struct PlayerData_List players_a = init_players();
    display_map(map_a);
    

    int city_count = 0;
    //initialize spawns
    int spawns_completed = 0;
    struct PlayerData_Node *current_node = players_a.head;
    
    while(spawns_completed < num_players)
    {
        int min_row = 0;
        int max_row = tile_rows - 1;
        int true_row = rand() % (max_row - min_row + 1) + min_row;   
        int min_col = 0;
        int max_col = tile_cols - 1;
        int true_col = rand() % (max_col - min_col + 1) + min_col;
        if((!coord_under_player_control(true_row,true_col,players_a)) & (map_a.tiles[true_row][true_col].tiletype != mountain) & (map_a.tiles[true_row][true_col].tiletype != ocean)
        & (map_a.tiles[true_row][true_col].tiletype != lake))
        {
            create_city(true_row,true_col,current_node->data.player_id,players_a,&city_count);
            spawns_completed++;
            current_node = current_node->next;
        }
        
    }
    






    return 0;
}