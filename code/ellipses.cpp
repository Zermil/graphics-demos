#include "main.cpp"

// @Note: There is an algorithm for midpoint ellipse, but I feel
// like this gets the points across and is easier to understand at a glance.
internal void render_draw_ellipse(int cx, int cy, int rx, int ry)
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
                ARRAY_AT(frame_buffer, x, y) = 0xFF0000FF;
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
    UNUSED(dt);

    render_draw_ellipse(WIDTH/2, HEIGHT/2, 600, 120);
}
