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
                if(coord.y == row && coord.x == col)
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
            //printf("NAME: %s\n",city->data.city_name);
            //printf("ROW: %d, COL:%d\n",city->data.city_center_coord.x,city->data.city_center_coord.y);
            //printf("Center_Obj: %d\n",city->data.built_structures.head->data.district.district_type);
            printf("Production spent on current district type %d from %s: %d\n",city->data.current_structure_in_production.district.district_type,city->data.city_name,city->data.current_structure_in_production.production_spent);
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
        if(((row+x) < 0) || ((row+x) >= tile_rows))
        {
            continue;
        }
        for(int y = -radius; y <= radius; y++)
        {
            if(((col+y) < 0) || ((col+y) >= tile_cols))
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
    //printf("E");
    //printf("row: %d, col: %d",row,col);

    //printf("TESTING %d g",map.tiles[row][col].civ_id_controlling);

    struct Tile_Coord_List coords;
    struct Tile_Coord_Node *current_node = NULL;
    int radius = 2;
    for(int x = -radius; x <= radius; x++)
    {   
        if(((row+x) < 0) || ((row+x) >= tile_rows))
        {
            continue;
        }
        for(int y = -radius; y <= radius; y++)
        {
            if(((col+y) < 0) || ((col+y) >= tile_cols))
            {
                continue;
            }
            if(map->tiles[row+x][col+y].civ_id_controlling == default_int)
            {
                map->tiles[row+x][col+y].civ_id_controlling = player_id;
                //printf("C");
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

struct Buildable_Structure* create_none_structure()
{
    struct Buildable_Structure* empty= calloc(1, sizeof(struct Buildable_Structure));;
    empty->is_empty = true;
    struct District empty_district;
    empty_district.current_level = 0;
    empty_district.max_level = 0;
    empty_district.production_cost_to_upgrade = 0;
    empty_district.district_type = none_district;
    empty->district = empty_district;
    empty->bonus_type = none_bonus;
    empty->bonus_amount = 0;
    empty->production_cost = 0;
    empty->production_spent = 0;
    empty->completed = true;
    empty->estimated_turns_until_completion = 0;
    empty->build_id = default_int;
    return empty;
}

struct Buildable_Structure* create_city_center()
{
    struct Buildable_Structure * center = calloc(1, sizeof(struct Buildable_Structure));
    center->is_empty = false;
    struct District center_district;
    center_district.current_level = 1;
    center_district.max_level = 1;
    center_district.production_cost_to_upgrade = 0;
    center_district.district_type = city;
    center->district = center_district;
    center->bonus_type = food;
    center->bonus_amount = 2;
    center->production_cost = 60;
    center->production_spent = 0;
    center->completed = true;
    center->estimated_turns_until_completion = 0;
    center->build_id = 0;
    return center;
}

struct Buildable_Structure* create_beta_structure()
{
        struct Buildable_Structure * structure = calloc(1, sizeof(struct Buildable_Structure));
        structure->is_empty = false;
        struct District district;
        district.current_level = 1;
        district.max_level = 1;
        district.production_cost_to_upgrade = 0;
        district.district_type = farm;
        structure->district = district;
        structure->bonus_type = food;
        structure->bonus_amount = 3;
        structure->production_cost = 15;
        structure->production_spent = 0;
        structure->completed = false;
        structure->estimated_turns_until_completion = 0;
        structure->build_id = 0;
        return structure;
}

struct Buildable_Structure* create_build_structre(enum DistrictType type, int build_id, struct Tile_Coord coord)
{
    if(type == farm)
    {
        struct Buildable_Structure * structure = create_beta_structure();
        structure->district.district_type = farm;
        structure->bonus_type = food;
        structure->bonus_amount = 3;
        structure->production_cost = 15;
        structure->build_id = build_id;
        structure->coordinate = coord;
        return structure;
    }
    else if(type == mine)
    {
        struct Buildable_Structure * structure = create_beta_structure();
        structure->district.district_type = mine;
        structure->bonus_type = production;
        structure->bonus_amount = 3;
        structure->production_cost = 25;
        structure->build_id = build_id;
        structure->coordinate = coord;
        return structure;
    }
    else if(type == city)
    {
        struct Buildable_Structure * structure = create_beta_structure();
        structure->district.district_type = city;
        structure->bonus_type = food;
        structure->bonus_amount = 2;
        structure->production_cost = 60;
        structure->build_id = build_id;
        structure->coordinate = coord;
        return structure;
    }
    else
    {
        struct Buildable_Structure * structure = create_beta_structure();
        structure->build_id = build_id;
        structure->coordinate = coord;
        return structure;
    }
}


//Todo
void create_city(int row, int col, int player_id, struct PlayerData_List players, struct MapData *map)
{   
    //printf("B");
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
    struct Buildable_Structure buildable_structure = *create_none_structure();
    city_data.current_structure_in_production = buildable_structure;
    struct Buildable_Structure_List built;
    struct Buildable_Structure_Node *city_center_node = calloc(1, sizeof(struct Buildable_Structure_Node));
    struct Buildable_Structure * city_structure = create_city_center();
    city_center_node->data = *city_structure;
    city_center_node->data.coordinate = center;
    city_center_node->next = NULL;
    built.head = city_center_node;
    built.length = 1;
    city_data.built_structures = built;
    //printf("district type: %d",city_data.built_structures.head->data.district.district_type);

    //adding the built city to the map
    map->tiles[row][col].buildable_structure = city_structure;

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


void harvest_food_production(struct MapData *map, struct PlayerData_List players)
{
    struct PlayerData_Node *current_player_node = players.head;
    while((current_player_node != NULL) )
    {
        struct City_List cities = current_player_node->data.cities;
        struct City_Node *mycity = cities.head;
        int empire_production = 0;
        while(mycity != NULL)
        {
            struct Tile_Coord_List worked_list = mycity->data.worked_tiles;
            struct Tile_Coord_Node *worked_node = worked_list.head;
            int food_turn = 0;
            int production_turn = 0;

            while(worked_node != NULL)
            {
                struct Tile_Coord coord = worked_node->data;
                struct TileData tile_worked_from_map = (*map).tiles[coord.x][coord.y];
                food_turn += tile_worked_from_map.food;
                production_turn += tile_worked_from_map.production;

                struct Buildable_Structure * built_on_tile = tile_worked_from_map.buildable_structure;
                if(built_on_tile != NULL)
                {
                    if(built_on_tile->bonus_type == food)
                    {
                        food_turn += built_on_tile->bonus_amount;
                    }
                    else if(built_on_tile->bonus_type == production)
                    {
                        production_turn += built_on_tile->bonus_amount;
                    }
                }

                

                worked_node = worked_node->next;
            }//exit worked tiles iteration and jump now into the entire city

            //food_turn and production_turn for the city should now be accurate
            empire_production += production_turn;


            //potentially increment population
            mycity->data.food_per_tern = food_turn;
            mycity->data.food_accumulated += food_turn;

            int delta_housing = mycity->data.housing - mycity->data.population;
            int food_needed_to_gain_pop = (pow(2.0,-delta_housing) + 1) * 8 * mycity->data.population;

            if(mycity->data.food_accumulated >= food_needed_to_gain_pop)
            {
                mycity->data.population+=1;
                mycity->data.food_accumulated -= food_needed_to_gain_pop;
            }

            //if current building produced is not a non_building or completed
            if(!mycity->data.current_structure_in_production.is_empty && !mycity->data.current_structure_in_production.completed)
            {
                mycity->data.current_structure_in_production.production_spent+= production_turn;
                mycity->data.current_structure_in_production.estimated_turns_until_completion = (mycity->data.current_structure_in_production.production_cost - mycity->data.current_structure_in_production.production_spent)/production_turn;
                //if we finished the building
                if(mycity->data.current_structure_in_production.production_spent >= mycity->data.current_structure_in_production.production_cost)
                {   
                    //check if the finished space isn't occupied, if not add it to completed buildings and add it to the map data otherwise we lost the production
                    struct TileData structures_tile = (*map).tiles[mycity->data.current_structure_in_production.coordinate.x][mycity->data.current_structure_in_production.coordinate.y];
                    if((structures_tile.civ_id_controlling == default_int) || ((structures_tile.city_id_controlling == mycity->data.city_id) && (structures_tile.civ_id_controlling == current_player_node->data.player_id)))
                    {
                        //check if we finished constructing a city
                        if(mycity->data.current_structure_in_production.district.district_type == city)
                        {
                            create_city(mycity->data.current_structure_in_production.coordinate.x,mycity->data.current_structure_in_production.coordinate.y,current_player_node->data.player_id,players,map);
                        }//else add structure to built list and display in map
                        else
                        {
                            //add a node to built structures
                            mycity->data.current_structure_in_production.completed = true;
                            struct Buildable_Structure_Node* new_completed_building = calloc(1, sizeof(struct Buildable_Structure_Node));
                            new_completed_building->data = mycity->data.current_structure_in_production;
                            new_completed_building->next = NULL;

                            struct Buildable_Structure_Node* current_built_obj = mycity->data.built_structures.head;
                            //the city center should always have been build so the current built_obj shouldn't be null
                            while(current_built_obj->next != NULL)
                            {
                                current_built_obj = current_built_obj->next;
                            }
                            //the next field is now null
                            current_built_obj->next = new_completed_building;
                            mycity->data.built_structures.length++;
                            map->tiles[mycity->data.current_structure_in_production.coordinate.x][mycity->data.current_structure_in_production.coordinate.y].buildable_structure = &(mycity->data.current_structure_in_production);
                        }
                    }

                    //change whats currently in production to default
                    mycity->data.current_structure_in_production = *create_none_structure();
                }
            }
            else
            {
                perror("Trying to add production to a structure that is empty or is already completed\n");
            }

            mycity = mycity->next;
        }

        current_player_node->data.production_per_turn = empire_production;
        current_player_node= current_player_node->next;
    }
}

bool coord_in_list(struct Tile_Coord_List coord_list, int row, int col)
{
    struct Tile_Coord_Node *node = coord_list.head;
    while(node != NULL)
    {
        if((node->data.x == row) && (node->data.y == col))
        {
            return true;
        }
        node = node->next;
    }
    return false;
}

struct Tile_Coord_List get_available_coords_settler(struct MapData map, struct City city)
{

    struct Tile_Coord_List new_coords;
    ///struct Tile_Coord_Node *current_node = calloc(1, sizeof(struct Tile_Coord_Node));
    struct Tile_Coord_Node *current_node = NULL;
    new_coords.head = current_node;
    new_coords.length = 0;

    struct Tile_Coord city_center_coord = city.city_center_coord;
    int row = city_center_coord.x;
    int col = city_center_coord.y;
    int outer_radius = 5;
    int inner_radius = 2;
    for(int i = -outer_radius; i >= outer_radius; i++)
    {
        if((i <= inner_radius) && (i >= -inner_radius))
        {
            continue;
        }
        if((row+i < 0) || (row+i >= tile_rows))
        {
            continue;
        }

        for(int j = -outer_radius; j >= outer_radius; j++)
        {
            if((j <= inner_radius) && (j >= -inner_radius))
            {
                continue;
            }
            if((col+j < 0) || (col+j >= tile_rows))
            {
                continue;
            }
            struct TileData tile = map.tiles[row+i][col+j];
            if((tile.tiletype == mountain) || (tile.tiletype == lake) || (tile.tiletype == ocean))
            {
                continue;
            }

            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data.x = row+i;
            new_node->data.y = col+j;
            new_node->next = NULL;
            if(new_coords.head == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
    }
    return new_coords;
}

struct Tile_Coord_List get_available_aquaduct(struct MapData map, struct City city)
{
    struct Tile_Coord city_center_coord = city.city_center_coord;
    struct Tile_Coord_List new_coords;
    ///struct Tile_Coord_Node *current_node = calloc(1, sizeof(struct Tile_Coord_Node));
    struct Tile_Coord_Node *current_node = NULL;
    new_coords.head = current_node;
    new_coords.length = 0;
    int row = city_center_coord.x;
    int col = city_center_coord.y;

    if((row - 2) >= 0)
    {

        if(((map.tiles[row-2][col].tiletype == mountain) || (map.tiles[row-2][col].tiletype == lake)) && coord_in_list(city.tiles_under_controll,row - 1,col) && map.tiles[row-1][col].buildable_structure == NULL)
        {
            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data.x = row-1;
            new_node->data.y = col;
            new_node->next = NULL;
            if(new_coords.head == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
    }
    if((row + 2) < tile_rows)
    {
        if(((map.tiles[row+2][col].tiletype == mountain) || (map.tiles[row+2][col].tiletype == lake)) && coord_in_list(city.tiles_under_controll,row + 1,col) && map.tiles[row+1][col].buildable_structure == NULL)
        {
            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data.x = row+1;
            new_node->data.y = col;
            new_node->next = NULL;
            if(new_coords.head == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
    }
    if((col - 2) >= 0)
    {
        if(((map.tiles[row][col-2].tiletype == mountain) || (map.tiles[row][col-2].tiletype == lake)) && coord_in_list(city.tiles_under_controll,row,col-1) && map.tiles[row][col-1].buildable_structure == NULL)
        {
            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data.x = row;
            new_node->data.y = col-1;
            new_node->next = NULL;
            if(new_coords.head == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
    }
    if((col + 2) < tile_cols)
    {
        if(((map.tiles[row][col+2].tiletype == mountain) || (map.tiles[row][col+2].tiletype == lake)) && coord_in_list(city.tiles_under_controll,row,col+1) && map.tiles[row][col+1].buildable_structure == NULL)
        {
            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data.x = row;
            new_node->data.y = col+1;
            new_node->next = NULL;
            if(new_coords.head == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
    }
    return new_coords;
}


struct Tile_Coord_List get_available_coords(struct MapData map, struct City mycity, enum DistrictType type)
{

    if(type == aquaduct)
    {
        return get_available_aquaduct(map,mycity);
    }
    else if(type == city)
    {
        return get_available_coords_settler(map,mycity);
    }

    struct Tile_Coord_List new_coords;
    ///struct Tile_Coord_Node *current_node = calloc(1, sizeof(struct Tile_Coord_Node));
    struct Tile_Coord_Node *current_node = NULL;
    new_coords.head = current_node;
    new_coords.length = 0;

    //we want to find get tyles with nothing built on them and are buildable: river, flat, hill
    struct Tile_Coord_List control_list = mycity.tiles_under_controll;
    struct Tile_Coord_Node *control_node = control_list.head;
    while(control_node != NULL)
    {   
        //cant build any district on impassable land
        struct TileData tile = map.tiles[control_node->data.x][control_node->data.y];
        if((tile.tiletype == mountain) || (tile.tiletype == lake) || (tile.tiletype == ocean))
        {
            control_node = control_node->next;
            continue;
        }

        if(type == farm)
        {
            if(tile.tiletype == flat)
            {
                struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
                new_node->data = control_node->data;
                new_node->next = NULL;

                if(current_node == NULL)
                {
                    new_coords.head = new_node;
                }
                else
                {
                    current_node->next = new_node;
                }
                current_node = new_node;
                new_coords.length++;
                
            }
        }
        else if(type == mine)
        {
            if(tile.tiletype == hill)
            {
                struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
                new_node->data = control_node->data;
                new_node->next = NULL;

                if(current_node == NULL)
                {
                    new_coords.head = new_node;
                }
                else
                {
                    current_node->next = new_node;
                }
                current_node = new_node;
                new_coords.length++;
            }
        }
        //industry, campus, holy city can technically be built on any passible land that is owned by the current city
        else
        {
            struct Tile_Coord_Node *new_node = calloc(1, sizeof(struct Tile_Coord_Node));
            new_node->data = control_node->data;
            new_node->next = NULL;

            if(current_node == NULL)
            {
                new_coords.head = new_node;
            }
            else
            {
                current_node->next = new_node;
            }
            current_node = new_node;
            new_coords.length++;
        }
        control_node = control_node->next;
    }
    return new_coords;
}


struct Tile_Coord get_ith_coord(struct Tile_Coord_List list,int index)
{
    struct Tile_Coord_Node* node = list.head;
    int count = 0;
    while(count < index)
    {
        if(node == NULL)
        {
            perror("Index out of Bounds error");
            return list.head->data;
        }

        node = node->next;
        count++;
    }
    return node->data;

}

void make_move(int player_id, struct PlayerData_List players, struct MapData *map)
{
    struct PlayerData_Node* current_player_node = find_player(player_id, players);
    struct City_List cities = current_player_node->data.cities;
    struct City_Node *mycity = cities.head;
    while(mycity != NULL)
    {
        //havest_food should've set a finished building to empty
        int city_move_attempt = 0;
        while(mycity->data.current_structure_in_production.is_empty)
        {
            //now we are concedering cities where there are things to do
            int min = 0;
            int max = 2;
            int choice = rand() % (max- min + 1) + min;  
            printf("REEEE");

            if(choice == 0)//farm
            {
                struct Tile_Coord_List possible_placement = get_available_coords(*map,mycity->data,farm);
                if(possible_placement.length != 0)
                {
                    min = 0;
                    max = possible_placement.length-1;
                    int location_choice = rand() % (max- min + 1) + min;
                    struct Tile_Coord cord_to_choose = get_ith_coord(possible_placement,location_choice);
                    struct Buildable_Structure *building = create_build_structre(farm,mycity->data.built_structures.length, cord_to_choose);
                    mycity->data.current_structure_in_production = *building;
                }
                city_move_attempt++;
            }
            else if(choice == 1)//mine
            {
                struct Tile_Coord_List possible_placement = get_available_coords(*map,mycity->data,mine);
                if(possible_placement.length != 0)
                {
                    min = 0;
                    max = possible_placement.length-1;
                    int location_choice = rand() % (max- min + 1) + min;
                    struct Tile_Coord cord_to_choose = get_ith_coord(possible_placement,location_choice);
                    struct Buildable_Structure *building = create_build_structre(mine,mycity->data.built_structures.length, cord_to_choose);
                    mycity->data.current_structure_in_production = *building;
                }
                city_move_attempt++;
            }
            else if(choice == 2)//settler
            {   
                printf("SETTLER\n");
                struct Tile_Coord_List possible_placement = get_available_coords(*map,mycity->data,city);
                if(possible_placement.length != 0)
                {
                    min = 0;
                    max = possible_placement.length-1;
                    int location_choice = rand() % (max- min + 1) + min;
                    struct Tile_Coord cord_to_choose = get_ith_coord(possible_placement,location_choice);
                    struct Buildable_Structure *building = create_build_structre(city,mycity->data.built_structures.length, cord_to_choose);
                    mycity->data.current_structure_in_production = *building;
                }
                else
                {
                    printf("SETTLER ERROR?\n");
                }
                city_move_attempt++;
            }
            if(city_move_attempt == 100)
            {
                perror("City had no possible moves\n");
                break;
            }
        }
        mycity = mycity->next;
    }
    current_player_node= current_player_node->next;
}


void make_all_moves(struct PlayerData_List players, struct MapData *map)
{
    for(int i = 0; i < num_players; i++)
    {
        make_move(i,players,map);
    }
}



int main() {
    //initialize map and players
    struct MapData map_a = map_initialize_default();
    struct PlayerData_List players_a = init_players();


    //we now spawn in the players
    int number_players = count_players(players_a);
    int city_count = 0;
    int spawns_completed = 0;
    struct PlayerData_Node *current_node = players_a.head;
    
    while(spawns_completed < number_players)
    {
        int min_row = 0;
        int max_row = tile_rows - 1;
        int true_row = rand() % (max_row - min_row + 1) + min_row;   
        int min_col = 0;
        int max_col = tile_cols - 1;
        int true_col = rand() % (max_col - min_col + 1) + min_col;
        if((!coord_under_player_control(true_row,true_col,players_a)) && (map_a.tiles[true_row][true_col].tiletype != mountain) && (map_a.tiles[true_row][true_col].tiletype != ocean)
        && (map_a.tiles[true_row][true_col].tiletype != lake))
        {
            
            if((num_players == 2))
            {
                if(!((true_col < (tile_cols/4)) || (true_col >= (3*(tile_cols/4)))))
                {
                    continue;
                }
                create_city(true_row,true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
                create_city(true_row,tile_cols -1 -true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
            }
            else if((num_players == 4) && (tile_cols == tile_rows))
            {
                if(!(((true_col < (tile_cols/2)) && (true_row < (tile_rows/2))) && ((true_col < (tile_cols/4)) || (true_row < (tile_rows/4)))))
                {
                    continue;
                }
                int half_size = tile_rows / 2;
                int rot_90_true_row = true_col; int rot_90_true_col = tile_cols - 1 - true_row;
                int rot_180_true_row = tile_cols - 1 - true_row; int rot_180_true_col = tile_cols - 1 - true_col;
                int rot_270_true_row = tile_cols -1 - true_col; int rot_270_true_col = true_row;

                create_city(true_row,true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
                create_city(rot_90_true_row,rot_90_true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
                create_city(rot_180_true_row,rot_180_true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
                create_city(rot_270_true_row,rot_270_true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
            }
            else
            {
                create_city(true_row,true_col,current_node->data.player_id,players_a,&map_a);
                spawns_completed++;
                current_node = current_node->next;
            }
        }
        
    }
    //player spawning complete
    int moves_into_future = 200;
    for(int i = 0; i < moves_into_future; i++)
    {
        make_all_moves(players_a, &map_a);
        harvest_food_production(&map_a,players_a);
        printf("MOVE %d\n",i);
        print_cities(players_a);
    }


    display_loop(&(players_a.head->data), &map_a, &players_a);
    //print_cities(players_a);
    
    




    return 0;
}