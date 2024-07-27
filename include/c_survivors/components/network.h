#pragma once

#include <flecs.h>

typedef struct {
  int network_id;
  bool is_local;
} NetworkComponent;

ECS_COMPONENT_DECLARE(NetworkComponent);
