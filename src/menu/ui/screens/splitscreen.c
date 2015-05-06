#include "../../sl/sl.h"
#include "../../sty/sty.h"
#include "../../tk/ctrl/controls.h"
#include "../../tk/toolkit.h"
#include "../ui.h"
#include "../ui_text.h"

/*
        TODO:
                - store 'int controllers_found' in the client struct
                        (communication between native and menu)
                - send the abstracted splitscreen geometry values to the meta
                        component instead of the layout ID (this is for
   debugging
                        only)
*/

// TODO: place this in a common.h or something like that
#define GTA2_MP_MAX_PLAYERS 6

#define TODO_controllers_found 6

// USERDATA STRUCT
typedef struct {
  ui_t *ui;

  tk_el_t *titlebar;
  tk_el_t *players;
  tk_el_t *screen_layout;
  tk_el_t *map;
  tk_el_t *game_type;
  tk_el_t *time;
  tk_el_t *cops;
  tk_el_t *play;
} ud_splitscreen_t;

#define G2HR_SPLITSCREEN_PREVIEW_W 100
#define G2HR_SPLITSCREEN_PREVIEW_H 76
#define G2HR_SPLITSCREEN_PREVIEW_X 520
#define G2HR_SPLITSCREEN_PREVIEW_Y 60

void splitscreen_draw_layout(ud_splitscreen_t *ud) {
  sl_t *sl = ud->ui->sl;
  int players_max = ((ud_arrowtext_t *)(ud->players->userdata))->entry_selected;
  int layout_id =
      ((ud_arrowtext_t *)(ud->screen_layout->userdata))->entry_selected;
  for (int i = 0; i <= players_max; i++) {
    // draw the player's panel
    sl_geo_t geo;
    sl_calc(sl, G2HR_SPLITSCREEN_PREVIEW_W, G2HR_SPLITSCREEN_PREVIEW_H,
            players_max, layout_id, i, &geo);

    SDL_Rect r;
    r.x = geo.x + G2HR_SPLITSCREEN_PREVIEW_X;
    r.y = geo.y + G2HR_SPLITSCREEN_PREVIEW_Y;
    r.w = geo.w - 2;
    r.h = geo.h - 2;

    SDL_Renderer *renderer = ud->ui->tk->renderer;
    SDL_SetRenderDrawColor(renderer, 0xbd, 0x08, 0x19, 0xff);
    SDL_RenderFillRect(renderer, &r);

    // draw the player's number in the center of the panel
    char num_str[2];
    num_str[0] = '1' + i;
    num_str[1] = '\0';

    int num_w;
    int num_h;
    int font_id = GTA2_FONT_FSTYLE_WHITE_RED_TINY;
    sty_t *fsty = ud->ui->tk->fsty;
    sty_text_measure(fsty, &num_w, &num_h, font_id, (const char *)num_str);

    int num_x = r.x + r.w / 2 - num_w / 2;
    int num_y = r.y + r.h / 2 - num_h / 2;
    sty_text(renderer, fsty, font_id, 0xffffffff, num_x, num_y, 0, num_str);
  }
}

void splitscreen_set_max_layouts(ud_splitscreen_t *ud) {
  ud_arrowtext_t *ud_layout = ((ud_arrowtext_t *)(ud->screen_layout->userdata));
  ud_arrowtext_t *ud_players = ((ud_arrowtext_t *)(ud->players->userdata));

  int players = ud_players->entry_selected;
  int layout = ud_layout->entry_selected;
  int layout_max = ud->ui->sl->players[players]->count;
  if (!layout_max)
    layout_max = 1;

  if (layout >= layout_max)
    ud_layout->entry_selected = layout_max - 1;

  if (layout_max > 1) {
    tk_el_enabled(ud->screen_layout);
    tk_el_enabled(ud_layout->text_pre);
    tk_el_enabled(ud_layout->text);
  } else {
    tk_el_disabled(ud->screen_layout);
    tk_el_disabled(ud_layout->text_pre);
    tk_el_disabled(ud_layout->text);
  }
  ud_layout->entry_count = layout_max;
}

// ACTIONFUNC
void splitscreen_actionfunc(tk_t *tk, tk_el_t *el, tk_el_t *el_selected,
                            tk_action_t action, SDL_Keycode key) {
  ud_splitscreen_t *ud = (ud_splitscreen_t *)el->userdata;

  if (action == TK_ACTION_BEFORE_FIRST_SCREEN_FRAME) {
    ((ud_arrowtext_t *)ud->map->userdata)->entry_selected =
        ud->ui->map_selected;
    splitscreen_set_max_layouts(ud);
  }

  if (action == TK_ACTION_ENTER) {
    if (el_selected == ud->play) {
      int players =
          1 + ((ud_arrowtext_t *)(ud->players->userdata))->entry_selected;

      int screen_layout =
          ((ud_arrowtext_t *)(ud->screen_layout->userdata))->entry_selected;

      char *time = ud->ui->multiplayer_time_values
                       ->values[((ud_arrowtext_t *)(ud->time->userdata))
                                    ->entry_selected];

      char game_type =
          ((ud_arrowtext_t *)ud->game_type->userdata)->entry_selected;

      int cops_enabled = ((ud_arrowtext_t *)ud->cops->userdata)->entry_selected;

      // gta2 reads the map index from the registry
      int map_id = ((ud_arrowtext_t *)ud->map->userdata)->entry_selected;

      char *buffer = malloc(100);
      snprintf(buffer, 100, "SPLITSCREEN %i %i %i %i %s %i", players,
               screen_layout, map_id, game_type, time, cops_enabled);
      server_send(ud->ui->server, buffer, 1);
    }
    if (el_selected == ud->map) {
      ud->ui->map_selected =
          ((ud_arrowtext_t *)ud->map->userdata)->entry_selected;
      tk->screen_active = ud->ui->levels;
    }
  }
  if (action == TK_ACTION_LEFT || action == TK_ACTION_RIGHT) {
    if (el_selected == ud->players)
      splitscreen_set_max_layouts(ud);
    if (el_selected == ud->screen_layout)
      splitscreen_draw_layout(ud);
  }

  if (action == TK_ACTION_DRAW_BEFORE_CONTROLS)
    splitscreen_draw_layout(ud);
}

// SCREEN INIT & LAYOUT

tk_screen_t *ui_screen_splitscreen(tk_t *tk, ui_t *ui) {
  ud_splitscreen_t *ud = malloc(sizeof(ud_splitscreen_t));
  ud->ui = ui;

  tk_screen_t *splitscreen = tk_screen(tk, NULL, NULL);

  // create the screen layout
  TK_STACK_SCREEN(
      splitscreen,

      TK_PARENT->bg_mashup =
          bg_mashup(tk->bg, "3_tables", NULL, NULL, "g2hr_splitscreen");

      // title bar
      ud->titlebar = tk_label(tk, TK_PARENT, "6 CONTROLLERS FOUND",
                              GTA2_FONT_FSTYLE_WHITE_RED_NORMAL, 0);
      tk_el_padding(ud->titlebar, 315, 12, 0, 0);

      // content
      TK_STACK(
          splitscreen->el_content_container = TK_PARENT;
          tk_el_padding(TK_PARENT, 300, 170, 0, 0); TK_PARENT->userdata = ud;
          TK_PARENT->actionfunc = (void *)splitscreen_actionfunc;

          // players
          ud->players =
              tk_ctrl_arrowtext(tk, TK_PARENT, NULL /*bg*/, 0, ui->numbers,
                                TODO_controllers_found, "PLAYERS: ", NULL, NULL,
                                NULL, NULL, NULL /*bottom text*/);

          // screen layout
          ud->screen_layout = tk_ctrl_arrowtext(
              tk, TK_PARENT, NULL /*bg*/, 0, ui->letters, 1, "SCREEN LAYOUT: ",
              NULL, NULL, NULL, NULL, NULL /*bottom text*/);

          // map
          ud->map = tk_ctrl_arrowtext(tk, TK_PARENT, NULL /*bg*/, 0, ui->maps,
                                      ui->mmp->file_count, "MAP: ", NULL, NULL,
                                      "ENTER: CHOOSE FROM LIST", NULL, NULL);

          // game type
          ud->game_type =
              tk_ctrl_arrowtext(tk, TK_PARENT, NULL /*bg*/, 0, ui->game_types,
                                G2HR_UI_GAME_TYPES_COUNT, "GAME TYPE: ", NULL,
                                NULL, NULL, NULL, NULL /*bottom text*/);

          // time
          ud->time =
              tk_ctrl_arrowtext(tk, TK_PARENT, NULL /*bg*/, 0,
                                ui->multiplayer_time_values->values,
                                ui->multiplayer_time_values->count, "TIME: ",
                                " MIN", NULL, NULL, NULL, NULL /*bottom text*/);

          // cops
          ud->cops = tk_ctrl_boolean(tk, TK_PARENT, NULL, "COPS: ");

          // big play button. We don't have the huge font in red
          // for black backgrounds, but modifying argb_selected has
          // the same effect :)

          ud->play = tk_ctrl_button(tk, TK_PARENT, "PLAY!", NULL, NULL);
          ud->play->font_id = ud->play->font_id_selected =
              GTA2_FONT_FSTYLE_WHITE_BLACK_HUGE;
          ud->play->argb_selected = 0xffff0000;
          tk_el_padding(ud->play, 0, 20, 0, 0);

          );

      );
  return splitscreen;
}
