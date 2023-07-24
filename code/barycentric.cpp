#include "main.cpp"

global float g_angle = 0.0f;

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

// @Note: Yes, this could be a bunch of 'struct Point {};' or similar
// however I wanted to make something more generic and not force a type
// for the sake of demonstration.
internal void rotate_points(int *x0, int *y0,
                            int *x1, int *y1,
                            int *x2, int *y2,
                            float angle)
{
    const int cx = (*x0 + *x1 + *x2)/3;
    const int cy = (*y0 + *y1 + *y2)/3;
    const float sine = sinf(angle);
    const float cosine = cosf(angle);

    int dx0 = *x0 - cx;
    int dy0 = *y0 - cy;
    int dx1 = *x1 - cx;
    int dy1 = *y1 - cy;
    int dx2 = *x2 - cx;
    int dy2 = *y2 - cy;
    
    *x0 = (int) (cx + dx0*cosine - dy0*sine);
    *y0 = (int) (cy + dx0*sine + dy0*cosine);

    *x1 = (int) (cx + dx1*cosine - dy1*sine);
    *y1 = (int) (cy + dx1*sine + dy1*cosine);
    
    *x2 = (int) (cx + dx2*cosine - dy2*sine);
    *y2 = (int) (cy + dx2*sine + dy2*cosine);
}

internal void render(float dt)
{    
    int x0 = WIDTH/2;
    int y0 = HEIGHT/4;
    int x1 = WIDTH/3;
    int y1 = HEIGHT - HEIGHT/3;
    int x2 = WIDTH - WIDTH/3;
    int y2 = HEIGHT - HEIGHT/3;
  
    rotate_points(&x0, &y0, &x1, &y1, &x2, &y2, TURNS(g_angle));
    render_triangle(x0, y0, x1, y1, x2, y2);

    g_angle += TURNS(0.05f)*dt;
}
