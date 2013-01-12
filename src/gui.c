/*
 * elemines: an EFL minesweeper
 * Copyright (C) 2012-2013 Jerome Pinot <ngc891@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "elemines.h"

static double pause_time = 0;

void
_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   etrophy_shutdown();
   elm_exit();
}

static void
_popup_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_hide(game.ui.popup);
}

static void
_show_score(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{

   Evas_Object *button, *leaderboard;

   game.ui.popup = elm_popup_add(game.ui.window);
   elm_object_part_text_set(game.ui.popup, "title,text", "High Scores");

   leaderboard = etrophy_score_layout_add(game.ui.popup, game.trophy.gamescore);
   elm_object_content_set(game.ui.popup, leaderboard);

   button = elm_button_add(game.ui.popup);
   elm_object_text_set(button, "OK");
   elm_object_part_content_set(game.ui.popup, "button1", button);
   evas_object_smart_callback_add(button, "clicked", _popup_del, NULL);
   evas_object_show(game.ui.popup);
}

static void
_config(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int number;
   Evas_Object *spin = data;

   number = elm_spinner_value_get(spin);
   if ( (number < 2) || (number > (SIZE_X * SIZE_Y)) )
       number = MINES;

   game.datas.mines_total = number;
   evas_object_hide(game.ui.popup);
   init(NULL, NULL, NULL);
}

static void
_show_config(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *button, *spin;

   game.ui.popup = elm_popup_add(game.ui.window);
   elm_object_part_text_set(game.ui.popup, "title,text", "Configuration");

   spin = elm_spinner_add(game.ui.window);
   elm_spinner_label_format_set(spin, "%.0f mines");
   elm_spinner_min_max_set(spin, 2, SIZE_X * SIZE_Y - 1);
   elm_spinner_value_set(spin, game.datas.mines_total);
   evas_object_size_hint_weight_set(spin, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(spin, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(spin);

   elm_object_content_set(game.ui.popup, spin);

   button = elm_button_add(game.ui.popup);
   elm_object_text_set(button, "OK");
   elm_object_part_content_set(game.ui.popup, "button1", button);
   evas_object_smart_callback_add(button, "clicked", _config, spin);
   evas_object_show(game.ui.popup);

}

static void
_about_del(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   evas_object_hide(obj);
}

static void
_about(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *popup, *label;
   char buffer[256];

   /* Show the about window */
   popup = elm_win_inwin_add(game.ui.window);
   elm_object_style_set(popup, "minimal_vertical");
   evas_object_show(popup);

   /* Construct a formatted label for the inwin */
   label = elm_label_add(game.ui.window);
   snprintf(buffer, sizeof(buffer), "<b>%s %s</b><br><br>"
            "%s<br><br>"
            "Pictures and sounds derived from:<br>"
            " - http://www.wesnoth.org/<br>",
            PACKAGE, VERSION, COPYRIGHT);
   elm_object_text_set(label, buffer);
   evas_object_show(label);
   elm_win_inwin_content_set(popup, label);

   /* Close the inwin when clicked */
   evas_object_event_callback_add(popup, EVAS_CALLBACK_MOUSE_DOWN, _about_del, NULL);
}

static void
_pause_del(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   /* compute the pause delay to remove it from timer */
   game.clock.delay += ecore_time_get() - pause_time;
   if (game.clock.etimer)
     {
        ecore_timer_thaw(game.clock.etimer);
     }
   else
     {
        game.clock.delay = 0;
     }
   evas_object_hide(obj);
}

void
_pause(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *popup, *layout;

   /* Show the pause window */
   popup = elm_win_inwin_add(game.ui.window);
   evas_object_show(popup);

   /* pause the timer */
   pause_time = ecore_time_get();
   if (game.clock.etimer) ecore_timer_freeze(game.clock.etimer);

   /* Construct a formatted label for the inwin */
   layout = elm_layout_add(game.ui.window);
   elm_layout_file_set(layout, game.edje_file, "pause");
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(layout);
   elm_win_inwin_content_set(popup, layout);

   /* Close the inwin when clicked */
   evas_object_event_callback_add(popup, EVAS_CALLBACK_MOUSE_DOWN, _pause_del, NULL);

}

Eina_Bool
gui(char *theme, Eina_Bool fullscreen)
{
   Evas_Object *background, *vbox, *toolbar, *hbox, *icon, *bg, *blank;
   int x, y;

   /* get the edje theme file */
   snprintf(game.edje_file, sizeof(game.edje_file), "%s/themes/%s.edj", PACKAGE_DATA_DIR, theme);
   if (access(game.edje_file, R_OK) != 0)
     {
        EINA_LOG_CRIT("Loading theme error: can not read %s", game.edje_file);
        return EINA_FALSE;
     }

   elm_theme_extension_add(NULL, game.edje_file);

   /* set general properties */
   game.ui.window = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
   elm_win_title_set(game.ui.window, PACKAGE);
   elm_win_autodel_set(game.ui.window, EINA_TRUE);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   if (fullscreen == EINA_TRUE)
     {
        elm_win_fullscreen_set(game.ui.window, EINA_TRUE);
        evas_object_move(game.ui.window, 0, 0);
     }

   /* init score system */
   etrophy_init();
   game.trophy.gamescore = etrophy_gamescore_load(PACKAGE);
   if (!game.trophy.gamescore)
     {
        game.trophy.gamescore = etrophy_gamescore_new(PACKAGE);
        game.trophy.level =  etrophy_level_new("standard");
        etrophy_gamescore_level_add(game.trophy.gamescore, game.trophy.level);
        game.trophy.level =  etrophy_level_new("custom");
        etrophy_gamescore_level_add(game.trophy.gamescore, game.trophy.level);
     }

   /* add a background */
   background = elm_bg_add(game.ui.window);
   evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(game.ui.window, background);
   evas_object_show(background);

   /* main box */
   vbox = elm_box_add(game.ui.window);
   elm_box_homogeneous_set(vbox, EINA_FALSE);
   elm_win_resize_object_add(game.ui.window, vbox);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(vbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(vbox);

   /* the toolbar */
   toolbar = elm_toolbar_add(game.ui.window);
   elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
   evas_object_size_hint_weight_set(toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.0);
   evas_object_show(toolbar);
   elm_box_pack_end(vbox, toolbar);
   elm_toolbar_item_append(toolbar, "refresh", "Refresh", init, NULL);
   elm_toolbar_item_append(toolbar, "media-playback-pause", "Pause", _pause, NULL);
   elm_toolbar_item_append(toolbar, "config", "Config.", _show_config, NULL);
   elm_toolbar_item_append(toolbar, "score", "Score", _show_score, NULL);
   elm_toolbar_item_append(toolbar, "help-about", "About", _about, NULL);
   elm_toolbar_item_append(toolbar, "close", "Quit", _quit, NULL);

   /* box for timer and mine count */
   hbox = elm_box_add(game.ui.window);
   elm_box_homogeneous_set(hbox, EINA_FALSE);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   elm_win_resize_object_add(game.ui.window, hbox);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.1);
   evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(hbox);

   /* timer */
   game.ui.timer = elm_layout_add(game.ui.window);
   elm_layout_file_set(game.ui.timer, game.edje_file, "timer");
   evas_object_size_hint_weight_set(game.ui.timer, 0.5, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(game.ui.timer, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(game.ui.timer);
   elm_box_pack_end(hbox, game.ui.timer);

   /* remaining mines */
   game.ui.mines = elm_layout_add(game.ui.window);
   elm_layout_file_set(game.ui.mines, game.edje_file, "mines");
   evas_object_size_hint_weight_set(game.ui.mines, 0.5, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(game.ui.mines, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(game.ui.mines);
   elm_box_pack_end(hbox, game.ui.mines);
   elm_box_pack_end(vbox, hbox);

   /* add the main table for storing cells */
   game.ui.table = elm_table_add(game.ui.window);
   elm_table_homogeneous_set(game.ui.table, EINA_TRUE);
   elm_win_resize_object_add(game.ui.window, game.ui.table);
   evas_object_size_hint_weight_set(game.ui.table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(game.ui.table, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, game.ui.table);
   evas_object_show(game.ui.table);

   /* white background to ensure consistent look */
   bg = elm_layout_add(game.ui.window);
   elm_layout_file_set(bg, game.edje_file, "white-bg");
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(game.ui.table, bg, 0, 0, SIZE_X+2, SIZE_Y+2);
   evas_object_show(bg);

   /* add a nice border around the board */
   for (x = 1; x < SIZE_X + 1; x++)
     {
        icon = elm_layout_add(game.ui.window);
        elm_layout_file_set(icon, game.edje_file, "grass-s");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(game.ui.table, icon, x, 0, 1, 1);
        evas_object_show(icon);

        icon = elm_layout_add(game.ui.window);
        elm_layout_file_set(icon, game.edje_file, "grass-n");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(game.ui.table, icon, x, SIZE_Y+1, 1, 1);
        evas_object_show(icon);
     }
   for (y = 1; y < SIZE_Y + 1; y++)
     {
        icon = elm_layout_add(game.ui.window);
        elm_layout_file_set(icon, game.edje_file, "grass-w");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(game.ui.table, icon, 0, y, 1, 1);
        evas_object_show(icon);

        icon = elm_layout_add(game.ui.window);
        elm_layout_file_set(icon, game.edje_file, "grass-e");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(game.ui.table, icon, SIZE_X + 1, y, 1, 1);
        evas_object_show(icon);
     }

   /* Add a blank cell on the bottom right to get a right/bottom margin */
   blank = elm_layout_add(game.ui.window);
   elm_layout_file_set(blank, game.edje_file, "blank");
   evas_object_size_hint_weight_set(blank, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(blank, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(game.ui.table, blank, SIZE_X+1, SIZE_Y+1, 1, 1);
   evas_object_show(blank);

   /* Get window's size from edje and resize it */
   x = atoi(edje_file_data_get(game.edje_file, "width"));
   y = atoi(edje_file_data_get(game.edje_file, "height"));

   evas_object_resize(game.ui.window, x, y);
   elm_object_focus_set(game.ui.window, EINA_TRUE);
   evas_object_show(game.ui.window);

   return EINA_TRUE;
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
