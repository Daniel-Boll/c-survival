#include <c_survivors/components.h>
#include <c_survivors/networking.h>
#include <flecs.h>
#include <raylib.h>
#include <third_party/log/log.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

extern void MovementSystem(ecs_iter_t *it);
extern void NetworkSystem(ecs_iter_t *it);
extern void RenderSystem(ecs_iter_t *it);

NetworkContext *g_net_context;
ecs_world_t *g_world;

int main(int argc, char *argv[]) {
  bool is_server = (argc > 1 && strcmp(argv[1], "server") == 0);

  // Initialize networking
  if (!network_init()) {
    return 1;
  }

  if (is_server) {
    g_net_context = network_create_server();
    log_info("Server started. Waiting for connections...");
  } else {
    g_net_context = network_create_client("localhost");
    InitWindow(800, 600, "C Survivors");
    SetTargetFPS(60);
  }

  if (!g_net_context) {
    network_deinit();
    return 1;
  }

  // Initialize flecs
  g_world = ecs_init();

  // Register components
  ECS_COMPONENT(g_world, Movement);
  ECS_COMPONENT(g_world, Stats);
  ECS_COMPONENT(g_world, Renderable);
  ECS_COMPONENT(g_world, Character);
  ECS_COMPONENT(g_world, NetworkComponent);

  // Register systems
  ECS_SYSTEM(g_world, MovementSystem, EcsOnUpdate, Movement, Stats);
  ECS_SYSTEM(g_world, NetworkSystem, EcsOnUpdate, Movement, NetworkComponent);

  // Create player entity
  if (!is_server) {
    ECS_SYSTEM(g_world, RenderSystem, EcsOnUpdate, Movement, Renderable);

    ecs_entity_t player = ecs_new(g_world);
    ecs_set(g_world, player, Movement, {(Vector2){400, 300}, (Vector2){0, 0}});
    ecs_set(g_world, player, Stats, {100, 100, 10, 200, 0, 1, 1, 0.05, 1, 1});
    ecs_set(g_world, player, Renderable, {RED, 20.f});
    ecs_set(g_world, player, Character, {CHARACTER_LEO});
    ecs_set(g_world, player, NetworkComponent, {g_net_context->client_id, true});
  }

  // Main game loop
  while (is_server || !WindowShouldClose()) {
    network_update(g_net_context);

    // Update
    ecs_progress(g_world, 0);

    // Draw
    if (!is_server) {
      BeginDrawing();
      ClearBackground(RAYWHITE);
      EndDrawing();
    }
  }

  // Cleanup
  ecs_fini(g_world);
  network_deinit();
  if (!is_server) {
    CloseWindow();
  }

  return 0;
}
