#include "../main.cpp"

global float g_angle = 0.0f;

struct Vec2 {
    int x;
    int y;
};

struct Vec2f {
    float x;
    float y;
};

internal inline int vec2_dot(Vec2 *a, Vec2 *b)
{
    return(a->x*b->x + a->y*b->y);
}

internal inline float vec2f_length(Vec2f vec)
{    
    return(sqrtf(vec.x*vec.x + vec.y*vec.y));
}

// @Note: This is just the basic formula for point/vector rotation.
internal void vec2_rotate_many_points_around(Vec2 *points, size_t length, Vec2 *centroid)
{
    ERROR_EXIT(points == NULL, "[ERROR]: Array of points was NULL\n");
    
    const float sine = sinf(g_angle);
    const float cosine = cosf(g_angle);
    
    for (size_t i = 0; i < length; ++i) {
        Vec2 rot = { points[i].x - centroid->x, points[i].y - centroid->y };
        points[i].x = (int) (centroid->x + rot.x*cosine - rot.y*sine);
        points[i].y = (int) (centroid->y + rot.x*sine + rot.y*cosine);
    }
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

// @Note: Using SDFs (Signed distance fields) to render a rounded rectangle, typically
// something like this would be done on a shader, but this is for demo purposes meant to be
// quickly understood by most.
internal void render_rounded_rectangle(int x0, int y0, int w, int h, int r, uint32_t colour)
{
    for (int y = y0; y <= y0 + h; ++y) {
        for (int x = x0; x <= x0 + w; ++x) {
            // @Note: Boundary check.
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;
            
            Vec2 p = { x - (x0 + w/2), y - (y0 + h/2) };
            
            // @Note: AFAIK - We're subtracting w/2 and h/2 because (look above) we
            // added w/2 and h/2 and we just need to check the corner here.
            Vec2 q = { ABS(p.x) - w/2 + r, ABS(p.y) - h/2 + r };
            
            float outside = vec2f_length({ MAX(q.x, 0.0f), MAX(q.y, 0.0f) });
            float inside = MIN(MAX(q.x, q.y), 0.0f) - r;            
            if (outside + inside <= 0.0f) ARRAY_AT(frame_buffer, x, y) = colour;
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

    Vec2 AB = { x1 - x0, y1 - y0 };
    int dot_ab = vec2_dot(&AB, &AB);
    
    Vec2 BC = { x2 - x1, y2 - y1 };
    int dot_bc = vec2_dot(&BC, &BC);
    
    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            // Note: Boundary check.
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;

            // @Note: Check if point is in rectangle.
            Vec2 AP = { x - x0, y - y0 };

            int dot_ap = vec2_dot(&AB, &AP);
            int dot_bp = vec2_dot(&BC, &AP);
            
            if ((dot_ap >= 0 && dot_ap <= dot_ab) &&
                (dot_bp >= 0 && dot_bp <= dot_bc))
            {
                ARRAY_AT(frame_buffer, x, y) = colour;
            }
        }
    }
}

internal void render(float dt)
{
    g_angle += TURNS(0.30f)*dt;
    
    // @Note: Simple rectangle.
    {
        uint32_t colours[] = { 0x000000FF, 0xA3A3A3FF, 0xFFFFFFFF, 0x800080FF};
        int w = 200;
        int h = 30;
        
        Vec2 p0 = {
            WIDTH/2 - w/2,
            10
        };

        float t = (sinf(g_angle) + 1)/4 + 0.5f;
    
        for (int i = 0; i < ARRAY_SIZE(colours); ++i) {
            render_simple_rectangle(p0.x, p0.y + i*h, w, h, interpolate_colour(colours[i], t));
        }
    }

    // @Note: Rotating rectangle.
    {
        int w = 100;
        int h = 150;
        
        Vec2 p[] = {
            { WIDTH/2 - w/2, HEIGHT/2 + 100},
            { WIDTH/2 + w/2, HEIGHT/2 + 100},
            { WIDTH/2 + w/2, HEIGHT/2 + 100 + h },
            { WIDTH/2 - w/2, HEIGHT/2 + 100 + h },
        };
        
        Vec2 centroid = {
            (p[0].x + p[1].x + p[2].x + p[3].x)/4,
            (p[0].y + p[1].y + p[2].y + p[3].y)/4
        };
        
        vec2_rotate_many_points_around(p, ARRAY_SIZE(p), &centroid);
        render_rectangle_quad(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y, 0x5BCEFAFF);
    }

    // @Note: Rounded rectangle.
    {
        int w = 200;
        int h = 120;
        int r = 15;
        
        render_rounded_rectangle(WIDTH/2 - w/2, HEIGHT/2 - h/2 - r, w, h, r, 0xEF7627FF);
    }
}
