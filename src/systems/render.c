#include <c_survivors/components/movement.h>
#include <c_survivors/components/renderable.h>
#include <c_survivors/systems/render.h>
#include <flecs.h>
#include <raylib.h>

void RenderSystem(ecs_iter_t *it) {
  Movement *m = ecs_field(it, Movement, 0);
  Renderable *r = ecs_field(it, Renderable, 1);

  for (int i = 0; i < it->count; i++) {
    DrawCircleV(m[i].position, r[i].radius, r[i].color);
  }
}
