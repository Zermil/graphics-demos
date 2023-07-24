#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

#define MATH_PI  3.14159265358979323846f
#define MATH_TAU 6.28318530717958647692f

#define TURNS(x) (MATH_TAU*(x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define UNUSED(x) ((void)(x))
#define ERROR_EXIT(cond, msg, ...)                  \
    do {                                            \
        if ((cond)) {                               \
            fprintf(stderr, (msg), __VA_ARGS__);    \
            exit(1);                                \
        }                                           \
    } while(0)

#define internal static
#define global static

#define FPS 30
#define MS_PER_FRAME (1000/FPS)
#define WIDTH 800
#define HEIGHT 600

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define ARRAY_AT(arr, x, y) ((arr)[(y)*WIDTH + (x)])
global uint32_t frame_buffer[WIDTH*HEIGHT] = {0};

struct Render_Context {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *buffer_texture;
};

internal void render(float dt);

internal Render_Context create_render_context(unsigned int width, unsigned int height, const char *title)
{
    Render_Context render_context = {0};

    ERROR_EXIT(SDL_Init(SDL_INIT_VIDEO) != 0, "[ERROR]: Could not initialize SDL2\n");

    render_context.window = SDL_CreateWindow(title,
                                             SDL_WINDOWPOS_CENTERED,
                                             SDL_WINDOWPOS_CENTERED,
                                             width,
                                             height,
                                             SDL_WINDOW_SHOWN);
    ERROR_EXIT(render_context.window == NULL, "[ERROR]: Could not create SDL window\n");

    render_context.renderer = SDL_CreateRenderer(render_context.window, -1, SDL_RENDERER_ACCELERATED);
    ERROR_EXIT(render_context.renderer == NULL, "[ERROR]: Could not create SDL renderer\n");

    render_context.buffer_texture = SDL_CreateTexture(render_context.renderer,
                                                      SDL_PIXELFORMAT_RGBA8888,
                                                      SDL_TEXTUREACCESS_STREAMING,
                                                      WIDTH, HEIGHT);
    ERROR_EXIT(render_context.buffer_texture == NULL, "[ERROR]: Could not create SDL texture\n");

    return(render_context);
}

internal void destroy_render_context(Render_Context *ctx)
{
    SDL_DestroyWindow(ctx->window);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyTexture(ctx->buffer_texture);

    SDL_Quit();
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Render_Context ctx = create_render_context(WIDTH, HEIGHT, "A window");
    bool should_quit = false;
    
    int current_time = 0;
    int previous_time = SDL_GetTicks();

    while (should_quit != true) {
        current_time = SDL_GetTicks();
        int time_elapsed = current_time - previous_time;
        int time_to_wait = MS_PER_FRAME - time_elapsed;
        float dt = time_elapsed / 1000.0f;
        previous_time = current_time;
        
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    should_quit = true;
                } break;
            }
        }

        for (size_t i = 0; i < WIDTH*HEIGHT; ++i) {
            frame_buffer[i] = 0x181818FF;
        }
        
        render(dt);

        SDL_UpdateTexture(ctx.buffer_texture, NULL, frame_buffer, sizeof(uint32_t)*WIDTH);
        SDL_RenderCopy(ctx.renderer, ctx.buffer_texture, NULL, NULL);
        SDL_RenderPresent(ctx.renderer);

        if (time_to_wait > 0 && time_to_wait < MS_PER_FRAME) {
            SDL_Delay(time_to_wait);
        }
    }

    destroy_render_context(&ctx);
    
    return 0;
}
