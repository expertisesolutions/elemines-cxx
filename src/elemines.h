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

#ifndef ELEMINES_H
#define ELEMINES_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <Eina.h>
#include <Evas.h>
#include <Ecore_Getopt.h>
#include <Edje.h>
#include <Elementary.h>
#include <Etrophy.h>

#include <Eina.hh>

#include "gettext.h"

#define _(String) gettext(String)
#define N_(String) gettext_noop (String)

#define COPYRIGHT "Copyright © 2012-2013 Jérôme Pinot <ngc891@gmail.com> and various contributors (see AUTHORS)."

#define STANDARD _("Standard")
#define CUSTOM _("Custom")

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Elemines_Cell Elemines_Cell;
typedef struct _Elemines_Walker Elemines_Walker;

/* structure to hold datas for each cell */
struct _Elemines_Cell {
   unsigned char neighbours : 4;  /* (0-8, 9 for bomb) */
   Eina_Bool mine : 1;            /* (0/1) */
   Eina_Bool flag : 1;            /* (0/1) */
   Eina_Bool uncover : 1;         /* (0/1) */
};

/* global variables */
struct ui_struct {
   ui_struct(char* theme, Eina_Bool fullscreen)
   {
     std::size_t s = snprintf(0, 0, "%s/themes/%s.edj"
                              , PACKAGE_DATA_DIR, theme);
     edje_file.resize(s+1);
     snprintf(&edje_file[0], edje_file.size(), "%s/themes/%s.edj"
              , PACKAGE_DATA_DIR, theme);
     edje_file.pop_back();

     /* set general properties */
     window = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
     elm_win_title_set(window, PACKAGE);
     elm_win_autodel_set(window, EINA_TRUE);
     elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
     /* add a background */
     Evas_Object *background = elm_bg_add(window);
     elm_bg_file_set(background, edje_file.c_str(), "bg");
     evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND,
                                      EVAS_HINT_EXPAND);
     elm_win_resize_object_add(window, background);
     evas_object_show(background);
     
     /* main box */
     Evas_Object* vbox = elm_box_add(window);
     elm_box_homogeneous_set(vbox, EINA_FALSE);
     elm_win_resize_object_add(window, vbox);
     evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(vbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
     evas_object_show(vbox);
     
     /* the toolbar */
     toolbar = elm_toolbar_add(window);
     elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_SCROLL);
     //elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
     evas_object_size_hint_weight_set(toolbar, 0.0, 0.0);
     evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.0);
     evas_object_show(toolbar);
     elm_box_pack_end(vbox, toolbar);

     /* box for timer and mine count */
     Evas_Object* hbox = elm_box_add(window);
     elm_box_homogeneous_set(hbox, EINA_FALSE);
     elm_box_horizontal_set(hbox, EINA_TRUE);
     evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.1);
     evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
     evas_object_show(hbox);

     /* timer */
     timer = elm_layout_add(window);
     elm_layout_file_set(timer, edje_file.c_str(), "timer");
     evas_object_size_hint_weight_set(timer, 0.5, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(timer, EVAS_HINT_FILL,
                                     EVAS_HINT_FILL);
     evas_object_show(timer);
     elm_box_pack_end(hbox, timer);

     /* remaining mines */
     mines = elm_layout_add(window);
     elm_layout_file_set(mines, edje_file.c_str(), "mines");
     evas_object_size_hint_weight_set(mines, 0.5, EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(mines, EVAS_HINT_FILL,
                                     EVAS_HINT_FILL);
     evas_object_show(mines);
     elm_box_pack_end(hbox, mines);
     elm_box_pack_end(vbox, hbox);

     /* add the main table for storing cells */
     table = elm_layout_add(window);
     elm_layout_file_set(table, edje_file.c_str(), "board");
     evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND,
                                      EVAS_HINT_EXPAND);
     evas_object_size_hint_align_set(table, EVAS_HINT_FILL,
                                     EVAS_HINT_FILL);
     elm_box_pack_end(vbox, table);
     evas_object_show(table);

     if (fullscreen)
       {
         /* use conformant */
         elm_win_conformant_set(window, EINA_TRUE);
         elm_win_fullscreen_set(window, EINA_TRUE);
         evas_object_move(window, 0, 0);
         Evas_Object* conform = elm_conformant_add(window);
         evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND,
                                                  EVAS_HINT_EXPAND);
         evas_object_show(conform);
         elm_object_content_set(conform, vbox);
       }
     else
       {
         /* Get window's size from edje and resize it */
         char *str1, *str2;
         int x = atoi(str1 = edje_file_data_get(edje_file.c_str(), "width"));
         int y = atoi(str2 = edje_file_data_get(edje_file.c_str(), "height"));
         free(str1);
         free(str2);
         evas_object_resize(window, x, y);
       }

     ///  board
     edje = elm_layout_edje_get(table);

    elm_theme_extension_add(NULL, edje_file.c_str());
   }

   std::string edje_file;
   Evas_Object *window;
   Evas_Object *table;
   Evas_Object *timer;
   Evas_Object *mines;
   Evas_Object *popup;
   Evas_Object *toolbar;
   Evas_Object *edje;
};

struct datas_struct {
   int x_theme;
   int y_theme;
   int mines_total;
   int mines_theme;
   int remain;
   int counter;
};

struct clock_struct {
   clock_struct()
     : started(false), etimer(0)
     , delay(0.0)
   {}
   Eina_Bool started;
   Ecore_Timer *etimer;
   double delay;
};

struct trophy_struct {
   trophy_struct()
     : gamescore(0), level(0), escore(0)
     , game_type(0)
   {}

   Etrophy_Gamescore *gamescore;
   Etrophy_Level *level;
   Etrophy_Score *escore;
   char *game_type;
};

struct game_struct;

struct game_struct {

  game_struct(const char* theme, int mines_total, ui_struct& ui)
    : ui(&ui)
  {
    elm_object_signal_emit(this->ui->table, "reset", "");   

    clock.started = EINA_FALSE;
    clock.delay = 0;
    datas.mines_total = mines_total;

    std::size_t s = snprintf(0, 0, "%s/themes/%s.edj"
                             , PACKAGE_DATA_DIR, theme);
    edje_file.resize(s+1);
    snprintf(&edje_file[0], edje_file.size(), "%s/themes/%s.edj"
             , PACKAGE_DATA_DIR, theme);
    edje_file.pop_back();

    char *str1 = 0, *str2 = 0;
    /* get board size from theme */
    if (((str1 = edje_file_data_get(edje_file.c_str(), "SIZE_X")) != NULL)
        && ((str2 = edje_file_data_get(edje_file.c_str(), "SIZE_Y")) != NULL))
      {
        datas.x_theme = atoi(str1);
        datas.y_theme = atoi(str2);
        free(str1);
        free(str2);
      }
    else
      {
        EINA_LOG_CRIT("Loading theme error: can not read the SIZE_? value in %s", edje_file.c_str());
        if (str1) free(str1);
        if (str2) free(str2);
        throw std::runtime_error("");
      }

    /* get default mines count from theme */
    if ((str1 = edje_file_data_get(edje_file.c_str(), "MINES")) != NULL)
      {
        datas.mines_theme = atoi(str1);
        free(str1);
      }
    else
      {
        EINA_LOG_CRIT("Loading theme error: can not read the MINES value in %s", edje_file.c_str());
        if (str1) free(str1);
        throw std::runtime_error("");
      }
    
    /* Validate input values */
    if (datas.mines_total == 0)
      datas.mines_total =  datas.mines_theme;
    if (datas.mines_total < 0)
      datas.mines_total = 1;
    if (datas.mines_total > (datas.x_theme * datas.y_theme - 1))
      datas.mines_total = datas.x_theme * datas.y_theme - 1;

    datas.remain = datas.mines_total;
    datas.counter = datas.x_theme * datas.y_theme - datas.mines_total;

    /* init score system */
    etrophy_init();
    trophy.gamescore = etrophy_gamescore_load(PACKAGE);
    if (!trophy.gamescore)
      {
        trophy.gamescore = etrophy_gamescore_new(PACKAGE);
        trophy.level =  etrophy_level_new(STANDARD);
        etrophy_gamescore_level_add(trophy.gamescore, trophy.level);
        trophy.level =  etrophy_level_new(CUSTOM);
        etrophy_gamescore_level_add(trophy.gamescore, trophy.level);
      }

    matrix.resize(datas.x_theme + 2, datas.y_theme + 2);

    for (std::size_t i = 0; i < datas.mines_total; i++)
      {
        /* random coordinates */
        std::size_t x = (int)((double)datas.x_theme * rand() / RAND_MAX + 1);
        std::size_t  y = (int)((double)datas.y_theme * rand() / RAND_MAX + 1);

        if (matrix[x][y].mine == 0 )
          matrix[x][y].mine = 1;
        else /* if there is already a bomb here, try again */
          i--;
      }

    // init matrix
    for(std::size_t x = 0u; x != matrix.x(); ++x)
      for(std::size_t y = 0u; y != matrix.y(); ++y)
        {
          char tmp[128];
          sprintf(tmp, "board[%i,%i]:reset", x, y);
          elm_object_signal_emit(this->ui->table, tmp, "");

          int scenery = (int)((double)100 * rand() / RAND_MAX + 1);
          if (scenery < 15)
            {
              sprintf(tmp, "board[%i,%i]:flowers", x, y);
              elm_object_signal_emit(this->ui->table, tmp, "");
            }
          if ((scenery > 12) && (scenery < 18))
            {
              sprintf(tmp, "board[%i,%i]:mushrooms", x, y);
              elm_object_signal_emit(this->ui->table, tmp, "");
            }
        }

    /* 2nd table: neighbours */
    // it = _walk(1, 1, game.datas.x_theme, game.datas.y_theme);
    for(std::size_t x = 1; x != matrix.x() - 1; ++x)
      for(std::size_t y = 1; y != matrix.y() - 1; ++y)
        {
          int neighbours = 
            matrix[x-1][y-1].mine
            + matrix[x-1][y].mine
            + matrix[x-1][y+1].mine
            + matrix[x][y-1].mine
            + matrix[x][y+1].mine
            + matrix[x+1][y-1].mine
            + matrix[x+1][y].mine
            + matrix[x+1][y+1].mine;

          matrix[x][y].neighbours = neighbours;
        }
  }

  struct matrix_struct
  {
    typedef std::size_t size_type;

    matrix_struct()
      : width(0u)
    {
    }
    matrix_struct(int x, int y)
    {
      resize(x, y);
    }

    void resize(int x, int y)
    {
      _cells.resize(x*y);
      width = x;
    }
    
    size_type x()
    {
      return width;
    }
    size_type y()
    {
      return _cells.size()/width;
    }

    Elemines_Cell* operator[](std::size_t y)
    {
      return &_cells[y*width];
    }

    std::vector<Elemines_Cell> _cells;
    std::size_t width;
  } matrix;

   double t0;
   std::string edje_file;
   struct ui_struct* ui;
   struct datas_struct datas;
   struct clock_struct clock;
   struct trophy_struct trophy;
};

// extern struct game_struct game;

struct _Elemines_Walker
{
   Elemines_Cell *cell;
   const char *target;
   unsigned char x;
   unsigned char y;
};

/* global functions */
void show_help(void);
void show_version(void);
Eina_Iterator *_walk(unsigned char x, unsigned char y, unsigned char w, unsigned char h);

#ifdef __cplusplus
}
#endif

#endif

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
