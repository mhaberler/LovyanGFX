#if 1
#include <lgfx/v1/platforms/sdl/Panel_sdl.hpp>
#if defined(SDL_h_)

void setup(void);
void loop(void);

__attribute__((weak)) int user_func(bool *running)
{
  printf("before setup\n");

  setup();
  do
  {
    printf("before loop\n");

    loop();
  } while (*running);
  return 0;
}

int main(int, char **)
{
  printf("main...\n");

  // return
//  lgfx::Panel_sdl::main(user_func);
  printf("exitus...\n");
}

#endif
#endif

#if 0

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>
#include <emscripten.h>

void setup(void);
void loop(void);

void loopThread(void *arg)
{
  loop();
  // lgfx::Panel_sdl::sdl_event_handler();
}

int monitor_hor_res, monitor_ver_res;

int main(int argc, char **argv)
{
        printf("before setup...\n");

  setup();
          printf("after setup...\n");

  emscripten_set_main_loop_arg(loopThread, NULL, -1, true);
      printf("quitting...\n");
}
#endif

#if 0
#include <stdio.h>

/**
 * Provides emscripten_set_main_loop_arg and emscripten_cancel_main_loop
 */
#include <emscripten.h>

/**
 * A context structure that we can use for passing variables to our loop
 * function, in this case it just contains a single integer
 */
struct context
{
    int x;
};

/**
 * The loop handler, will be called repeatedly
 */
void loop_fn(void *arg)
{
    struct context *ctx = (struct context *)arg;

    printf("x: %d\n", ctx->x);

    if (ctx->x >= 100)
    {
        /**
         * After 101 iterations, stop looping
         */
        emscripten_cancel_main_loop();
        printf("got here...\n");
        return;
    }

    ctx->x += 1;
}

int main()
{
    struct context ctx;
    int simulate_infinite_loop = 1;

    ctx.x = 0;

    emscripten_set_main_loop_arg(loop_fn, &ctx, -1, simulate_infinite_loop);

    /**
     * If simulate_infinite_loop = 0, emscripten_set_main_loop_arg won't block
     * and this code will run straight away.
     * If simulate_infinite_loop = 1 then this code will not be reached
     */
    printf("quitting...\n");
}

#endif
