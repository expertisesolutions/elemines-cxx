/*
 * elemines: an EFL minesweeper
 * Copyright (C) 2012-2013 Jerome Pinot <ngc891@gmail.com> and various
 * contributors (see AUTHORS).
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
_show_score(game_struct& game)
{
   game.ui->popup = elm_popup_add(game.ui->window);
   elm_object_part_text_set(game.ui->popup, "title,text", _("High Scores"));

   /* we use the default layout from etrophy library */
   Evas_Object* leaderboard = etrophy_score_layout_add(game.ui->popup,
                                          game.trophy.gamescore);
   elm_object_content_set(game.ui->popup, leaderboard);

   Evas_Object* button = elm_button_add(game.ui->popup);
   elm_object_text_set(button, "OK");
   elm_object_part_content_set(game.ui->popup, "button1", button);
   evas_object_smart_callback_add(button, "clicked"
                                  , efl::eina::call(evas_object_smart_callback_add)
                                  , efl::eina::wrap(evas_object_smart_callback_add)
                                  (std::bind
                                   (
                                    [&game]() { evas_object_hide(game.ui->popup); }
                                    )));
   evas_object_show(game.ui->popup);
}

void
_show_config(game_struct& game)
{
   char buffer[512];

   game.ui->popup = elm_popup_add(game.ui->window);
   elm_object_part_text_set(game.ui->popup, "title,text", _("Configuration"));

   Evas_Object* vbox = elm_box_add(game.ui->window);
   elm_box_homogeneous_set(vbox, EINA_FALSE);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(vbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(vbox);

   // spinner to change mine number
   Evas_Object* spin = elm_spinner_add(game.ui->window);
   elm_spinner_label_format_set(spin, _("%.0f mines"));
   elm_spinner_min_max_set(spin, 2, game.datas.x_theme * game.datas.y_theme - 1);
   elm_spinner_value_set(spin, game.datas.mines_total);
   evas_object_size_hint_weight_set(spin, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(spin, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(spin);
   elm_box_pack_end(vbox, spin);

   // Add some comments about scoring
   Evas_Object* label = elm_label_add(game.ui->window);
   elm_label_line_wrap_set(label, ELM_WRAP_WORD);
   snprintf(buffer, sizeof(buffer), _("<b>Note:</b> default mine number is "
            "<b>%d</b> with scoring in <b>%s</b> category. If you change "
            "the mine number to something else, your score will be put in the "
            "<b>%s</b> category."), game.datas.mines_theme, STANDARD, CUSTOM);
   elm_object_text_set(label, buffer);
   evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(label);
   elm_box_pack_end(vbox, label);

   elm_object_content_set(game.ui->popup, vbox);

   // button for validating
   Evas_Object* button = elm_button_add(game.ui->popup);
   elm_object_text_set(button, _("OK"));
   elm_object_part_content_set(game.ui->popup, "button1", button);

   evas_object_smart_callback_add
     (button, "clicked"
      , efl::eina::call(evas_object_smart_callback_add)
      , efl::eina::wrap(evas_object_smart_callback_add)
      (
       std::bind(
       [spin,&game]()
       {
         int number = elm_spinner_value_get(spin);
         if ( (number < 2) || (number > (game.datas.x_theme * game.datas.y_theme)) )
           number = game.datas.mines_theme;
         
         game.datas.mines_total = number;
         evas_object_hide(game.ui->popup);
       }
       ))
      );
   evas_object_show(game.ui->popup);
}

void
_show_about(game_struct& game)
{
   char buffer[256];

   game.ui->popup = elm_popup_add(game.ui->window);
   elm_object_part_text_set(game.ui->popup, "title,text", _("About"));

   // Construct a formatted label for the about popup
   Evas_Object* label = elm_label_add(game.ui->window);
   snprintf(buffer, sizeof(buffer), _("<b>%s %s</b><br><br>"
            "%s<br><br>"
            "Pictures derived from Battle For Wesnoth:<br>"
            "http://www.wesnoth.org/<br>"),
            PACKAGE, VERSION, COPYRIGHT);
   elm_object_text_set(label, buffer);
   evas_object_show(label);

   elm_object_content_set(game.ui->popup, label);

   Evas_Object* button = elm_button_add(game.ui->popup);
   elm_object_text_set(button, _("OK"));
   elm_object_part_content_set(game.ui->popup, "button1", button);
   evas_object_smart_callback_add(button, "clicked"
                                  , efl::eina::call(evas_object_smart_callback_add)
                                  , efl::eina::wrap(evas_object_smart_callback_add)
                                  (std::bind
                                   (
                                    [&game]() { evas_object_hide(game.ui->popup); }
                                    )));
   evas_object_show(game.ui->popup);
}

void
_pause(game_struct& game)
{
   Evas_Object *popup, *layout;

   // Show the pause window
   popup = elm_win_inwin_add(game.ui->window);
   evas_object_show(popup);

   // pause the timer
   pause_time = ecore_time_get();
   if (game.clock.etimer) ecore_timer_freeze(game.clock.etimer);

   // Construct a formatted label for the inwin
   layout = elm_layout_add(game.ui->window);
   elm_layout_file_set(layout, game.edje_file.c_str(), "pause");
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(layout);
   elm_win_inwin_content_set(popup, layout);

   // Close the inwin when clicked
   evas_object_event_callback_add
     (popup, EVAS_CALLBACK_MOUSE_DOWN
      , efl::eina::call(evas_object_event_callback_add)
      , efl::eina::wrap(evas_object_event_callback_add)
      (std::bind
       (
        [&game](Evas_Object* obj)
        {
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
        , std::placeholders::_2
        )));
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
