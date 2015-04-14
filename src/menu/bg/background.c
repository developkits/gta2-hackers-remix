#include "background.h"
#include <SDL2/SDL_image.h>

bg_t *bg_load_single(const char *name) {
  bg_t *bg = malloc(sizeof(bg));
  bg->name = name;
  bg->next = NULL;

  char fullpath[100];
  snprintf(fullpath, sizeof(fullpath), "data/frontend/%s.tga", name);
  printf("loading %s...\n", fullpath);

  SDL_Surface *surface = IMG_Load(fullpath);
  if (!surface)
    exit(printf("File read error!\n"));

  // Workaround for upstream bug:
  //		https://bugzilla.libsdl.org/show_bug.cgi?id=2840
  char *pixels = surface->pixels;
  for (int y = 0; y < surface->h; y++)
    for (int x = 0; x < surface->w; x++) {
      int left_byte_addr = y * surface->w * 2 + x * 2;
      char left_old = pixels[left_byte_addr];

      pixels[left_byte_addr + 0] = pixels[left_byte_addr + 1];
      pixels[left_byte_addr + 1] = left_old;
    }

  bg->surface = surface;
  return bg;
}

bg_t *bg_init(const char **tgas) {
  bg_t *first = NULL;
  bg_t *last = NULL;

  for (int i = 0; i < sizeof(tgas) / sizeof(char *); i++) {
    bg_t *new = bg_load_single(tgas[i]);
    if (!first)
      first = new;
    else
      last->next = new;
    last = new;
  }
  return first;
}

bg_t *bg_cached(bg_t *bg, const char *name) {
  while (bg->name != name) {
    bg = bg->next;
    if (bg)
      continue;

    printf("ERROR: background %s not cached!\n", name);
    return NULL;
  }
  return bg;
}

bg_mashup_t *bg_mashup(bg_t *bg, const char *full, const char *left,
                       const char *right, const char *custom) {
  bg_mashup_t *ret = malloc(sizeof(bg_mashup_t));
  ret->full = bg_cached(bg, full);
  ret->left = bg_cached(bg, left);
  ret->right = bg_cached(bg, right);
  ret->custom = bg_cached(bg, custom);
  return ret;
}

void bg_cleanup(bg_t *bg) {
  bg_t *next = bg;
  while (next) {
    next = bg->next;
    SDL_FreeSurface(bg->surface);
    free(bg);
  }
}
