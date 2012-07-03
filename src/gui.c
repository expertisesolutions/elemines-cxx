/*
 * elemines: an EFL mines sweeper
 * Copyright (C) 2012 Jerome Pinot <ngc891@gmail.com>
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

static void
_about_del(__UNUSED__ void *data, __UNUSED__ Evas *e, Evas_Object *obj, __UNUSED__ void *event_info)
{
   evas_object_hide(obj);
}

void
_about(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *popup, *label;
   char buffer[256];

   /* Show the about window */
   popup = elm_win_inwin_add(window);
   elm_object_style_set(popup, "minimal_vertical");
   evas_object_show(popup);

   /* Construct a formatted label for the inwin */
   label = elm_label_add(window);
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
_pause_del(void *data, __UNUSED__ Evas *e, Evas_Object *obj, __UNUSED__ void *event_info)
{
   double current;

   /* compute the pause delay to remove it from timer */
   memcpy(&current, &data, sizeof(current));
   delay += ecore_time_get() - current;
   ecore_timer_thaw(etimer);
   evas_object_hide(obj);
}

void
_pause(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *popup, *label;
   double current;
   void *current_ptr = NULL;

   /* Show the pause window */
   popup = elm_win_inwin_add(window);
   evas_object_show(popup);

   /* pause the timer */
   current = ecore_time_get();
   memcpy(&current_ptr, &current, sizeof(current));
   ecore_timer_freeze(etimer);

   /* Construct a formatted label for the inwin */
   label = elm_label_add(window);
   elm_object_text_set(label, "Pause!");
   evas_object_show(label);
   elm_win_inwin_content_set(popup, label);

   /* Close the inwin when clicked */
   evas_object_event_callback_add(popup, EVAS_CALLBACK_MOUSE_DOWN, _pause_del, current_ptr);

}

Eina_Bool
gui(char *theme)
{
   Evas_Object *background, *vbox, *toolbar, *hbox, *icon, *blank;
   char str[8] = { 0 };
   int x, y;

   /* get the edje theme file */
   snprintf(edje_file, sizeof(edje_file), "%s/themes/%s.edj", PACKAGE_DATA_DIR, theme);
   if (access(edje_file, R_OK) != 0)
     {
        EINA_LOG_CRIT("Loading theme error: can not read %s", edje_file);
        return EINA_FALSE;
     }

   /* set general properties */
   window = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
   elm_win_title_set(window, PACKAGE);
   elm_win_autodel_set(window, EINA_TRUE);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   /* add a background */
   background = elm_bg_add(window);
   evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(window, background);
   evas_object_show(background);

   /* main box */
   vbox = elm_box_add(window);
   elm_box_homogeneous_set(vbox, EINA_FALSE);
   elm_win_resize_object_add(window, vbox);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(vbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(vbox);

   /* the toolbar */
   toolbar = elm_toolbar_add(window);
   elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
   evas_object_size_hint_weight_set(toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.0);
   evas_object_show(toolbar);
   elm_box_pack_end(vbox, toolbar);
   elm_toolbar_item_append(toolbar, "refresh", "Refresh", init, NULL);
   elm_toolbar_item_append(toolbar, "media-playback-pause", "Pause", _pause, NULL);
   elm_toolbar_item_append(toolbar, "help-about", "About", _about, NULL);

   /* box for timer and mine count */
   hbox = elm_box_add(window);
   elm_box_homogeneous_set(hbox, EINA_FALSE);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   elm_win_resize_object_add(window, hbox);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.1);
   evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(hbox);

   /* clock */
   icon = elm_icon_add(window);
   elm_icon_standard_set(icon, "clock");
   evas_object_size_hint_weight_set(icon, 0.15, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(icon);
   elm_box_pack_end(hbox, icon);

   /* timer */
   timer = elm_layout_add(window);
   elm_layout_file_set(timer, edje_file, "timer");
   evas_object_size_hint_weight_set(timer, 0.4, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(timer, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(timer);
   elm_box_pack_end(hbox, timer);

   /* bomb icon */
   icon = elm_layout_add(window);
   elm_layout_file_set(icon, edje_file, "mine_icon");
   evas_object_size_hint_weight_set(icon, 0.15, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(icon);
   elm_box_pack_end(hbox, icon);

   /* remaining mines */
   mines = elm_layout_add(window);
   elm_layout_file_set(mines, edje_file, "mines");
   evas_object_size_hint_weight_set(mines, 0.25, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(mines, EVAS_HINT_FILL, EVAS_HINT_FILL);
   snprintf(str, sizeof(str), "%03d", MINES);
   elm_object_part_text_set(mines, "mines", str);
   evas_object_show(mines);
   elm_box_pack_end(hbox, mines);
   elm_box_pack_end(vbox, hbox);

   /* add the main table for storing cells */
   table = elm_table_add(window);
   elm_table_homogeneous_set(table, EINA_TRUE);
   elm_win_resize_object_add(window, table);
   evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, table);
   evas_object_show(table);

   /* add a nice border around the board */
   for (x = 1; x < SIZE_X + 1; x++)
     {
        icon = elm_layout_add(window);
        elm_layout_file_set(icon, edje_file, "grass-s");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(table, icon, x, 0, 1, 1);
        evas_object_show(icon);

        icon = elm_layout_add(window);
        elm_layout_file_set(icon, edje_file, "grass-n");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(table, icon, x, SIZE_Y+1, 1, 1);
        evas_object_show(icon);
     }
   for (y = 1; y < SIZE_Y + 1; y++)
     {
        icon = elm_layout_add(window);
        elm_layout_file_set(icon, edje_file, "grass-w");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(table, icon, 0, y, 1, 1);
        evas_object_show(icon);

        icon = elm_layout_add(window);
        elm_layout_file_set(icon, edje_file, "grass-e");
        evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(icon);
        elm_table_pack(table, icon, SIZE_X + 1, y, 1, 1);
        evas_object_show(icon);
     }

   /* Add a blank cell on the bottom right to get a right/bottom margin */
   blank = elm_layout_add(window);
   elm_layout_file_set(blank, edje_file, "blank");
   evas_object_size_hint_weight_set(blank, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(blank, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(table, blank, SIZE_X+1, SIZE_Y+1, 1, 1);
   evas_object_show(blank);

   elm_object_cursor_set(table, ELM_CURSOR_HAND2);

   /* Get window's size from edje and resize it */
   x = atoi(edje_file_data_get(edje_file, "width"));
   y = atoi(edje_file_data_get(edje_file, "height"));

   evas_object_resize(window, x, y);
   elm_object_focus_set(window, EINA_TRUE);
   evas_object_show(window);

   return EINA_TRUE;
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
