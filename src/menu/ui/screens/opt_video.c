#include "../../../common/headers/common.h"
#include "../../tk/ctrl/controls.h"
#include "../../tk/toolkit.h"
#include "../ui.h"
#include <stdbool.h>

// USERDATA STRUCT
typedef struct {
  ui_t *ui;
  tk_el_t *fullscreen;
  tk_el_t *upscaling;
  tk_el_t *lighting;
  tk_el_t *gamma;
  tk_el_t *exploding_scores;

} ud_opt_video_t;

void opt_video_actionfunc(tk_t *tk, tk_el_t *el, tk_el_t *el_selected,
                          tk_action_t action, SDL_Keycode key) {
  ud_opt_video_t *ud = (ud_opt_video_t *)el->userdata;

  if (action == TK_ACTION_LEFT || action == TK_ACTION_RIGHT ||
      action == TK_ACTION_ENTER) {
    if (el_selected == ud->fullscreen) {
      bool is_fullscreen =
          ((ud_arrowtext_t *)ud->fullscreen->userdata)->entry_selected;

      ini_modify(ud->ui->ini_settings, "video", "fullscreen",
                 is_fullscreen ? "true" : "false", true);
      ui_apply_video_config(ud->ui);
    }

    ini_save(ud->ui->ini_settings, NULL, false, false);
  }
}

tk_screen_t *ui_screen_opt_video(tk_t *tk, ui_t *ui) {
  ud_opt_video_t *ud = calloc(1, sizeof(ud_opt_video_t));
  ud->ui = ui;

  tk_screen_t *ret = tk_screen(tk, ui->credits, NULL);

  TK_STACK_SCREEN(
      ret,
      TK_PARENT->bg_mashup = bg_mashup(tk->bg, NULL, "1_options", "1", NULL);

      TK_STACK(
          ret->el_content_container = TK_PARENT; TK_PARENT->userdata = ud;
          TK_PARENT->actionfunc = (void *)opt_video_actionfunc;
          tk_el_padding(TK_PARENT, 300, 150, 0, 0);

          // create controls
          ud->fullscreen = tk_ctrl_arrowtext(
              tk, TK_PARENT, NULL, 0, NULL, 0, "FULLSCREEN: ", NULL,
              "SEE ALSO: GIT.IO/G2HR_MENU",
              tk->wine ? "RESTART REQUIRED TO APPLY" : NULL, NULL, NULL);

          ud->upscaling = tk_ctrl_arrowtext(
              tk, TK_PARENT, NULL, 0, ui->menu_upscaling_values->pieces,
              ui->menu_upscaling_values->count, "UPSCALING: ", NULL,
              "ONLY FOR THE MENU", "DEFAULT: LINEAR", NULL, NULL);

          ud->lighting = tk_ctrl_arrowtext(
              tk, TK_PARENT, NULL, 0, ui->ingame_lighting_values->pieces,
              ui->ingame_lighting_values->count, "LIGHTING: ", NULL, NULL,
              "DEFAULT: NOON?", NULL, NULL);

          ud->gamma = tk_ctrl_arrowtext(tk, TK_PARENT, NULL, 0,
                                        ui->gamma_values->pieces,
                                        ui->gamma_values->count, "GAMMA: ",
                                        NULL, NULL, "DEFAULT: 10", NULL, NULL);

          ud->exploding_scores = tk_ctrl_arrowtext(
              tk, TK_PARENT, NULL, 0, NULL, 0, "SCORE EFFECTS: ", NULL,
              "INGAME EXPLODING SCORES", tk->wine ? "DEFAULT: ON" : NULL, NULL,
              NULL);

          // set values from config
          ((ud_arrowtext_t *)ud->fullscreen->userdata)->entry_selected =
              (strcmp(ini_read(ui->ini_settings, "video", "fullscreen"),
                      "true") == 0);

          ((ud_arrowtext_t *)ud->exploding_scores->userdata)->entry_selected =
              (strcmp(ini_read(ui->ini_settings, "video", "exploding_scores"),
                      "true") == 0);

          ));

  return ret;
}
