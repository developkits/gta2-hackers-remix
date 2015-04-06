#include "sty/sty.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Syntax: %s font_id text_string\n", argv[0]);
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    exit(printf("SDL_ERROR: %s\n", SDL_GetError()));

  SDL_Window *window =
      SDL_CreateWindow("G2HR - sprite test", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0xff, 0, 0); // green
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  sty_t *sty = sty_load("data/fstyle.sty");

  int font_id = -1;
  int font_count = sty->font_base.font_count;
  while (1) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT)
      break;
    if (event.type == SDL_KEYDOWN || font_id == -1) {
      font_id++;
      if (font_id >= sty->font_base.font_count)
        font_id = 0;
      printf("font_id %i/%i, offset: %i\n", font_id, font_count - 1,
             sty->font_base.base[font_id]);
    }
    SDL_RenderClear(renderer);
    sty_text(renderer, sty, atoi(argv[1]), argv[2]);

    /*
    // print 200 characters, starting at the font base
    // fixme: this likes to segfault when it goes out of bounds,
    // so currently we can't print the last font!
    int offset_x = 0;
    int offset_y = 0;
    int offset_n = 0;//42;

    for(int y=0;y<10;y++)
    {
            for(int x=0;x<20;x++)
            {
                    char letter = x + 20*y + offset_n;
                    int width, height;
                    int sprite_id = letter + sty->sprite_base.font
                            + sty->font_base.base[font_id];

                    SDL_Texture* sprite = sty_sprite(renderer, sty, 1,
    sprite_id);
                    if(sprite)
                    {
                            SDL_QueryTexture(sprite, NULL, NULL, &width,
    &height);
                            SDL_Rect dest = { offset_x, offset_y, width,
    height};
                            SDL_RenderCopy(renderer, sprite, NULL, &dest);
                            SDL_DestroyTexture(sprite);
                    }

                    offset_x+=32;
            }
            offset_x = 0;
            offset_y += 480/10;
    }

    */

    SDL_RenderPresent(renderer);
  }
}
