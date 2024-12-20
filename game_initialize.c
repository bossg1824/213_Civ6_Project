#pragma once

#include <string.h>
#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>   
#include <sys/time.h> 
#include <time.h>
#include <math.h>
#include "civ.h"

struct TwoDNoise{
    float data[tile_rows][tile_cols];
};

struct OffSetList{
    float data[11];
};

struct OffSetLists{
    struct OffSetList x_offsets;
    struct OffSetList y_offsets;
};


void printmap(struct MapData map){
    printf("\n\nPrinting tiletype values");
    for(int i = 0; i < map.rows; i++)
    {
        printf("\n");
        for(int j = 0; j < map.cols; j++)
        {
            printf("%d  ",map.tiles[i][j].tiletype);
        }
    }
    printf("\n\nPrinting food values");
    for(int i = 0; i < map.rows; i++)
    {
        printf("\n");
        for(int j = 0; j < map.cols; j++)
        {
            printf("%d  ",map.tiles[i][j].food);
        }
    }
    printf("\n\nPrinting production values");
    for(int i = 0; i < map.rows; i++)
    {
        printf("\n");
        for(int j = 0; j < map.cols; j++)
        {
            printf("%d  ",map.tiles[i][j].production);
        }
    }
    printf("\n\nPrinting resource values");
    for(int i = 0; i < map.rows; i++)
    {
        printf("\n");
        for(int j = 0; j < map.cols; j++)
        {
            printf("%d  ",map.tiles[i][j].resource);
        }
    }
    printf("\n");
}

//radius should be an int between 1 and 200
//dist dependancy should be between 0 and 100
//denormalizer should be between 1 and 20
struct TwoDNoise perlin_noise_one_octave(int rows, int cols, int radius_to_consider, float dist_dependancy_constant, float denormalizer)
{   
    srand(map_seed);
    int adjust_cols = cols + (radius_to_consider * 2) - 1;
    int adjust_rows = rows + (radius_to_consider * 2) - 1;
    float matrix [adjust_rows][adjust_cols];
    //create random noise
    for (int i = 0; i < adjust_rows; i++)
    {
        for (int j = 0; j < adjust_cols; j++)
        {
            matrix[i][j] = (float)rand() / (float)(RAND_MAX);
            //printf("%f ",matrix[i][j]);
        }
        //printf("\n");
    }

    float perlin_matrix [rows][cols];
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {   
            float rand_x = (radius_to_consider - 1) + i + ((float)rand() / (float)(RAND_MAX));
            float rand_y = (radius_to_consider - 1) + j + ((float)rand() / (float)(RAND_MAX));
            int sub_matrix_size = (radius_to_consider * 2) * (radius_to_consider * 2);
            float distances[sub_matrix_size];
            float values[sub_matrix_size];
            int counter = 0;
            for (int x = 0; x < radius_to_consider * 2; x++)
            {
                for (int y = 0; y < radius_to_consider * 2; y++)
                {
                    int a = x + i;
                    int b = y + j;
                    float distance = sqrt(pow((float) a - rand_x,2.0) + pow((float) b - rand_y,2.0));
                    float value = matrix[a][b];
                    distances[x + (y * radius_to_consider * 2)] = distance;
                    values[x + (y * radius_to_consider * 2)] = value;
                    counter++;
                }
            }
            float value_weight_sum = 0.0;
            float weight_sum = 0.0;
            for (int a = 0; a < sub_matrix_size; a++)
            {
                float value_i = values[a];
                float weight_i = 1.0/pow(distances[a],(float)dist_dependancy_constant);
                value_weight_sum+= (value_i * weight_i);
                weight_sum+= weight_i;
            }
            float weighted_average = value_weight_sum/weight_sum;

            //we now shift the distribution outward with a function we created using desmos
            float updated_weighted_avg;
            if(weighted_average > 0.5)
            {
                updated_weighted_avg =  0.5 * (1 - pow(1 - (2 * fabs(weighted_average - 0.5)),denormalizer)) + 0.5;
            }
            else
            {
                updated_weighted_avg =  -0.5 * (1 - pow(1 - (2 * fabs(weighted_average - 0.5)),denormalizer)) + 0.5;
            }
            //printf("   %d    ",i);
            perlin_matrix[i][j] = updated_weighted_avg;         
            //printf("%.2f ",updated_weighted_avg);
            
        }
        //printf("\n");
    }
    //printf("%f ", avg_diff_run);
    struct TwoDNoise noise_matrix;
    memcpy(noise_matrix.data,perlin_matrix,sizeof(perlin_matrix));
    return noise_matrix;
}

//using desmos and trial and error
//frequency is on 0 to 1 exclusive
struct TwoDNoise procedural_noise(int rows, int cols, float frequency)
{
    float distance_constant = 0.8;
    int radius = (int)round(pow(2,4 * (1 - frequency)));
    int decenterize_constant = (int)round(pow(2,2.34 * (1 - frequency)));
    return perlin_noise_one_octave(rows,cols,radius,distance_constant,decenterize_constant);
}


struct MapData create_testmap1()
{
    struct MapData mymap;
    mymap.cols = tile_cols;
    mymap.rows = tile_rows;

    for(int i = 0; i < mymap.rows; i++){
        for(int j = 0; j < mymap.cols; j++){
            struct Tile_Coord coord;
            coord.x = i;
            coord.y = j;
            enum TileType tile_type = mountain;
            enum ResourceType resource = none;
            //int civ_id_controlling = default_int;
            int production = 1;
            int food = 2;
            struct Buildable_Structure *buildable = NULL;
            struct TileData tile;
            tile.coordinate = coord;
            tile.tiletype = tile_type;
            tile.resource = resource;
            tile.city_id_controlling = 0;
            tile.production = production;
            tile.food = food;
            tile.buildable_structure = buildable;
            mymap.tiles[i][j] = tile;
            //printf("%d%d\n",i,j);
        }
    }
    return mymap;
}

void rotate_90(int matrix[tile_rows][tile_cols]) {
    
    //Temp matrix
    int rotated[tile_rows][tile_cols];
    
    //90 deg clock rotation
    for (int i = 0; i < tile_rows; i++) {
        for (int j = 0; j < tile_cols; j++) {
            rotated[j][tile_cols - 1 - i] = matrix[i][j];
        }
    }

    //copy temp into original
    for (int i = 0; i < tile_rows; i++) {
        for (int j = 0; j < tile_cols; j++) {
            matrix[i][j] = rotated[i][j];
        }
    }
}

struct OffSetLists generate_offsets(int starting_direction, int tile_row, int tile_col)
{   
    int rand_val;
    int xoff = 0;
    int yoff = 0;
    int direction = starting_direction;
    int quadrant = direction/4;
    int angle_change = direction%4;
    if (quadrant == 0)
    {
        rand_val = rand() % (3 - 0 + 1) + 0;
        if(rand_val >= angle_change)
        {
            xoff = 0;
            yoff = 1;
        }
        else
        {
            xoff = 1;
            yoff = 0;
        }
    }
    else if (quadrant == 1)
    {
        rand_val = rand() % (3 - 0 + 1) + 0;
        if(rand_val >= angle_change)
        {
            xoff = 1;
            yoff = 0;
        }
        else
        {
            xoff = 0;
            yoff = -1;
        }
    }
    else if (quadrant == 2)
    {
        rand_val = rand() % (3 - 0 + 1) + 0;
        if(rand_val >= angle_change)
        {
            xoff = 0;
            yoff = -1;
        }
        else
        {
            xoff = -1;
            yoff = 0;
        }
    }
    else if (quadrant == 3)
    {   rand_val = rand() % (3 - 0 + 1) + 0;
        if(rand_val >= angle_change)
        {
            xoff = -1;
            yoff = 0;
        }
        else
        {
            xoff = 0;
            yoff = -1;
        }
    }
    struct OffSetLists offs;
    offs.x_offsets.data[0] = xoff;
    offs.y_offsets.data[0] = yoff;

    int main_xoff = xoff;
    int main_yoff = yoff;
    direction = starting_direction;

    for(int i = 1; i < 11; i++)
    {
        quadrant = direction/4;
        angle_change = direction%4;
        if (quadrant == 0)
        {
            rand_val = rand() % (3 - 0 + 1) + 0;
            if(rand_val >= angle_change)
            {
                xoff = 0;
                yoff = 1;
            }
            else
            {
                xoff = 1;
                yoff = 0;
            }
        }
        else if (quadrant == 1)
        {
            rand_val = rand() % (3 - 0 + 1) + 0;
            if(rand_val >= angle_change)
            {
                xoff = 1;
                yoff = 0;
            }
            else
            {
                xoff = 0;
                yoff = -1;
            }
        }
        else if (quadrant == 2)
        {
            rand_val = rand() % (3 - 0 + 1) + 0;
            if(rand_val >= angle_change)
            {
                xoff = 0;
                yoff = -1;
            }
            else
            {
                xoff = -1;
                yoff = 0;
            }
        }
        else if (quadrant == 3)
        {   rand_val = rand() % (3 - 0 + 1) + 0;
            if(rand_val >= angle_change)
            {
                xoff = -1;
                yoff = 0;
            }
            else
            {
                xoff = 0;
                yoff = -1;
            }
        }
        main_xoff = main_xoff + xoff;
        main_yoff = main_yoff + yoff;
        offs.x_offsets.data[i] = main_xoff;
        offs.y_offsets.data[i] = main_yoff;
    }
    //printf("GENERATED OFFSETS");
    return offs;
}





struct MapData map_initialize_default()
{
    unsigned int seed = map_seed;
    struct MapData mymap;
    mymap.cols = tile_cols;
    mymap.rows = tile_rows;

    struct TwoDNoise noise_obj = procedural_noise(tile_rows,tile_cols,0.4);
    float noise_matrix [tile_rows][tile_cols];
    memcpy(noise_matrix,noise_obj.data,sizeof(noise_matrix));

    for(int i = 0; i < mymap.rows; i++){
        for(int j = 0; j < mymap.cols; j++){
            seed++;
            struct Tile_Coord coord;
            coord.x = i;
            coord.y = j;
            float random_val = noise_matrix[i][j];

            //setting the tiletypes based on the noise
            enum TileType tile_type;
            if(random_val <= 0.34)//0 to 0.34 inclusive
            {
                tile_type = lake;
            }
            else if(random_val <= 0.42)//0.34 exclusive to 0.42 inclusive
            {
                tile_type = mountain;
            }
            else if(random_val <= 0.61)//0.42 exclusive to 0.57 inclusive
            {
                tile_type = flat;
            }
            else //0.57 exclusive
            {
                tile_type = hill;
            }
            
            //itializing resources
            enum ResourceType resource;
            //1 divided by max is probabability
            int approx_num_resources = 40;
            int max = (float)(tile_rows * tile_cols)/(float)(approx_num_resources * 8);
            if (max < 1)
            {
                max = 1;
            }
            int min = 1;
            int rand_value = rand_r(&seed) % (max - min + 1) + min;
            //We hit a rare instance and we are on an accessible tile
            if((rand_value == 1) & (tile_type == 5))
            {
                //set to a resource of the 4 valid ones
                int resource_count = 4;
                resource = (rand_r(&seed) % (resource_count - 1 + 1) + min);  
            }
            else
            {
                resource = 0;
            }

            //initialize production values
            int production = 0;
            if((tile_type == ocean)|| (tile_type == mountain) || (tile_type == lake))
            {
                production = 0;
            }
            else if(resource > 0)
            {
                production = 5;
            }
            else if(tile_type == flat)
            {
                production = 1;
            }
            else if(tile_type == hill)
            {
                production = (7.692 * random_val) - 1.692;
            }

            //itializing food
            int food = 0;
            if(tile_type == mountain)
            {
                food = 0;
            }
            else if((tile_type == ocean) || (tile_type == lake) || (tile_type == hill))
            {
                food = 1;
            }
            else if(tile_type == flat)
            {
                food = (10.525* random_val) - 2.42;
            } 

            
            int civ_id_controlling = default_int;
            struct Buildable_Structure *buildable = NULL;
            struct TileData tile;
            tile.coordinate = coord;
            tile.tiletype = tile_type;
            tile.resource = resource;
            tile.civ_id_controlling = civ_id_controlling;
            tile.city_id_controlling = default_int;
            tile.production = production;
            tile.food = food;
            tile.buildable_structure = buildable;
            mymap.tiles[i][j] = tile;
        }
    }

    //generate ocean
    for(int y = 0; y < mymap.rows; y++){
        for(int x = 0; x < mymap.cols; x++){
            float inverse_ocean_size_multiplier = 0.9;
            float outside_ellipse = pow((x-(tile_cols/2.0 - 0.5)),2)/(pow((tile_cols/2.0 - 0.5),2) * inverse_ocean_size_multiplier) +
            pow((y-(tile_rows/2.0 - 0.5)),2)/(pow((tile_rows/2.0 - 0.5),2) * inverse_ocean_size_multiplier);
            if (outside_ellipse > 1)
            {
                mymap.tiles[y][x].tiletype = ocean;
                mymap.tiles[y][x].food = 1;
                mymap.tiles[y][x].production = 0;
                mymap.tiles[y][x].resource = none;
            }
        }
    }  



    //generate rivers

    int river_block_matrix[mymap.rows][mymap.cols];
    for(int i = 0; i < mymap.rows; i++){
        for(int j = 0; j < mymap.cols; j++){
            river_block_matrix[i][j] = 0;
        }
    }

    int river_length = 11;
    for(int i = 0; i < mymap.rows; i++){
        for(int j = 0; j < mymap.cols; j++){
            if((mymap.tiles[i][j].tiletype == mountain) | (mymap.tiles[i][j].tiletype == ocean) | (mymap.tiles[i][j].tiletype == lake))
            {
                //check if we are not blocked
                if (river_block_matrix[i][j] == 0)
                {
                    //start generating path

                    //int x_offsets [river_length];
                    //int y_offsets [river_length];

                    //try to generate river
                    int min = 0;
                    int max = 15;
                    int start_direction = rand() % (max - min + 1) + min;
                    struct OffSetLists offsets = generate_offsets(start_direction,i,j);
                    int safe_zone_radius = 8;
                    int river_nonproximity_radius = 3;
                    //check if out of bounds
                    bool valid_river = true;
                    for (int x = 0; x < river_length; x++)
                    {
                        int x_off = offsets.x_offsets.data[x];
                        int y_off = offsets.y_offsets.data[x];
                        int new_coord_x = i + x_off;
                        int new_coord_y = j + y_off;
                        


                        if((new_coord_x < 0) | (new_coord_x >= tile_rows))
                        {
                            valid_river = false;
                            break;
                        }
                        if((new_coord_y < 0) | (new_coord_y >= tile_cols))
                        {
                            valid_river = false;
                            break;
                        }
                        enum TileType current_tiletype = mymap.tiles[new_coord_x][new_coord_y].tiletype;
                        if((x > 0) & ((current_tiletype == mountain)|(current_tiletype == ocean)|(current_tiletype == river)|(current_tiletype == lake)))
                        {
                            valid_river = false;
                            break;
                        }



                        for(int m = -river_nonproximity_radius; m < river_nonproximity_radius; m++)
                        {
                            int new_coord_x_radius = new_coord_x + m;
                            if((new_coord_x_radius  < 0) | (new_coord_x_radius  >= tile_rows))
                            {
                                continue;
                            }

                            
                            for(int n = -river_nonproximity_radius; n < river_nonproximity_radius; n++)
                            {
                                int new_coord_y_radius = new_coord_y + n;
                                if((new_coord_y_radius < 0) | (new_coord_y_radius >= tile_cols))
                                {
                                    continue;
                                }
                                enum TileType current_tile = mymap.tiles[new_coord_x_radius][new_coord_y_radius].tiletype;
                                if(current_tile == river)
                                {
                                    valid_river = false;
                                    break;
                                }
                            }
                        }






                    }
                    if(valid_river)
                    {
                        for (int x = 0; x < river_length; x++)
                        {
                            
                            int x_off = offsets.x_offsets.data[x];
                            int y_off = offsets.y_offsets.data[x];
                            int new_coord_x = i + x_off;
                            int new_coord_y = j + y_off;

                            if((mymap.tiles[new_coord_x][new_coord_y].tiletype == ocean) & (x == 0))
                            {
                                continue;
                            }

                            //make river tile
                            mymap.tiles[new_coord_x][new_coord_y].tiletype = river;

                            //set non river zone
                            for(int m = -safe_zone_radius; m < safe_zone_radius; m++)
                            {
                                int new_coord_x_radius = new_coord_x + m;
                                if((new_coord_x_radius < 0) | (new_coord_x_radius >= tile_rows))
                                {
                                    continue;
                                }

                            
                                for(int n = -safe_zone_radius; n < safe_zone_radius; n++)
                                {
                                    int new_coord_y_radius = new_coord_y + n;
                                    if((new_coord_y_radius < 0) | (new_coord_y_radius >= tile_cols))
                                    {
                                        continue;
                                    }
                                    river_block_matrix[new_coord_x_radius][new_coord_y_radius] = 1;
                                    //printf("%d %d, ",new_coord_x_radius,new_coord_y_radius);
                                }
                            }
                        }

                        
                    }


                    

                    //printf("%d ",rand_value);
                }
            }
        }
    }

    for(int i = 0; i < mymap.rows; i++){
        for(int j = 0; j < mymap.cols; j++){
            if(mymap.tiles[i][j].tiletype == river)
            {
                mymap.tiles[i][j].food = 2;
                if(mymap.tiles[i][j].resource == 0)
                {
                    mymap.tiles[i][j].production= 1;
                }
            }


        }
    }

    //symmetry across y-axis if two players
    if(num_players == 2)
    {
        for(int i = 0; i < mymap.rows; i++)
        {
            for(int j = 0; j < mymap.cols/2; j++)
            {
                int mirror_j = mymap.cols/2 - j - 1;
                mymap.tiles[i][mirror_j + (mymap.cols/2)] = mymap.tiles[i][j];
            }
        }
    }
    //balancing map if 4 players and square map region: rotation 4 ways
    else if((num_players == 4) & (tile_cols == tile_rows))
    {
            int half_size = tile_rows / 2;  // Size of the region to rotate

        //Temp rotation matrix
        struct TileData rotation_0[tile_rows][tile_rows], rotation_90[tile_rows][tile_rows];
        struct TileData rotation_180[tile_rows][tile_rows], rotation_270[tile_rows][tile_cols];

        //Extract top left to rotate
        for (int i = 0; i < half_size; i++) {
            for (int j = 0; j < half_size; j++) {
                rotation_0[i][j] = mymap.tiles[i][j];
            }
        }

        //generate all rotated regions
        for (int i = 0; i < half_size; i++) {
            for (int j = 0; j < half_size; j++) {
                rotation_90[j][half_size - 1 - i] = rotation_0[i][j];//90
                rotation_180[half_size - 1 - i][half_size - 1 - j] = rotation_0[i][j];//180
                rotation_270[half_size - 1 - j][i] = rotation_0[i][j];//270
            }
        }
        //note top left is unchanged since its what we used to generate the other regions
        //top right region
        for (int i = 0; i < half_size; i++) {
            for (int j = half_size; j <tile_rows; j++) {
                mymap.tiles[i][j] = rotation_90[i][j - half_size];
            }
        }

        //bottom left region
        for (int i = half_size; i <tile_rows; i++) {
            for (int j = 0; j < half_size; j++) {
                mymap.tiles[i][j] = rotation_270[i - half_size][j];
            }
        }

        //bottom right region
        for (int i = half_size; i <tile_rows; i++) {
            for (int j = half_size; j <tile_rows; j++) {
                mymap.tiles[i][j] = rotation_180[i - half_size][j - half_size];
            }
        }
    }

    return mymap;
}

struct MapData map_initialize_current()
{   
    struct MapData * mymap = calloc(1, sizeof(struct MapData));
    struct MapData check = map_initialize_default();
    memcpy(mymap, &check, sizeof(struct MapData));
    //check to see if the random generation caused a surface with too many mountains or hills
    //otherwise generate map again
    float mountain_ratio = 0;
    float hill_ratio = 0;





    //compute sums
    for (int i = 0; i <tile_rows; i++) {
        for (int j = 0; j < tile_cols; j++) {
            enum TileType tile = mymap->tiles[i][j].tiletype;
            if(tile == mountain)
            {
                mountain_ratio++;
            }
            else if(tile == hill)
            {
                hill_ratio++;
            }
        }
    }
    //get ration from sums
    mountain_ratio/= (tile_cols * tile_rows);
    hill_ratio/= (tile_cols * tile_rows);

    //printf("%f %f", mountain_ratio, hill_ratio);
    while(mountain_ratio > 0.1)
    {   
        check = map_initialize_default();
        memcpy(mymap, &check, sizeof(struct MapData));
        //check to see if the random generation caused a surface with too many mountains or hills
        //otherwise generate map again

        mountain_ratio = 0;



        //compute sums
        for (int i = 0; i <tile_rows; i++) {
            for (int j = 0; j < tile_cols; j++) {
                enum TileType tile = mymap->tiles[i][j].tiletype;
                if(tile == mountain)
                {
                    mountain_ratio++;
                }
                else if(tile == hill)
                {
                    hill_ratio++;
                }
            }
        }
        //get ration from sums
        mountain_ratio/= (tile_cols * tile_rows);
        hill_ratio/= (tile_cols * tile_rows);

        //printf("MOUNTAIN RATIO %f", mountain_ratio);
    }
    //printf("DONE");
    return *mymap;
}





//int main() {
    
    //struct MapData map_a = map_initialize_default();
    //printmap(map_a);
    //return 0;
//}