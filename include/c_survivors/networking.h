#pragma once

#include <enet/enet.h>
#include <stdbool.h>

#define MAX_CLIENTS 4
#define SERVER_PORT 7777

typedef struct {
  ENetHost *host;
  ENetPeer *peer;
  bool is_server;
  int client_id;
} NetworkContext;

typedef enum {
  PACKET_PLAYER_POSITION,
  PACKET_PLAYER_INPUT,
  PACKET_PLAYER_JOIN,
  PACKET_PLAYER_LEAVE
} PacketType;

bool network_init(void);
void network_deinit(void);
NetworkContext *network_create_server(void);
NetworkContext *network_create_client(const char *server_address);
void network_update(NetworkContext *context);
void network_send(NetworkContext *context, const void *data, size_t data_size, PacketType type,
                  bool reliable);
void network_broadcast(NetworkContext *context, const void *data, size_t data_size, PacketType type,
                       bool reliable);
void network_handle_events(NetworkContext *context);
