#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define no
#include "civ.h"
#include "civ_server.h"
#include "move.h"
#include "civ_server.c"
#include "display.c"
#include "game_initialize.c"
#include "game_play.c"
#include "move.c"


void print_server_usage(char * name);

void print_client_usage(char * name);

void print_all_usage(char * name);

struct GameState * load_from_file(FILE * load_file);

void save_to_file(FILE * save_to, struct GameState * save_state);

struct move set_initial_move(struct GameState* initial_state);

int main(int argc, char ** argv){
    //if there are not enough command line args
    if(argc < 2){
        //tell em how to and what for (or something of the sort)
        print_all_usage(argv[0]);
        return -1;
    } 

    //if the command line argument indicates client functionality
    if(!strcmp(argv[1], "c")){
        if(argc != 3){
            print_client_usage(argv[0]);
            return -1;
        }
        //do client things
        return 0;
    } 

    FILE * load_file = NULL;
    FILE * save_file = NULL;

    //if the command line argument indicates server functionality
    if(!strcmp(argv[1], "s")){
        //if we aren't using the defaults
        if(argc != 2){
            //if the user has not supplied exactly three extra arguments
            if(argc != 4){
                print_server_usage(argv[0]);
                return -1;
            }
            //if the user has indicated they wish to load a file
            if(!strcmp(argv[2], "load")){
                load_file = fopen(argv[3], "r");
                if(load_file == NULL){
                    printf("Failed to open %s :\n", argv[3]);
                    perror("");
                    return -1;
                } 
                //if wanting to save to a specific file
            } else if(!strcmp(argv[2], "save")){
                save_file = fopen(argv[3], "w");
                if(save_file == NULL){
                    printf("Failed to open %s :\n", argv[3]);
                    perror("");
                    return -1;
                }
            } else {
                //open the file to load from
                load_file = fopen(argv[2], "r");
                if(load_file == NULL){
                    printf("Failed to open %s :\n", argv[2]);
                    perror("");
                    return -1;
                }
                //open the file to save to
                save_file = fopen(argv[3], "w");
                if(save_file == NULL){
                    printf("Failed to open %s :\n", argv[3]);
                    perror("");
                    return -1;
                }
            }
        }
        //if user selected neither server nor client side
    } else {
        print_all_usage(argv[0]);
        return -1;
    }

    //assuming server side

    //if there is no saveFile specified
    if(save_file == NULL){
        //open the default save file
        save_file = fopen("civ_save.txt", "w");
        if(save_file == NULL){
            perror("Failed to open civ_save.txt :");
            return -1;
        }
    }

    sem_t * move_recieved = calloc(1, sizeof(sem_t)); //the semaphore that stops threads when they aren't waiting to read a move
    sem_init(move_recieved, 0, 0);

    sem_t * names_lock = calloc(1, sizeof(sem_t));
    sem_init(names_lock, 0, 1);

    struct GameState * server_state;
    struct move_stack * moves_made = create_move_stack(); //start the stack of tracking moves
    bool * errors = calloc(1, sizeof(bool)); //for threads to indicate to each other that there has been an error (and for us to know to save)
    
    //if a file was provided to load from
    if(load_file != NULL){ 
        //load from the file provided
        server_state = load_from_file(load_file);

        //if no file was specified
    } else { 
        //grab memory for the gamestate
        server_state = calloc(1, sizeof(struct GameState));
        //indicate that no moves have been made yet
        server_state->player_turn = default_int;
        //create the initial map
        server_state->mymap = map_initialize_default();
        //create the initial players
        server_state->players = init_players();
        //place the starting cities
        place_initial_cities(&(server_state->mymap), &(server_state->players));

        int max_turns = -1;
        do{
            printf("What is the max turns the game can last? (enter a positive integer, at least 1)\n");
            int readargs = scanf("%d", &max_turns);

            //if we read no argument or an unreasonable amount of turns is selected
            if(readargs != 1 || max_turns < 1){
                //clear the input buffer
                while(getchar() != '\n');
            }
        } while(max_turns < 1);

        server_state->turn_limit = max_turns;
    }

    
    add_move(moves_made, set_initial_move(server_state));

    //setup an array for all the players
    int human_players = server_state->players.length;

    
    pthread_t ** threads = calloc(human_players, sizeof(pthread_t *));
    struct PlayerData_Node * player_hosted = server_state->players.head;
    for(int i = 0; i < server_state->players.length; i++){
        pthread_t * thread = calloc(1, sizeof(pthread_t));
        threads[i] = thread;
        
        struct server_func_args * hosting_args = calloc(1, sizeof(struct server_func_args));
        hosting_args->player_id = player_hosted->data.player_id;
        hosting_args->player_name = player_hosted->data.civ_name;
        hosting_args->global_moves_made = moves_made;
        hosting_args->error = errors;
        hosting_args->initial_board = server_state;
        hosting_args->sem = move_recieved;
        hosting_args->times_to_post = human_players;

        pthread_create(threads[i], NULL, &(civ_server_side_init), hosting_args);
        player_hosted = player_hosted->next;
    }

    if(human_players != server_state->players.length){
        for(int i  = human_players; i < server_state->players.length; i++){
            struct PlayerData_Node * cur_cpu = find_player(i, server_state->players);
            if(cur_cpu->data.civ_name == NULL){
                char * cpu_name = calloc(20, sizeof(char));
                sprintf(cpu_name, "Computer_%d", (i - human_players + 1));
                cur_cpu->data.civ_name = cpu_name;
            }
        }
        
        while(moves_made->head->data.turn <= server_state->turn_limit && !errors){
                //do computer moves if the time is right
                if(pos_in_list(&(server_state->players), moves_made->head->data.next_player_id) + 1 > human_players){
                        //struct move comp_move = get_comp_move();
                        //add_move(moves_made, comp_move);
                        update_game(server_state, move_peek(moves_made));
                        for(int i = 0; i < human_players; i++){
                            sem_post(move_recieved);
                        }
                } else {
                    sem_wait(move_recieved);
                }
        }
    }

    //make sure all threads end
    for(int i = 0; i < human_players; i++){
        pthread_join(*threads[i], NULL);
    }

    //if we had an error or someone quit
    if(errors){
        save_to_file(save_file, server_state);
    }

    fclose(save_file);
    return 0;
}


void print_server_usage(char * name){
    printf("Server Usage\n");
    printf("1. To play a fresh game with a default save file:\n");
    printf("%s s\n", name);
    printf("2. To load a game from a file and save to default save\n");
    printf("%s s load loadfrom.txt\n", name);
    printf("3. To play a fresh game but save to a specific file\n");
    printf("%s s save saveto.txt\n", name);
    printf("4 To load a game from a file and save to a specific file\n");
    printf("%s s loadfrom.txt saveto.txt\n", name);
    return;
}

void print_client_usage(char * name){
    printf("Client Usage\n");
    printf("%s c servername portnumber\n", name);
    return;
}

void print_all_usage(char * name){
    print_server_usage(name);
    print_client_usage(name);
    return;
}   

//loads a gamestate from a file, does literally no error checking on the file format
struct GameState* load_from_file(FILE * load_file){
        //make space for the size of the byte representation of the saved game
        size_t byte_size;
        //get the size of the byte representation
        fgets((char *) &byte_size, sizeof(size_t) + 1, load_file);

        //make space for the byte representation
        uint8_t * bytes_of_game = calloc(1, byte_size);
        //load the byte representation
        fgets((char *) bytes_of_game, byte_size + 1, load_file);

        //we are done with the file so close it
        fclose(load_file);

        //set up the game state from the bytes
        struct GameState * returning = byte_array_to_game_state(bytes_of_game, byte_size);

        //we are done with the bytes so free them
        free(bytes_of_game);

        return returning;
}

//saves a gamestate to a file by first saving the size of what we are about to write into the file
void save_to_file(FILE * save_to, struct GameState * save_state){
    size_t writing_size = size_of_civ_state(save_state);

    fwrite(&writing_size, sizeof(size_t), 1, save_to);

    uint8_t * bytes_array = game_state_to_byte_array(save_state);

    fwrite(bytes_array, sizeof(uint8_t), writing_size, save_to);

    free(bytes_array);
    return;
}

struct move set_initial_move(struct GameState* initial_state){
    struct move returning;
    struct move * zero_out = calloc(1, sizeof(struct move));
    returning = *zero_out;
    free(zero_out);
    if(initial_state->player_turn == default_int){
        returning.next_player_id = player_id_from_pos(&(initial_state->players), 0);
        return returning;
    }

    int pos_of_wanted_id = (pos_in_list(&(initial_state->players), initial_state->player_turn) + 1) % initial_state->players.length;

    //setting the correct turn numbers
    returning.turn = initial_state->turn_count;
    returning.next_player_id = player_id_from_pos(&(initial_state->players), pos_of_wanted_id);
    return returning;
}