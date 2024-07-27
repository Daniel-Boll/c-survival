#include <c_survivors/components/movement.h>
#include <c_survivors/components/network.h>
#include <c_survivors/manager/game.h>
#include <c_survivors/networking.h>
#include <string.h>
#include <third_party/log/log.h>

extern GameManager* g_game_manager;

void broadcast_player_join(NetworkContext* context, int client_id) {
  if (!context->is_server) return;

  uint8_t buffer[sizeof(PacketType) + sizeof(int)];
  PacketType type = PACKET_PLAYER_JOIN;
  memcpy(buffer, &type, sizeof(PacketType));
  memcpy(buffer + sizeof(PacketType), &client_id, sizeof(int));

  network_broadcast(context, buffer, sizeof(buffer), PACKET_PLAYER_JOIN, true);
}

void send_existing_players(NetworkContext* context, ENetPeer* new_peer) {
  if (!context->is_server) return;

  ecs_query_t* q = ecs_query(g_game_manager->world, {.terms = {{.id = ecs_id(NetworkComponent)}}});

  ecs_iter_t it = ecs_query_iter(g_game_manager->world, q);
  while (ecs_query_next(&it)) {
    NetworkComponent* nc = ecs_field(&it, NetworkComponent, 1);
    for (int i = 0; i < it.count; i++) {
      uint8_t buffer[sizeof(PacketType) + sizeof(int)];
      PacketType type = PACKET_PLAYER_JOIN;
      memcpy(buffer, &type, sizeof(PacketType));
      memcpy(buffer + sizeof(PacketType), &nc[i].network_id, sizeof(int));

      ENetPacket* packet = enet_packet_create(buffer, sizeof(buffer), ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(new_peer, 0, packet);
    }
  }

  ecs_query_fini(q);
}

void network_handle_events(NetworkContext* context) {
  ENetEvent event;
  while (enet_host_service(context->host, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        log_info("A new client connected from %x:%u.", event.peer->address.host,
                 event.peer->address.port);
        event.peer->data
            = (void*)(intptr_t)(context->is_server ? context->host->connectedPeers : 0);
        if (context->is_server) {
          int client_id = (intptr_t)event.peer->data;
          game_manager_create_player(g_game_manager, client_id, false);
          broadcast_player_join(context, client_id);
          send_existing_players(context, event.peer);
        }
        break;
      case ENET_EVENT_TYPE_RECEIVE: {
        PacketType* type = (PacketType*)event.packet->data;
        switch (*type) {
          case PACKET_PLAYER_POSITION: {
            int client_id = (intptr_t)event.peer->data;
            Vector2* pos = (Vector2*)(event.packet->data + sizeof(PacketType));

            ecs_query_t* q = ecs_query(
                g_game_manager->world,
                {.terms = {{.id = ecs_id(NetworkComponent)}, {.id = ecs_id(Movement)}}});

            ecs_iter_t it = ecs_query_iter(g_game_manager->world, q);
            while (ecs_query_next(&it)) {
              NetworkComponent* nc = ecs_field(&it, NetworkComponent, 1);
              Movement* m = ecs_field(&it, Movement, 2);
              for (int i = 0; i < it.count; i++) {
                if (nc[i].network_id == client_id) {
                  m[i].position = *pos;
                  break;
                }
              }
            }

            ecs_query_fini(q);

            if (context->is_server) {
              network_broadcast(context, event.packet->data, event.packet->dataLength,
                                PACKET_PLAYER_POSITION, false);
            }
          } break;
          case PACKET_PLAYER_JOIN:
            if (!context->is_server) {
              int* client_id = (int*)(event.packet->data + sizeof(PacketType));
              game_manager_create_player(g_game_manager, *client_id, false);
            }
            break;
          case PACKET_PLAYER_LEAVE:
            if (!context->is_server) {
              int* client_id = (int*)(event.packet->data + sizeof(PacketType));
              game_manager_remove_player(g_game_manager, *client_id);
            }
            break;
          case PACKET_PLAYER_INPUT:
            break;
        }
        enet_packet_destroy(event.packet);
      } break;
      case ENET_EVENT_TYPE_DISCONNECT:
        log_info("Client %d disconnected.", (int)(intptr_t)event.peer->data);
        if (context->is_server) {
          int client_id = (intptr_t)event.peer->data;
          game_manager_remove_player(g_game_manager, client_id);
          uint8_t buffer[sizeof(PacketType) + sizeof(int)];
          PacketType type = PACKET_PLAYER_LEAVE;
          memcpy(buffer, &type, sizeof(PacketType));
          memcpy(buffer + sizeof(PacketType), &client_id, sizeof(int));
          network_broadcast(context, buffer, sizeof(buffer), PACKET_PLAYER_LEAVE, true);
        }
        event.peer->data = NULL;
        break;
      case ENET_EVENT_TYPE_NONE:
        break;
    }
  }
}
void network_broadcast(NetworkContext* context, const void* data, size_t data_size, PacketType type,
                       bool reliable) {
  if (!context->is_server) return;

  ENetPacket* packet = enet_packet_create(NULL, data_size + sizeof(PacketType),
                                          reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

  *(PacketType*)packet->data = type;
  memcpy(packet->data + sizeof(PacketType), data, data_size);

  enet_host_broadcast(context->host, 0, packet);
}

bool network_init(void) {
  if (enet_initialize() != 0) {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return false;
  }
  return true;
}

void network_deinit(void) { enet_deinitialize(); }

NetworkContext* network_create_server(void) {
  NetworkContext* context = malloc(sizeof(NetworkContext));
  if (!context) return NULL;

  ENetAddress address;
  address.host = ENET_HOST_ANY;
  address.port = SERVER_PORT;

  context->host = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
  if (!context->host) {
    free(context);
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    return NULL;
  }

  context->is_server = true;
  context->peer = NULL;
  context->client_id = -1;
  return context;
}

NetworkContext* network_create_client(const char* server_address) {
  NetworkContext* context = malloc(sizeof(NetworkContext));
  if (!context) return NULL;

  context->host = enet_host_create(NULL, 1, 2, 0, 0);
  if (!context->host) {
    free(context);
    fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
    return NULL;
  }

  ENetAddress address;
  enet_address_set_host(&address, server_address);
  address.port = SERVER_PORT;

  context->peer = enet_host_connect(context->host, &address, 2, 0);
  if (!context->peer) {
    enet_host_destroy(context->host);
    free(context);
    fprintf(stderr, "No available peers for initiating an ENet connection.\n");
    return NULL;
  }

  context->is_server = false;
  context->client_id = -1;
  return context;
}

void network_update(NetworkContext* context) { network_handle_events(context); }

void network_send(NetworkContext* context, const void* data, size_t data_size, PacketType type,
                  bool reliable) {
  if (!context->peer) return;

  ENetPacket* packet = enet_packet_create(NULL, data_size + sizeof(PacketType),
                                          reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

  *(PacketType*)packet->data = type;
  memcpy(packet->data + sizeof(PacketType), data, data_size);

  enet_peer_send(context->peer, 0, packet);
}
