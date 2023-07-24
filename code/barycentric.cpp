#include "main.cpp"

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
            if (!get_barycentric_coordinates(x0, y0, x1, y1, x2, y2, x, y, &u0, &u1, &u2)) continue;

            // @Note: This expression can be simplified in various ways.
            if ((u0 >= 0.0f && u0 <= 1.0f) &&
                (u1 >= 0.0f && u1 <= 1.0f) &&
                (u2 >= 0.0f && u2 <= 1.0f))
            {
                // @Note: Point in triangle.
                int r = (int) (255.0f * u0);
                int g = (int) (255.0f * u1);
                int b = (int) (255.0f * u2);

                uint32_t out_color = (r << 3*8) | (g << 2*8) | (b << 1*8) | 0xFF;
                ARRAY_AT(frame_buffer, x, y) = out_color;
            }
        }
    }
}

internal void render(float dt)
{
    UNUSED(dt);

    int x0 = WIDTH/2;
    int y0 = 0;

    int x1 = 0;
    int y1 = HEIGHT;

    int x2 = WIDTH;
    int y2 = HEIGHT;
    
    render_triangle(x0, y0, x1, y1, x2, y2);
}
