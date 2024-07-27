#pragma once

#include <c_survivors/networking.h>
#include <flecs.h>

typedef struct {
  ecs_world_t *world;
  NetworkContext *net_context;
  bool is_server;
} GameManager;

GameManager *game_manager_create(bool is_server);
void game_manager_destroy(GameManager *manager);
void game_manager_update(GameManager *manager);
ecs_entity_t game_manager_create_player(GameManager *manager, int client_id, bool is_local);
void game_manager_remove_player(GameManager *manager, int client_id);
