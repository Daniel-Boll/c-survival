#pragma once

#include <flecs.h>

typedef struct {
  float health;
  float maxHealth;
  float armor;
  float moveSpeed;
  float abilityHaste;
  float healthRegen;
  float projectileCount;
  float criticalChance;
  float areaSize;
  int level;
} Stats;

ECS_COMPONENT_DECLARE(Stats);
