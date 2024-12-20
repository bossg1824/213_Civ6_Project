#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "civ.h"
#include "civ_server.h"
#include "socket.h"
#include "game_play.c"

void * civ_server_side_init (void * args){
    struct server_func_args * arguments = args;

    unsigned short port = 0;
    int server_socket_fd = server_socket_open(&port);
    if(server_socket_fd == -1){
        perror("Server socket did not open?");
        return NULL;
    }   

    if(listen(server_socket_fd, 1)){
        perror("listen failed");
        return NULL;
    } 

    if(arguments->player_name == NULL){
        printf("Listening on port %u for player %d\n", port, arguments->player_id);
    } else {
        printf("Listening on port %u for player %d: %s\n", port, arguments->player_id, arguments->player_name);    
    }

    int client_socket_fd = server_socket_accept(server_socket_fd);
    if(client_socket_fd == -1){
        perror("accept failed");
        return NULL;
    }
    struct PlayerData * hosted_player = &(find_player(arguments->player_id, arguments->initial_board->players)->data);
    send_civ_state(client_socket_fd, arguments->initial_board);
    if(hosted_player->civ_name == NULL){
        char * name = recieve_name(client_socket_fd);
        if(name == NULL){
            *(arguments->error) = true;
            for(int i = 0; i < arguments->times_to_post; i++){
                sem_post(arguments->sem);
            }
            return NULL;
        }
        hosted_player->civ_name = name;
    }

    return NULL;
}   

void * civ_client_side (void * args){
    struct client_func_args * arguments = args;
    
    int socket_fd = socket_connect(arguments->server_name, arguments->port);
    if(socket_fd == -1){
        perror("Failed to connect");
        return NULL;
    }

    struct player_port * returning = calloc(1, sizeof(struct player_port));

    returning->socket_fd = socket_fd;
    
    return returning;
}



int send_civ_state (int fd, const struct GameState * sending){
    printf("sending\n");
    printf("Client %d\n", fd);

    size_t game_size = size_of_civ_state(sending);
    if(write(fd, &game_size, sizeof(size_t)) != sizeof(size_t)){
        perror("failed writing size");
        return -1;
    }
    
    size_t bytes_written = 0;
    uint8_t * byte_array = game_state_to_byte_array(sending);
    while(bytes_written < game_size){
        
        ssize_t rc = write(fd, byte_array + bytes_written, game_size - bytes_written);

        if(rc <= 0){
            return -1;
        }

        bytes_written += rc;
    }

    free(byte_array);

    return 0;
}

struct GameState * recieve_civ_board (int fd){
    printf("to server %d\n", fd);
    size_t game_size;
    if(read(fd, &game_size, sizeof(size_t)) != sizeof(size_t)){
        perror("failed reading size");
        return NULL;
    }
    
    uint8_t * game_array = calloc(1, sizeof(uint8_t) * game_size);

    size_t bytes_read = 0;
    while(bytes_read < game_size){

        ssize_t rc = read(fd, game_array + bytes_read, game_size - bytes_read);

        if(rc <= 0){
            return NULL;
        }

        bytes_read += rc;
    }

    struct GameState * game = byte_array_to_game_state(game_array, game_size);
    free(game_array);

    return game; 
}

char * recieve_name(int fd){
    return NULL;
}