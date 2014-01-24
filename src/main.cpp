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

extern "C" Ecore_Getopt optdesc;

struct elm_shutdown
{
  ~elm_shutdown()
  {
    ::elm_shutdown();
  }
};

void
_pause(game_struct&);
void
_show_about(game_struct&);
void
_show_score(game_struct&);
void
_show_config(game_struct&);
void
_click(game_struct&, const char *emission, const char *source);

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   struct elm_shutdown elm_shutdown;
   char *theme = const_cast<char*>("default");
   int args;
   int mines_total = 0;
   Eina_Bool debug = EINA_FALSE;
   Eina_Bool fullscreen = EINA_FALSE;
   Eina_Bool quit_option = EINA_FALSE;
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(debug),
     ECORE_GETOPT_VALUE_BOOL(fullscreen),
     ECORE_GETOPT_VALUE_STR(theme),
     ECORE_GETOPT_VALUE_INT(mines_total),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   textdomain(PACKAGE);
#endif

   ui_struct ui(theme, fullscreen);
   game_struct game(theme, mines_total, ui);

   elm_theme_extension_add(NULL, game.edje_file.c_str());

   /* Get user values */
   args = ecore_getopt_parse(&optdesc, values, argc, argv);
   if (args < 0)
     {
       EINA_LOG_CRIT("Could not parse arguments.");
       return 0;
     }
   else if (quit_option)
     {
       return 0;
     }

   edje_object_signal_callback_add(ui.edje, "mouse,clicked,*", "board\\[*\\]:overlay"
                                   , efl::eina::call(edje_object_signal_callback_add)
                                   , efl::eina::wrap(edje_object_signal_callback_add)
                                   (std::bind(_click, std::ref(game)
                                              , std::placeholders::_2
                                              , std::placeholders::_3)));

   /* Add item to the toolbar now so everything is initialized in the UI */
   elm_toolbar_item_append
     (ui.toolbar, "refresh", _("Refresh")
      , efl::eina::call(elm_toolbar_item_append)
      , efl::eina::wrap(elm_toolbar_item_append)
      (std::bind
       ([&game,&ui, mines_total, theme]()
        {
          printf("refresh\n");
          game = game_struct(theme, mines_total, ui);
          
          edje_object_signal_callback_del_full
            (game.ui->edje, "mouse,clicked,*", "board\\[*\\]:overlay"
             , efl::eina::call(edje_object_signal_callback_add), 0);
          edje_object_signal_callback_add(ui.edje, "mouse,clicked,*", "board\\[*\\]:overlay"
                                          , efl::eina::call(edje_object_signal_callback_add)
                                          , efl::eina::wrap(edje_object_signal_callback_add)
                                          (std::bind(_click, std::ref(game)
                                                     , std::placeholders::_2
                                                     , std::placeholders::_3)));
        }
        )));
   elm_toolbar_item_append(ui.toolbar, "pause", _("Pause")
                           , efl::eina::call(elm_toolbar_item_append)
                           , efl::eina::wrap(elm_toolbar_item_append)
                           (std::bind
                            (_pause, std::ref(game))));
   elm_toolbar_item_append(ui.toolbar, "config", _("Configuration")
                           , efl::eina::call(elm_toolbar_item_append)
                           , efl::eina::wrap(elm_toolbar_item_append)
                           (std::bind(_show_config, std::ref(game))));
   elm_toolbar_item_append(ui.toolbar, "score", _("Scores")
                           , efl::eina::call(elm_toolbar_item_append)
                           , efl::eina::wrap(elm_toolbar_item_append)
                           (std::bind(_show_score, std::ref(game))));
   elm_toolbar_item_append(ui.toolbar, "about", _("About")
                           , efl::eina::call(elm_toolbar_item_append)
                           , efl::eina::wrap(elm_toolbar_item_append)
                           (std::bind(_show_about, std::ref(game))));

   elm_toolbar_item_append(ui.toolbar, "quit", _("Quit")
                           , efl::eina::call(elm_toolbar_item_append)
                           , efl::eina::wrap(elm_toolbar_item_append)
                           (std::bind
                            (
                             [&game]()
                             {
                               etrophy_shutdown();
                               elm_theme_extension_del(NULL, game.edje_file.c_str());
                               elm_theme_flush(NULL);
                               elm_exit();
                             }
                             )));
   elm_object_focus_set(ui.window, EINA_TRUE);
   evas_object_show(ui.window);

   elm_run();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
