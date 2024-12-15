#include "civ.h"
//#include "map_val_displayer.c"
#include "game_initialize.c"
#include "display.c"


struct PlayerData_List init_players()
{
    struct PlayerData_List players;
    players.head = NULL;
    players.length = 0;
    struct PlayerData_Node * currentNode = players.head;
    for(int i = 0; i < num_players;i++)
    {
        struct PlayerData_Node * player_node = calloc(1, sizeof(struct PlayerData_Node));
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
        player_node->data = player;
        player_node->next = NULL;
        if(players.head == NULL)
        {
            players.head = player_node;
        }
        else
        {
            currentNode->next = player_node;

        }
        currentNode = player_node;
        players.length++;
    }
    return players;
}

bool coord_under_player_control(int row, int col, struct PlayerData_List players)
{   
    struct PlayerData_Node *current_player_node = players.head;
    while((current_player_node != NULL) )
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

void print_cities(struct PlayerData_List players)
{   
    printf("PRINTING CITIES\n");
    struct PlayerData_Node *current_player_node = players.head;
    if(current_player_node == NULL)
    {
        printf("NO PLAYERS");
    }



    while((current_player_node != NULL) )
    {
        struct City_List cities = current_player_node->data.cities;
        struct City_Node *city = cities.head;
        if(city == NULL)
        {
            printf("NO CITIES");
        }
        while(city != NULL)
        {
            printf("NAME: %s\n",city->data.city_name);
            printf("ROW: %d, COL:%d\n",city->data.city_center_coord.x,city->data.city_center_coord.y);
            printf("Center_Obj: %d\n",city->data.built_structures.head->data.district.district_type);
            city = city->next;
        }
        current_player_node= current_player_node->next;
    }
}


int count_players(struct PlayerData_List players)
{
    int count = 0;
    struct PlayerData_Node *current_player_node = players.head;
    while(current_player_node != NULL)
    {
        //printf("%d\n",current_player_node->data.player_id);
        current_player_node = current_player_node->next;
        count++;
    }
    
    return count;
}

enum TileType get_type(struct MapData map, int row, int col)
{
    return map.tiles[row][col].tiletype;
}

int get_food(struct MapData map, int row, int col)
{
    return map.tiles[row][col].food;
}

int get_production(struct MapData map, int row, int col)
{
    return map.tiles[row][col].production;
}

bool next_to_lake_or_ocean(struct MapData map, int row, int col)
{
    int radius = 1;
    for(int x = -radius; x <= radius; x++)
    {   
        if(((row+x) < 0) | ((row+x) >= tile_rows))
        {
            continue;
        }
        for(int y = -radius; y <= radius; y++)
        {
            if(((col+y) < 0) | ((col+y) >= tile_cols))
            {
                continue;
            }
            if(get_type(map,row+x,col+y) == ocean || get_type(map,row+x,col+y) == lake)
            {
                return true;
            }
        }
    }
    return false;
}

struct Tile_Coord_List generate_city_area(int row, int col, struct MapData *map, int player_id, int city_id)
{
    printf("E");
    //printf("row: %d, col: %d",row,col);

    //printf("TESTING %d g",map.tiles[row][col].civ_id_controlling);

    struct Tile_Coord_List coords;
    struct Tile_Coord_Node *current_node = NULL;
    int radius = 2;
    for(int x = -radius; x <= radius; x++)
    {   
        if(((row+x) < 0) | ((row+x) >= tile_rows))
        {
            continue;
        }
        for(int y = -radius; y <= radius; y++)
        {
            if(((col+y) < 0) | ((col+y) >= tile_cols))
            {
                continue;
            }
            if(map->tiles[row+x][col+y].civ_id_controlling == default_int)
            {
                map->tiles[row+x][col+y].civ_id_controlling = player_id;
                printf("C");
                map->tiles[row+x][col+y].city_id_controlling = city_id;
                struct Tile_Coord_Node *new_coord_node = calloc(1, sizeof(struct Tile_Coord_Node));
                struct Tile_Coord new_coord;
                new_coord.x = row+x;
                new_coord.y = row+y;
                new_coord_node->data = new_coord;
                new_coord_node->next = NULL;

                if(current_node == NULL)
                {
                    coords.head = new_coord_node;
                }
                else
                {
                    current_node->next = new_coord_node;
                }
                current_node = new_coord_node;
            }
        }
    }
    return coords;
}


struct PlayerData_Node *find_player(int player_id, struct PlayerData_List players)
{
    struct PlayerData_Node *current_player_node = players.head;
    while(current_player_node != NULL)
    {
        int current_id = current_player_node->data.player_id;
        if(current_id == player_id)
        {
            return current_player_node;
        }
        current_player_node= current_player_node->next;
    }
    perror("COULD NOT FIND PLAYER WITH SPECIFIED ID");
    return NULL;
}

struct Buildable_Structure create_none_structure()
{
    struct Buildable_Structure empty;
    empty.is_empty = true;
    struct District empty_district;
    empty_district.current_level = 0;
    empty_district.max_level = 0;
    empty_district.production_cost_to_upgrade = 0;
    empty_district.district_type = none_district;
    empty.district = empty_district;
    empty.bonus_type = none_bonus;
    empty.bonus_amount = 0;
    empty.production_cost = 0;
    empty.production_spent = 0;
    empty.completed = false;
    empty.estimated_turns_until_completion = 0;
    empty.build_id = default_int;
    return empty;
}

struct Buildable_Structure create_city_center()
{
    struct Buildable_Structure center;
    center.is_empty = false;
    struct District center_district;
    center_district.current_level = 0;
    center_district.max_level = 0;
    center_district.production_cost_to_upgrade = 0;
    center_district.district_type = city;
    center.district = center_district;
    center.bonus_type = none_bonus;
    center.bonus_amount = 0;
    center.production_cost = 0;
    center.production_spent = 0;
    center.completed = false;
    center.estimated_turns_until_completion = 0;
    center.build_id = 0;
    return center;
}


//Todo
void create_city(int row, int col, int player_id, struct PlayerData_List players, struct MapData *map)
{   
    printf("B");
    struct PlayerData_Node* current_player_node = find_player(player_id, players);
    struct PlayerData current_player_data = current_player_node->data;
    struct City_Node *new_city = calloc(1, sizeof(struct City_Node));
    new_city->next = NULL;
    //giving the city data
    struct City city_data;
    char *city_name = malloc(20 * sizeof(char));
    sprintf(city_name,"City%d_%d",player_id,current_player_data.cities.length);
    city_data.city_name = city_name;
    //printf("namio%s",city_data.city_name);
    city_data.city_id = current_player_data.cities.length;
    struct Tile_Coord center_coord;
    center_coord.x = row;
    center_coord.y = col;
    city_data.city_center_coord = center_coord;
    int housing;
    if(get_type(*map,row,col) == river)
    {
        housing = 9;
    }
    else if(next_to_lake_or_ocean(*map,row,col))
    {
        housing = 6;
    }
    else
    {
        housing = 3;
    }
    city_data.housing = housing;
    city_data.population = 1;
    city_data.food_per_tern = 0;
    city_data.food_accumulated = 0;
    city_data.settlers_produced = 0;
    city_data.focus_food = true;
    city_data.focus_production = false;


    struct Tile_Coord_List worked;
    struct Tile_Coord_Node *center_node = calloc(1, sizeof(struct Tile_Coord_Node));;
    struct Tile_Coord center;
    center.x = row;
    center.y = col;
    center_node->data = center;
    center_node->next = NULL;
    worked.length = 1;
    worked.head = center_node;
    city_data.worked_tiles = worked;

    struct Tile_Coord_List controlled = generate_city_area(row,col,map,player_id,city_data.city_id);
    city_data.tiles_under_controll = controlled;

    struct Buildable_Structure buildable_structure = create_none_structure();
    city_data.current_structure_in_production = buildable_structure;
    struct Buildable_Structure_List built;
    struct Buildable_Structure_Node *city_center_node = calloc(1, sizeof(struct Buildable_Structure_Node));
    struct Buildable_Structure city_structure = create_city_center();
    city_center_node->data = city_structure;
    city_center_node->next = NULL;
    built.head = city_center_node;
    built.length = 1;
    city_data.built_structures = built;
    //printf("district type: %d",city_data.built_structures.head->data.district.district_type);

    

    //attach the data
    new_city->data = city_data;


    //printf("PRIOR TYPE %d", current_player_node->data.cities.head->data.built_structures.head->data.district.district_type);

    //attach the city to the city list
    if(current_player_data.cities.head == NULL)
    {
        //printf("ADDING FIRST CITY");
        current_player_node->data.cities.head =  new_city;
    }
    else
    {
        struct City_Node *last_city =  current_player_node->data.cities.head;
        while(last_city->next != NULL)
        {
            last_city = last_city->next;
        }
        //we are ensured the next is null
        last_city->next = new_city;
    }
    current_player_data.cities.length+=1;
    //printf("POST TYPE %d", current_player_node->data.cities.head->data.built_structures.head->data.district.district_type);
    //printf("POST Name %s", current_player_node->data.cities.head->data.city_name);
}

int main() {
    //printf("TEST\n");
    struct MapData map_a = map_initialize_default();
    struct PlayerData_List players_a = init_players();


    //display_map(map_a);

    //display_loop(&(players_a.head->data), &map_a, &players_a);

    int number_players_list = count_players(players_a);
    //printf("number_players: %d", number_players_list);

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
        //printf("A\n");
        if((!coord_under_player_control(true_row,true_col,players_a)) & (map_a.tiles[true_row][true_col].tiletype != mountain) & (map_a.tiles[true_row][true_col].tiletype != ocean)
        & (map_a.tiles[true_row][true_col].tiletype != lake))
        {
            printf("A\n");
            create_city(true_row,true_col,current_node->data.player_id,players_a,&map_a);
            //printf("PLAYER %d\n",current_node->data.player_id);
            spawns_completed++;
            //printf("C");
            current_node = current_node->next;
            //printf("D");
        }
        
    }
    //printf("ENDED");

    // for(int i = 0; i < tile_rows; i++)
    // {
    //     for(int j = 0; j < tile_rows; j++)
    //     {
    //         printf("%d",map_a.tiles[i][j].civ_id_controlling);
    //     }
    //     printf("\n");
    // }

    print_cities(players_a);



    display_loop(&(players_a.head->data), &map_a, &players_a);
    
    




    return 0;
}