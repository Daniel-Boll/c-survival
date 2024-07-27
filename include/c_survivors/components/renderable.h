#pragma once

#include <flecs.h>
#include <raylib.h>

typedef struct {
  Color color;
  float radius;
} Renderable;

ECS_COMPONENT_DECLARE(Renderable);
