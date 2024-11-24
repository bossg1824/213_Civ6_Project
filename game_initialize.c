#include <string.h>
#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>   
#include <sys/time.h> 
#include <time.h>
#include <math.h>
#include "civ.h"

void printmap(struct MapData map){
    for(int i = 0; i < map.cols; i++)
    {
        printf("\n");
        for(int j = 0; j < map.rows; j++)
        {
            printf("%d",map.tiles[i][j].food);
        }
    }
}

//radius should be an int between 1 and 200
//dist dependancy should be between 0 and 100
//denormalizer should be between 1 and 20
void perlin_noise_one_octave(int cols, int rows, int radius_to_consider, float dist_dependancy_constant, float denormalizer)
{   
    srand(time(NULL));
    int adjust_cols = cols + (radius_to_consider * 2) - 1;
    int adjust_rows = rows + (radius_to_consider * 2) - 1;
    float matrix [adjust_cols][adjust_rows];
    //create random noise
    for (int i = 0; i < adjust_cols; i++)
    {
        for (int j = 0; j < adjust_rows; j++)
        {
            matrix[i][j] = (float)rand() / (float)(RAND_MAX);
            //printf("%f ",matrix[i][j]);
        }
        //printf("\n");
    }

    float perlin_matrix [cols][rows];
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
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
            
            perlin_matrix[i][j] = updated_weighted_avg;         
            printf("%.2f ",updated_weighted_avg);
        }
        printf("\n");
    }
    //printf("%f ", avg_diff_run);
}

//using desmos and trial and error
//frequency is on 0 to 1 exclusive
void procedural_noise(int cols, int rows, float frequency)
{
    float distance_constant = 0.8;
    int radius = (int)round(pow(2,4 * (1 - frequency)));
    int decenterize_constant = (int)round(pow(2,2.34 * (1 - frequency)));
    perlin_noise_one_octave(rows,cols,radius,distance_constant,decenterize_constant);
}


struct MapData create_testmap1()
{
    struct MapData mymap;
    mymap.cols = tile_cols;
    mymap.rows = tile_rows;

    for(int i = 0; i < mymap.cols; i++){
        for(int j = 0; j < mymap.rows; j++){
            struct Tile_Coord coord;
            coord.x = i;
            coord.y = j;
            enum TileType tile_type = mountain;
            enum ResourceType resource = none;
            int civ_id_controlling = 0;
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
            
        }
    }
    return mymap;
}


int main() {
    
    //struct MapData map_a = create_testmap1();
    //printmap(map_a);

    procedural_noise(30,60,0.4);


    printf("Here\n"); // Added newline for output formatting
    return 0;
}