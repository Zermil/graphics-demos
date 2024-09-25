#include "../main.cpp"

struct Vec2f {
    float x;
    float y;
};

internal void render_line(Vec2f A, Vec2f B, unsigned int col)
{
    Vec2f dir = {
        B.x - A.x,
        B.y - A.y
    };
    
    Vec2f start = {
        floorf(A.x),
        floorf(A.y)
    };
    
    float steps = fabsf(dir.x) > fabsf(dir.y) ? fabsf(dir.x) : fabsf(dir.y);
    Vec2f ray_step = {
        dir.x / steps,
        dir.y / steps
    };

    for (int i = 0; i <= (int) steps; ++i) {
        ARRAY_AT(frame_buffer, (int) start.x, (int) start.y) = col;
        start.x += ray_step.x;
        start.y += ray_step.y;
    }    
}

internal void render(float dt)
{
    UNUSED(dt);
    
    Vec2f A = { 0.0f, 0.0f };
    Vec2f B = { WIDTH*.5f, 0.0f };
    Vec2f C = { WIDTH, 0.0f };
    Vec2f D = { WIDTH, HEIGHT*.5f };
    Vec2f E = { WIDTH, HEIGHT };
    Vec2f F = { WIDTH*.5f, HEIGHT };
    Vec2f G = { 0, HEIGHT };
    Vec2f H = { 0, HEIGHT*.5f };
    
    render_line(A, E, 0xFF0000FF);
    render_line(B, F, 0x00FF00FF);
    render_line(C, G, 0x0000FFFF);
    render_line(D, H, 0xFFFF00FF);
}
