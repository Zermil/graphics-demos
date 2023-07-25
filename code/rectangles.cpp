#include "main.cpp"

global float g_angle = 0.0f;

struct Vec2 {
    int x;
    int y;
};

internal inline int vec2_dot(Vec2 *a, Vec2 *b)
{
    return(a->x*b->x + a->y*b->y);
}

internal inline void vec2_rotate_around(Vec2 *a, Vec2 *center)
{
    const float sine = sinf(g_angle);
    const float cosine = cosf(g_angle);
    Vec2 rot = { a->x - center->x, a->y - center->y };
    
    a->x = (int) (center->x + rot.x*cosine - rot.y*sine);
    a->y = (int) (center->y + rot.x*sine + rot.y*cosine);
}

// @Note: 'Technically' :nerd_emoji: this is not a _correct_
// colour interpolation, because internally colours are stored
// as square roots, so we should 'square -> interpolate -> square root'
// but this is good enough for demo.
internal uint32_t interpolate_colour(uint32_t a, float t)
{
    unsigned int r = (unsigned int) (((a >> 3*8) & 0xFF) * t);
    unsigned int g = (unsigned int) (((a >> 2*8) & 0xFF) * t);
    unsigned int b = (unsigned int) (((a >> 1*8) & 0xFF) * t);

    return((r << 3*8) | (g << 2*8) | (b << 1*8) | 0xFF);
}

// @Note: Drawing a rectangle that's Axis-Aligned, no special checks
// just simple loops.
internal void render_simple_rectangle(int x0, int y0, int w, int h, uint32_t colour)
{
    for (int y = y0; y <= y0 + h; ++y) {
        for (int x = x0; x <= x0 + w; ++x) {
            // @Note: Boundary check.
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;

            ARRAY_AT(frame_buffer, x, y) = colour;
        }
    }
}

// @Note: We are assuming that provided points are clockwise.
// We could realistically sort them clockwise and ensue that we always
// render things correctly, but it's too much for a simple demo.
internal void render_rectangle_quad(int x0, int y0,
                                    int x1, int y1,
                                    int x2, int y2,
                                    int x3, int y3,
                                    uint32_t colour)
{
    // @Note: Finding the centroid of a quad, this will help
    // us determine whether or not it actually is a rectangle.
    Vec2 centroid = { (x0 + x1 + x2 + x3)/4, (y0 + y1 + y2 + y3)/4 };

    // @Note: If we wanted to we can sort points clockwise somewhere here
    // and check that provided points actually form a rectangle. (ie. sorting by
    // their angle from the center)
    //
    // ...
    //
    
    // @Note: Minimum bounding rectangle.
    int x_max = MAX(MAX(x0, x1), MAX(x2, x3));
    int x_min = MIN(MIN(x0, x1), MIN(x2, x3));
    int y_max = MAX(MAX(y0, y1), MAX(y2, y3));
    int y_min = MIN(MIN(y0, y1), MIN(y2, y3));

    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            // Note: Boundary check.
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;

            // @Note: Check if point is in rectangle.
            Vec2 AP = { x - x0, y - y0 };
            Vec2 AB = { x1 - x0, y1 - y0 };
            Vec2 BC = { x2 - x1, y2 - y1 };

            int dot_ap = vec2_dot(&AB, &AP);
            int dot_ab = vec2_dot(&AB, &AB);
            int dot_bp = vec2_dot(&BC, &AP);
            int dot_bc = vec2_dot(&BC, &BC);
            
            if ((dot_ap >= 0.0f && dot_ap <= dot_ab) &&
                (dot_bp >= 0.0f && dot_bp <= dot_bc))
            {
                ARRAY_AT(frame_buffer, x, y) = colour;
            }
        }
    }
}

internal void render(float dt)
{
    g_angle += TURNS(0.35f)*dt;
    
    // @Note: Simple rectangle.
    {
        uint32_t colours[] = { 0x000000FF, 0xA3A3A3FF, 0xFFFFFFFF, 0x800080FF};
        int w = 200;
        int h = 30;
        int x0 = WIDTH/2 - w/2;
        int y0 = 10;

        float t = (sinf(g_angle) + 1)/4 + 0.5f;
    
        for (int i = 0; i < ARRAY_SIZE(colours); ++i) {
            render_simple_rectangle(x0, y0 + i*h, w, h, interpolate_colour(colours[i], t));
        }
    }

    // @Note: Rotating rectangle.
    {
        int w = 100;
        int h = 150;
        
        Vec2 p0 = { WIDTH/2 - w/2, HEIGHT/2 };
        Vec2 p1 = { WIDTH/2 + w/2, HEIGHT/2 };
        Vec2 p2 = { WIDTH/2 + w/2, HEIGHT/2 + h };
        Vec2 p3 = { WIDTH/2 - w/2, HEIGHT/2 + h };
        Vec2 centroid = { (p0.x + p1.x + p2.x + p3.x)/4, (p0.y + p1.y + p2.y + p3.y)/4 };
        
        vec2_rotate_around(&p0, &centroid);
        vec2_rotate_around(&p1, &centroid);
        vec2_rotate_around(&p2, &centroid);
        vec2_rotate_around(&p3, &centroid);
        
        render_rectangle_quad(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0x5BCEFAFF);
    }
}
