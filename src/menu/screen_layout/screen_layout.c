#include "screen_layout.h"
#include <stdio.h>
#include <stdlib.h>

/*
        sl_parse() is a port from AutoIt3 to C99 of the function
        screen_layout_init() of screen_layout_init.au3, which was written
        for the alpha version of the splitscreen mod.
*/

#define ERR(MSG)                                                               \
  exit(printf("ERROR (line %i, col %i, char '%c'): %s!\n", line_number,        \
              char_in_line, buffer[i], MSG))
sl_t *sl_parse(char *buffer, size_t buffer_size) {
  sl_t *sl = malloc(sizeof(sl_t));
  sl->players = calloc(1, sizeof(sl_entry_t *) * GTA2_MAX_PLAYERS);

  sl_geo_t **block_geometry = NULL;
  char is_comment_line = 0;
  int char_in_line = 0;
  int line_number = 1;
  int layout_width = 0;
  int layout_height = 0;
  int last_left_border_pos_in_line = -1; // -1: not found in line yet
  char players_found = 0;

  for (size_t i = 0; i < buffer_size; i++) {
    char c = buffer[i];

    if (block_geometry && !layout_width && c != '\n' && c != '\r' && c != '-')
      ERR("The first line of a new layout block must only consist of dashes "
          "(-)");

    // beginning of a new line
    if (char_in_line == 0) {
      // layout block start
      if (c == '-' && !block_geometry) {
        printf("new block: ");
        block_geometry = calloc(1, sizeof(sl_geo_t *) * GTA2_MAX_PLAYERS);
        layout_width = 0;
        layout_height = 0;
        players_found = 0;
        is_comment_line = 0;
      }
      // layout block end ('\n\n')
      else if (c == '\n' && block_geometry) {
        block_geometry->w = layout_width;
        block_geometry->h = layout_height;

        // TODO: attach block to sl!
        block_geometry = NULL;
        last_left_border_pos_in_line = -1;
        printf("height: %2i (end)\n", layout_height);
      } else if (c != '-' && c != '|') {
        if (block_geometry)
          ERR("Found a comment line inside a layout block. This isn't allowed. "
              "Make sure that you have an empty line after each layout block");
        is_comment_line = 1;
      }
    }

    if (!is_comment_line) {
      if (c == '|')
        last_left_border_pos_in_line = char_in_line;

      // We found a number! let's measure the geometry by going
      // left, right, up and down until we hit all the borders.
      else if (c >= '1' && c <= '0' + GTA2_MAX_PLAYERS) {
        if (block_geometry[c - '1'])
          ERR("You must not use the same player number in one screen layout "
              "more than once");

        if (last_left_border_pos_in_line == -1)
          ERR("No border ('|') to the left of this number");

        sl_geo_t *geo = calloc(1, sizeof(sl_geo_t));

        // left
        geo->x = last_left_border_pos_in_line;

        // right
        for (int j = 0; i + j <= buffer_size; j++) {
          if (i + j == buffer_size || buffer[i + j] == '\n' ||
              buffer[i + j] == '\r')
            ERR("No border ('|') to the right of this number");
          if (buffer[i + j] != '|')
            continue;
          geo->w = j;
          break;
        }

        // up
        for (int j = 0;; j++) {
          if (buffer[i - j * layout_width] != '-')
            continue;
          geo->y = layout_height - j;
          break;
        }

        // down
        // FIXME: check at \n if bottom line was -----
        for (int j = 0; i + j < buffer_size; j += layout_width)
          if (buffer[i + j] == '-') {
            geo->h = -1 * (geo->y - layout_height) + j / layout_width;
            break;
          }
        if (!geo->h)
          ERR("No border ('-') at the bottom of this number");

        // debug:
        printf("%i: [%3u,%3u,%3u,%3u], ", c - '0', geo->x, geo->y, geo->w,
               geo->h);

        block_geometry[c - '1'] = geo;
      }

      else if (c > '0' + GTA2_MAX_PLAYERS)
        ERR("Invalid number (1...6 allowed)");
      else if (c != ' ' && c != '-' && c != '\n' && c != '\r')
        ERR("Invalid symbol");
    }

    if (c == '\n') {
      if (block_geometry) {
        if (!layout_width) {
          layout_width = char_in_line;
          printf("width: %2i, ", layout_width);
        } else if (layout_width != char_in_line)
          ERR("All lines in a layout block must have the same width!");

        layout_height++;
      }
      char_in_line = 0;
      line_number++;
    } else
      char_in_line++;
  }

  return sl;
}
#undef ERR

sl_t *sl_init(char *filename) {
  printf("parsing %s...\n", filename);
  FILE *handle = fopen(filename, "rb");
  if (!handle)
    exit(printf("ERROR: Couldn't read '%s'!\n", filename));

  // calculate file size
  int size;
  fseek(handle, 0, SEEK_END);
  size = ftell(handle);

  // read the whole file into RAM (<<10 MB) and close it
  char *buffer;
  rewind(handle);
  buffer = (char *)malloc(size);
  if (fread(buffer, 1, size, handle) != size)
    exit(printf("Read error while reading '%s'!\n", filename));
  fclose(handle);

  sl_t *sl = sl_parse(buffer, size);
  free(buffer);
  return sl;
}

void sl_cleanup(sl_t *sl) {
  for (int i = 0; i < GTA2_MAX_PLAYERS; i++)
    if (sl->players[i]) {
      // ...
      free(sl->players[i]);
    }
  free(sl);
}
