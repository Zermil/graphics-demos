#include "../main.cpp"

#define WAIT_FOR 6.0f
#define SEEDS_SZ 8
#define PALETTE_SZ 8

struct Vec2 {
    int x;
    int y;
};

global float timer = 6.0f;
// @Note: Colour palette -- https://lospec.com/palette-list/slso8
global uint32_t palette[PALETTE_SZ] = {
    0x0d2b45FF,
    0x203c56FF,
    0x544e68FF,
    0x8d697aFF,
    0xd08159FF,
    0xffaa5eFF,
    0xffd4a3FF,
    0xffecd6FF,
};
global Vec2 seeds[SEEDS_SZ] = {0};

// @Note: Returns a random float between 0 and 1.
internal inline float random_float()
{
    return ((float) rand() / (float) RAND_MAX);
}

// @Note: Standard calculations for distance.
internal inline int points_get_sqrdistance(int x0, int y0, int x1, int y1)
{
    return((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
}

internal inline int points_get_mandistance(int x0, int y0, int x1, int y1)
{    
    return(ABS(x1 - x0) + ABS(y1 - y0));
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
    if (timer >= WAIT_FOR) {
        for (int i = 0; i < SEEDS_SZ; ++i) {
            seeds[i] = {
                (int) (random_float() * WIDTH),
                (int) (random_float() * HEIGHT)
            };
        }
        
        timer = 0.0f;
    }
    
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int j = 0;
            int dst = points_get_mandistance(x, y, seeds[0].x, seeds[0].y);
            
            for (int i = 1; i < SEEDS_SZ; ++i) {
                int new_dst = points_get_mandistance(x, y, seeds[i].x, seeds[i].y);
                if (new_dst < dst) {
                    j = i;
                    dst = new_dst;
                }
            }

            ARRAY_AT(frame_buffer, x, y) = palette[j % PALETTE_SZ];
        }
    }

    for (int i = 0; i < SEEDS_SZ; ++i) {
        render_fill_circle(seeds[i].x, seeds[i].y, 3, 0x000000FF);
    }

    timer += dt;
}
