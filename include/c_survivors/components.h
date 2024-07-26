#ifndef C_SURVIVORS_COMPONENTS_H
#define C_SURVIVORS_COMPONENTS_H

#include <raylib.h>

typedef struct {
  Vector2 position;
  Vector2 velocity;
} Movement;

typedef struct {
  int network_id;
  bool is_local;
} NetworkComponent;

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

typedef struct {
  Color color;
  float radius;
} Renderable;

typedef enum { CHARACTER_LEO, CHARACTER_POW } CharacterType;

typedef struct {
  CharacterType type;
} Character;

#endif // C_SURVIVORS_COMPONENTS_H
