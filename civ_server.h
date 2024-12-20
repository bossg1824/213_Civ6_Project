#pragma once
#include "civ.h"
#include <semaphore.h>

struct server_func_args{
    int player_id; //what player id this connection is
    char * player_name; //what player we are
    struct move_stack * global_moves_made; //used to see moves that were made and when to send updates
    struct GameState * initial_board; //only used to send the starting board
    bool * error; //used to indicate errors to the server and that gameplay should stop (save to file)
    sem_t * sem; //used to signal threads to wake up after a move has been made
    int times_to_post; //times to post to the semaphore after recieving a move
    sem_t * name_lock;
    int * names_needed;
};

struct client_func_args{
    unsigned short port;
    char* server_name;
    struct GameState * client_game; //game board to update when recieving moves from server
};

//currently unused, initially used for when threads terminated after connecting with ports
struct player_port{
    int player_id;
    int socket_fd;
};

void * civ_server_side_init (void * args);

void * civ_client_side (void * args);

int send_civ_state (int fd, const struct GameState * sending);

struct GameState * recieve_civ_board (int fd);

char * recieve_name(int fd);

