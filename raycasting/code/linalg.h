#ifndef LINALG_H
#define LINALG_H

struct Vec2f {
    float x;
    float y;
};

Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2f vec2f_mult(Vec2f a, float b);
Vec2f vec2f_rot(Vec2f a, float angle);
float vec2f_dot(Vec2f a, Vec2f b);
float vec2f_len(Vec2f a);

#endif // LINALG_H
