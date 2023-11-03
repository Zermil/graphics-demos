#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <SDL2/SDL.h>

#include "linalg.h"

#define TURNS(x) ((x) * 6.2831855f)
#define TURN_SPEED TURNS(0.50f)
#define MOVE_SPEED 1.2f

#define UNUSED(x) ((void)(x))
#define internal static
#define global static

#define FPS 60
#define MS_PER_FRAME (1000/FPS)

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

#define MAP_W 24
#define MAP_H 24

global unsigned int level[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

struct Render_Context {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

internal Render_Context render_create_context(const char *window_name, unsigned int w, unsigned int h)
{
    Render_Context ctx = {0};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[ERROR] :: Could not initialize SDL2 -> %s\n", SDL_GetError());
        exit(1);
    }

    ctx.window = SDL_CreateWindow(window_name,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  w, h,
                                  SDL_WINDOW_SHOWN);
    if (ctx.window == 0) {
        fprintf(stderr, "[ERROR] :: Not enough memory to initialize SDL window\n");
        exit(1);
    }

    ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED);
    if (ctx.renderer == 0) {
        fprintf(stderr, "[ERROR] :: Not enough memory to initialize SDL renderer\n");
        exit(1);
    }
    
    return(ctx);
}

internal void render_destroy_context(Render_Context *ctx)
{
    SDL_DestroyWindow(ctx->window);
    SDL_DestroyRenderer(ctx->renderer);
    
    SDL_Quit();
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    Vec2f player = { 22.0f, 12.0f };
    Vec2f dir = { -1.0f, 0.0f };
    Vec2f plane = { 0.0f, 0.66f };
    
    // @Note: Rendering part
    Render_Context ctx = render_create_context("A window", WIN_WIDTH, WIN_HEIGHT);
    bool should_quit = false;
    
    unsigned int current_time = 0;
    unsigned int previous_time = SDL_GetTicks();
    
    while (!should_quit) {
        current_time = SDL_GetTicks();
        unsigned int elapsed_time = current_time - previous_time;
        previous_time = current_time;
        
        float dt = elapsed_time / 1000.0f;
        
        SDL_Event e = {0};
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    should_quit = true;
                } break;

                case SDL_KEYDOWN: {
                    switch (e.key.keysym.sym) {
                        case SDLK_q: {
                            plane = vec2f_rot(plane, TURN_SPEED * dt);
                            dir = vec2f_rot(dir, TURN_SPEED * dt);
                        } break;
                                
                        case SDLK_e: {                            
                            plane = vec2f_rot(plane, -TURN_SPEED * dt);
                            dir = vec2f_rot(dir, -TURN_SPEED * dt);
                        } break;

                        // @ToDo: Boundary check
                        case SDLK_w: {
                            player.y += dir.y * MOVE_SPEED;
                            player.x += dir.x * MOVE_SPEED;
                        } break;
                            
                        case SDLK_s: {
                            player.y -= dir.y * MOVE_SPEED;
                            player.x -= dir.x * MOVE_SPEED;
                        } break;
                    }
                } break;
            }
        }
        
        SDL_SetRenderDrawColor(ctx.renderer, 18, 18, 18, 255);
        SDL_RenderClear(ctx.renderer);

        for (unsigned int x = 0; x < WIN_WIDTH; ++x) {
            float nx = (((float) x / WIN_WIDTH) * 2) - 1; // @Note: Normalized to -1..1
            Vec2f ray_dir = vec2f_add(dir, vec2f_mult(plane, nx));
            Vec2f ray_step = {
                sqrtf(1.0f + (ray_dir.y * ray_dir.y) / (ray_dir.x * ray_dir.x)),
                sqrtf(1.0f + (ray_dir.x * ray_dir.x) / (ray_dir.y * ray_dir.y))
            };

            Vec2f step = {0};
            Vec2f length = {0};
            Vec2f start = { floorf(player.x), floorf(player.y) };
            
            if (ray_dir.x < 0) {
                // @Note: Moving left
                step.x = -1.0f;
                length.x = (start.x - player.x) * ray_step.x;
            } else {
                // @Note: Moving right
                step.x = 1.0f;
                length.x = ((start.x + 1.0f) - player.x) * ray_step.x;
            }

            if (ray_dir.y < 0) {
                // @Note: Moving up
                step.y = -1.0f;
                length.y = (start.y - player.y) * ray_step.y;
            } else {
                // @Note: Moving down
                step.y = 1.0f;
                length.y = ((start.y + 1.0f) - player.y) * ray_step.y;
            }

            // @Note: Looking for intersection
            bool side_hit = false;
            float distance = 0.0f;
            while (level[(int) start.x][(int) start.y] == 0) {
                if (length.x < length.y) {
                    start.x += step.x;
                    distance = length.x;
                    
                    length.x += ray_step.x;
                    side_hit = false;
                } else {
                    start.y += step.y;
                    distance = length.y;
                                        
                    length.y += ray_step.y;
                    side_hit = true;
                }
            }
            
            unsigned char colour_val = 255 - 128 * side_hit;
            switch (level[(int) start.x][(int) start.y]) {
                case 1: SDL_SetRenderDrawColor(ctx.renderer, colour_val, 0, 0, 255); break;
                case 2: SDL_SetRenderDrawColor(ctx.renderer, 0, colour_val, 0, 255); break;
                case 3: SDL_SetRenderDrawColor(ctx.renderer, 0, 0, colour_val, 255); break;
                case 4: SDL_SetRenderDrawColor(ctx.renderer, colour_val, colour_val, colour_val, 255); break;

                default: SDL_SetRenderDrawColor(ctx.renderer, colour_val, colour_val, 0, 255); break;
            }

            Vec2f intersection = vec2f_add(player, vec2f_mult(ray_dir, distance));
            Vec2f ray = vec2f_sub(intersection, player);
            
            float dir_angle = atan2f(dir.y, dir.x);
            float ray_angle = atan2f(ray.y, ray.x);

            float angle = ray_angle - dir_angle;
            float dist = WIN_HEIGHT / (vec2f_len(ray) * cosf(angle));
            
            SDL_RenderDrawLineF(ctx.renderer, (float) x, -dist + WIN_HEIGHT/2, (float) x, dist + WIN_HEIGHT/2);
        }

        SDL_RenderPresent(ctx.renderer);
        if (elapsed_time < MS_PER_FRAME) SDL_Delay(MS_PER_FRAME - elapsed_time);
    }

    render_destroy_context(&ctx);
    
    return 0;
}
