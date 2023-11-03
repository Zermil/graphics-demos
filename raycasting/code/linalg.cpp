#include "linalg.h"
#include <math.h>

Vec2f vec2f_add(Vec2f a, Vec2f b)
{
    return { a.x + b.x, a.y + b.y };
}

Vec2f vec2f_sub(Vec2f a, Vec2f b)
{
    return { a.x - b.x, a.y - b.y };
}

Vec2f vec2f_mult(Vec2f a, float b)
{
    return { a.x * b, a.y * b };
}

float vec2f_dot(Vec2f a, Vec2f b)
{
    return(a.x * b.x + a.y + b.y);
}

float vec2f_len(Vec2f a)
{
    return(sqrtf(a.x*a.x + a.y*a.y));
}

Vec2f vec2f_rot(Vec2f a, float angle)
{
    Vec2f b = {0};
    b.x = a.x * cosf(angle) - a.y * sinf(angle);
    b.y = a.x * sinf(angle) + a.y * cosf(angle);
    
    return(b);
}
