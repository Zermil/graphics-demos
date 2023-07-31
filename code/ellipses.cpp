#include "../main.cpp"

#define MAX_SQUISH 150
#define MAX_SQUASH 100
#define MIN_SQUISH 30
#define MIN_SQUASH 20
#define ANGLE_SPEED 0.50f

global float g_angle = 0.0f;

// @Note: Linearly interpolate between two values and provided percent
// (lerpf my beloved)
internal inline float lerpf(float a, float b, float t)
{
    return(a*(1.0f - t) + b*t);
}

// @Note: There is an algorithm for midpoint ellipse, but I feel
// like this gets the points across and is easier to understand at a glance.
internal void render_draw_ellipse(int cx, int cy, int rx, int ry, uint32_t colour)
{
    for (int y = cy - ry; y <= cy + ry; ++y) {
        if (y < 0 || y >= HEIGHT) continue;
        
        for (int x = cx - rx; x <= cx + rx; ++x) {
            if (x < 0 || x >= WIDTH) continue;

            int dx = x - cx;
            int dy = y - cy;

            float xcheck = (float) (dx*dx) / (float) (rx*rx);
            float ycheck = (float) (dy*dy) / (float) (ry*ry);
            
            if (xcheck + ycheck <= 1.0f) {
                ARRAY_AT(frame_buffer, x, y) = colour;
            }

            // @Note: You could argue that we can just do something like this
            //
            // uint64_t xcheck = (uint64_t) (dx*dx)*(ry*ry);
            // uint64_t ycheck = (uint64_t) (dy*dy)*(rx*rx);
            // uint64_t rcheck = (uint64_t) (rx*rx)*(ry*ry);
            //
            // if (xcheck + ycheck <= rcheck) {
            //     ARRAY_AT(frame_buffer, x, y) = 0xFF0000FF;
            // }
            // 
            // This however is too much memory for something so simple.
            // Feel free to use this approach however, if what your designing for
            // has better performance on integers and you know your problem domain well.
        }
    }
}

internal void render(float dt)
{
    g_angle += TURNS(ANGLE_SPEED)*dt;
    
    // @Note: Sin value between 0..1
    float sine = (sinf(g_angle) + 1)/2;

    int rx = (int) (MAX_SQUISH*(1.0f - sine)) + MIN_SQUISH;
    int ry = (int) (MAX_SQUASH*sine) + MIN_SQUASH;
    int cx = WIDTH/2;
    int cy = HEIGHT - (int) lerpf((float) ry, HEIGHT/2, sine);
    
    render_draw_ellipse(cx, cy, rx, ry, 0xEF7627FF);
}
