#include "main.cpp"

global float g_angle = 0.0f;

struct Vec2 {
    int x;
    int y;
};

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

// @Note: This is nothing special, we're just copy-pasting our formula
// that we previously derived.
internal bool get_barycentric_coordinates(int x0, int y0,
                                          int x1, int y1,
                                          int x2, int y2,
                                          int px, int py,
                                          float *u0, float *u1, float *u2)
{
    float det = (float) ((x0 - x2)*(y1 - y2) - (x1 - x2)*(y0 - y2));
    if (det == 0.0f) {
        return(false);
    }
    
    *u0 = ((px - x2)*(y1 - y2) + (py - y2)*(x2 - x1)) / det;
    *u1 = ((px - x2)*(y2 - y0) + (py - y2)*(x0 - x2)) / det;
    *u2 = 1.0f - (*u0) - (*u1);

    return(true);
}

// @Note: There are more computationally efficient algorithms
// for filling up a triangle, but this demo serves to showcase
// barycentric coordinates, so we will use them.
internal void render_triangle(int x0, int y0,
                              int x1, int y1,
                              int x2, int y2)
{
    // @Note: Bounding box for our triangle.
    int x_max = MAX(MAX(x0, x1), x2);
    int x_min = MIN(MIN(x0, x1), x2);
    int y_max = MAX(MAX(y0, y1), y2);
    int y_min = MIN(MIN(y0, y1), y2);
    
    float u0, u1, u2;
    
    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            // @Note: Boundary check.
            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;
            
            if (!get_barycentric_coordinates(x0, y0, x1, y1, x2, y2, x, y, &u0, &u1, &u2)) continue;
            
            // @Note: This expression can be simplified in various ways.
            if ((u0 >= 0.0f && u0 <= 1.0f) &&
                (u1 >= 0.0f && u1 <= 1.0f) &&
                (u2 >= 0.0f && u2 <= 1.0f))
            {
                // @Note: Point in triangle.
                unsigned int r = (unsigned int) (255.0f * u0);
                unsigned int g = (unsigned int) (255.0f * u1);
                unsigned int b = (unsigned int) (255.0f * u2);

                uint32_t out_color = (r << 3*8) | (g << 2*8) | (b << 1*8) | 0xFF;
                ARRAY_AT(frame_buffer, x, y) = out_color;
            }
        }
    }
}

internal void render(float dt)
{
    g_angle += TURNS(0.20f)*dt;

    Vec2 p[] = {
        { WIDTH/2, HEIGHT/4 },
        { WIDTH/3, HEIGHT - HEIGHT/3 },
        { WIDTH - WIDTH/3, HEIGHT - HEIGHT/3 },
    };
    
    Vec2 centroid = {
        (p[0].x + p[1].x + p[2].x)/3,
        (p[0].y + p[1].y + p[2].y)/3
    };
    
    vec2_rotate_many_points_around(p, ARRAY_SIZE(p), &centroid);
    render_triangle(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);
}
