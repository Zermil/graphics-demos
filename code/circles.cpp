#include "main.cpp"

#define CIRCLES_SIZE 8
#define CIRCLES_PAD 65
#define CIRCLES_WIDTH ((CIRCLES_SIZE - 1)*CIRCLES_PAD)

// @Note: Simple linear interpolation, probably the most useful
// and one of my favourite functions.
internal inline float lerpf(float a, float b, float t)
{
    return((a * (1.0f - t)) + (b * t));
}

// @Note: Could also use Bresenham's line drawing algorithm, but since
// we're just drawing horizontal lines it would be an over-kill.
internal void render_horizontal_line(int x_start, int x_stop, int y_start, uint32_t colour)
{
    if (y_start < 0 || y_start >= HEIGHT) return;
    
    for (int x = x_start; x <= x_stop; ++x) {
        if (x < 0 || x >= WIDTH) continue;
        ARRAY_AT(frame_buffer, x, y_start) = colour;
    }
}

// @Note: Bresenham's circle algorithm.
internal void render_fill_circle(int cx, int cy, int r, uint32_t colour)
{
    int p = 1 - r;
    int x = r;
    int y = 0;

    // @Note: Important! Otherwise you will be skipping the center middle
    // line based on the fact that in a while loop we're instantly incrementing y value.
    render_horizontal_line(cx-r, cx+r, cy, colour);
    
    while (x >= y) {
        y += 1;

        if (p <= 0) {
            p += 2*y + 1;
        } else {
            x -= 1;
            p += 2*y - 2*x + 1;
        }

        render_horizontal_line(cx-x, cx+x, cy-y, colour);
        render_horizontal_line(cx-x, cx+x, cy+y, colour);
        render_horizontal_line(cx-y, cx+y, cy-x, colour);
        render_horizontal_line(cx-y, cx+y, cy+x, colour);
    }
}

internal void render(float dt)
{
    UNUSED(dt);

    for (int y = 0; y < CIRCLES_SIZE; ++y) {
        for (int x = 0; x < CIRCLES_SIZE; ++x) {
            int rx = (int) lerpf(5.0f, 15.0f, (float) x/CIRCLES_SIZE);
            int ry = (int) lerpf(5.0f, 15.0f, (float) y/CIRCLES_SIZE);
            int cx = (WIDTH/2 - CIRCLES_WIDTH/2) + x*CIRCLES_PAD;
            int cy = (HEIGHT/2 - CIRCLES_WIDTH/2) + y*CIRCLES_PAD;

            unsigned int r = (unsigned int) (255.0f * x/CIRCLES_SIZE);
            unsigned int g = (unsigned int) (255.0f * y/CIRCLES_SIZE);
            uint32_t colour = ((r << 3*8) | (g << 2*8) | (0x00 << 1*8) | 0xFF);
            
            render_fill_circle(cx, cy, rx + ry, colour);
        }
    }
}
