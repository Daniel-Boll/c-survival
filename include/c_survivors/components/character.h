#pragma once

#include <flecs.h>

typedef enum { CHARACTER_LEO, CHARACTER_POW } CharacterType;

typedef struct {
  CharacterType type;
} Character;

ECS_COMPONENT_DECLARE(Character);
