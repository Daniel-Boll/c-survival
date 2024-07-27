#include <c_survivors/components/movement.h>
#include <c_survivors/components/network.h>
#include <c_survivors/manager/game.h>
#include <c_survivors/networking.h>
#include <c_survivors/systems/network.h>
#include <flecs.h>
#include <raylib.h>
#include <third_party/log/log.h>

extern GameManager *g_game_manager;

void NetworkSystem(ecs_iter_t *it) {
  Movement *m = ecs_field(it, Movement, 0);
  NetworkComponent *nc = ecs_field(it, NetworkComponent, 1);

  for (int i = 0; i < it->count; i++) {
    if (nc[i].is_local) {
      log_debug("[%d] @ %f, %f", nc[i].network_id, m[i].position.x, m[i].position.y);
      network_send(g_game_manager->net_context, &m[i].position, sizeof(Vector2),
                   PACKET_PLAYER_POSITION, false);
    }
  }
}
