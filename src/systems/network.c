#include <c_survivors/components.h>
#include <c_survivors/networking.h>
#include <flecs.h>
#include <raylib.h>
#include <third_party/log/log.h>

extern NetworkContext *g_net_context;

void NetworkSystem(ecs_iter_t *it) {
  Movement *m = ecs_field(it, Movement, 0);
  NetworkComponent *nc = ecs_field(it, NetworkComponent, 1);

  for (int i = 0; i < it->count; i++) {
    log_debug("[%b] Client ID: %d with position: %f, %f", nc[i].is_local, nc[i].network_id,
              m[i].position.x, m[i].position.y);
    if (nc[i].is_local) {
      // Send local player position to the network
      network_send(g_net_context, &m[i].position, sizeof(Vector2), PACKET_PLAYER_POSITION, false);
    }
  }
}
