#include <c_survivors/manager/game.h>
#include <raylib.h>

GameManager* g_game_manager;

int main(int argc, char* argv[]) {
  bool is_server = (argc > 1 && strcmp(argv[1], "server") == 0);

  if (!is_server) {
    InitWindow(800, 600, "C Survivors");
    SetTargetFPS(60);
  }

  g_game_manager = game_manager_create(is_server);

  if (!is_server) {
    game_manager_create_player(g_game_manager, g_game_manager->net_context->client_id, true);
  }

  while (is_server || !WindowShouldClose()) {
    game_manager_update(g_game_manager);

    if (!is_server) {
      BeginDrawing();
      ClearBackground(RAYWHITE);
      EndDrawing();
    }
  }

  game_manager_destroy(g_game_manager);

  if (!is_server) {
    CloseWindow();
  }

  return 0;
}
