#pragma once

#include <flecs.h>
#include <raylib.h>

typedef struct {
  Vector2 position;
  Vector2 velocity;
} Movement;

ecs_entity_t FLECS_IDMovementID_;
