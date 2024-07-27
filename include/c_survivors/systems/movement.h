#pragma once

#include <flecs.h>

void MovementSystem(ecs_iter_t *it);

ECS_SYSTEM_DECLARE(MovementSystem);
