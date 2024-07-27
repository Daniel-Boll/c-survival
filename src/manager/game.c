#include <c_survivors/components/character.h>
#include <c_survivors/components/movement.h>
#include <c_survivors/components/network.h>
#include <c_survivors/components/renderable.h>
#include <c_survivors/components/stats.h>
#include <c_survivors/manager/game.h>
#include <c_survivors/systems/movement.h>
#include <c_survivors/systems/network.h>
#include <c_survivors/systems/render.h>
#include <flecs.h>
#include <raylib.h>
#include <stdlib.h>

GameManager *game_manager_create(bool is_server) {
  GameManager *manager = malloc(sizeof(GameManager));
  manager->world = ecs_init();
  manager->is_server = is_server;

  // Register components
  {
    ecs_component_desc_t desc = {0};
    ecs_entity_desc_t edesc = {0};
    edesc.id = FLECS_IDMovementID_;
    edesc.use_low_id = true;
    edesc.name = "Movement";
    edesc.symbol = "Movement";
    desc.entity = ecs_entity_init(manager->world, &edesc);
    desc.type.size = ((ecs_size_t)(sizeof(Movement)));
    desc.type.alignment = (int64_t) __alignof__(Movement);
    FLECS_IDMovementID_ = ecs_component_init(manager->world, &desc);
  };
  ECS_COMPONENT_DEFINE(manager->world, Stats);
  ECS_COMPONENT_DEFINE(manager->world, Renderable);
  ECS_COMPONENT_DEFINE(manager->world, Character);
  ECS_COMPONENT_DEFINE(manager->world, NetworkComponent);

  // Register systems
  ECS_SYSTEM_DEFINE(manager->world, MovementSystem, EcsOnUpdate, Movement, Stats);
  ECS_SYSTEM_DEFINE(manager->world, NetworkSystem, EcsOnUpdate, Movement, NetworkComponent);

  if (!is_server) {
    ECS_SYSTEM_DEFINE(manager->world, RenderSystem, EcsOnUpdate, Movement, Renderable);
  }

  // Initialize networking
  manager->net_context = is_server ? network_create_server() : network_create_client("localhost");

  return manager;
}

void game_manager_destroy(GameManager *manager) {
  ecs_fini(manager->world);
  network_deinit();
  free(manager);
}

void game_manager_update(GameManager *manager) {
  network_update(manager->net_context);
  ecs_progress(manager->world, 0);
}

ecs_entity_t game_manager_create_player(GameManager *manager, int client_id, bool is_local) {
  ecs_entity_t player = ecs_new(manager->world);

  ecs_set(manager->world, player, Movement, {(Vector2){400, 300}, (Vector2){0, 0}});
  ecs_set(manager->world, player, Stats, {100, 100, 10, 200, 0, 1, 1, 0.05, 1, 1});
  ecs_set(manager->world, player, Renderable, {is_local ? RED : BLUE, 20});
  ecs_set(manager->world, player, Character, {CHARACTER_LEO});
  ecs_set(manager->world, player, NetworkComponent, {client_id, is_local});
  return player;
}

void game_manager_remove_player(GameManager *manager, int client_id) {
  ecs_query_t *q = ecs_query(manager->world, {.terms = {{.id = ecs_id(NetworkComponent)}}});

  ecs_iter_t it = ecs_query_iter(manager->world, q);
  while (ecs_query_next(&it)) {
    NetworkComponent *nc = ecs_field(&it, NetworkComponent, 0);
    for (int i = 0; i < it.count; i++) {
      if (nc[i].network_id == client_id) {
        ecs_delete(manager->world, it.entities[i]);
        break;
      }
    }
  }

  ecs_query_fini(q);
}
