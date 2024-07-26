#include <c_survivors/components.h>
#include <flecs.h>
#include <raylib.h>
#include <raymath.h>

void MovementSystem(ecs_iter_t *it) {
    Movement *m = ecs_field(it, Movement, 0);
    Stats *s = ecs_field(it, Stats, 1);

    for (int i = 0; i < it->count; i++) {
        Vector2 input = {0};
        if (IsKeyDown(KEY_W)) input.y -= 1;
        if (IsKeyDown(KEY_S)) input.y += 1;
        if (IsKeyDown(KEY_A)) input.x -= 1;
        if (IsKeyDown(KEY_D)) input.x += 1;

        if (input.x != 0 || input.y != 0) {
            input = Vector2Normalize(input);
            m[i].velocity = Vector2Scale(input, s[i].moveSpeed);
        } else {
            m[i].velocity = (Vector2){0, 0};
        }

        m[i].position = Vector2Add(m[i].position, Vector2Scale(m[i].velocity, GetFrameTime()));
    }
}
