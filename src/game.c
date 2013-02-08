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

double t0;
double dt = 0.1;

static int
_scoring(void)
{
   int score;
   double end_time;
   char *user;

   /* compute score using time, board size and mines count */
   end_time = ecore_loop_time_get() - t0 - game.clock.delay;
   score = (SIZE_X * SIZE_Y * game.datas.mines_total)
            - (10 * end_time);
   /* Don't be rude with bad players */
   if ( score < 0 ) score = 0;

   /* get system username for name */
   user = getenv("USER");

   /* add the score */
   game.trophy.escore = etrophy_score_new(user, score);
   /* Level is Standard if using default board values */
   if (game.datas.mines_total == MINES)
     {
        game.trophy.game_type = STANDARD;
     }
   else
     {
        game.trophy.game_type = CUSTOM;
     }

   game.trophy.level = etrophy_gamescore_level_get(game.trophy.gamescore,
                                                   game.trophy.game_type);
   etrophy_level_score_add(game.trophy.level, game.trophy.escore);
   etrophy_gamescore_save(game.trophy.gamescore, NULL);

   return score;

}

static Eina_Bool
_timer(void *data __UNUSED__)
{
   char str[8] = { 0 };
   int min = 0;
   double t;

   if (game.clock.started == EINA_FALSE)
     return EINA_FALSE;

   t = ecore_loop_time_get() - t0 - game.clock.delay;
   while (t >= 60)
     {
        t -= 60;
        min++;
     }

   snprintf(str, sizeof(str), "%02d:%04.1f", min, t);
   if (game.datas.counter != 0)
     elm_object_part_text_set(game.ui.timer, "time", str);

   return EINA_TRUE;
}

static void
_finish(int x, int y, Eina_Bool win)
{
   int i,j, score;
   char str[255];

   game.clock.started = EINA_FALSE;

   /* show bombs */
   for (i = 1; i < SIZE_X+1; i++)
     {
        for (j = 1; j < SIZE_Y+1; j++)
          {
             /* disable click */
             evas_object_event_callback_del(matrix[i][j].layout,
                                            EVAS_CALLBACK_MOUSE_DOWN, click);
             if (win == EINA_TRUE)
               {
                  elm_object_signal_emit(matrix[i][j].layout, "win", "");
               }
             else
               {
                  if (matrix[i][j].mine == 1)
                    elm_object_signal_emit(matrix[i][j].layout, "bomb", "");
                  elm_object_signal_emit(matrix[i][j].layout, "lose", "");
               }
          }
     }
   /* highlight the fatal bomb */
   if (win == EINA_FALSE) elm_object_signal_emit(matrix[x][y].layout,
                                                 "boom", "");
   if (win == EINA_TRUE)
     {
        /* prepare the congratulation message */
        game.ui.congrat = elm_layout_add(game.ui.window);
        elm_layout_file_set(game.ui.congrat, game.edje_file, "congratulation");
        evas_object_size_hint_weight_set(game.ui.congrat, EVAS_HINT_EXPAND,
                                                          EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(game.ui.congrat, EVAS_HINT_FILL,
                                                         EVAS_HINT_FILL);
        elm_table_pack(game.ui.table, game.ui.congrat, 1, 1, SIZE_X, SIZE_Y);

        score = _scoring();
        snprintf(str, sizeof(str), "Score: %d", score);
        evas_object_show(game.ui.congrat);
        elm_object_part_text_set(game.ui.congrat, "score", str);
        if ( score >= 
             etrophy_gamescore_level_hi_score_get(game.trophy.gamescore,
                                                  game.trophy.game_type) )
          {
             elm_object_part_text_set(game.ui.congrat, "best score",
                                      "High Score!!");
          }
        else
          {
             elm_object_part_text_set(game.ui.congrat, "best score", "");
          }
        elm_object_signal_emit(game.ui.congrat, "you win", "");
     }

   if (game.clock.etimer)
     {
        ecore_timer_del(game.clock.etimer);
        game.clock.etimer = NULL;
     }
}

static void
_clean(int x, int y, Evas_Object *obj)
{
   /* we are out of board */
   if (x == 0 || x == SIZE_X+1 || y == 0 || y == SIZE_Y+1)
     return;

   /* do nothing if the square is already uncovered */
   if (matrix[x][y].uncover == 1)
      return;

   /* flagged square can not be opened */
   if (matrix[x][y].flag == 1)
      return;

   /* no mine */
   if (matrix[x][y].mine == 0)
     {
        int scenery;
        char str[8];

        /* clean scenery */
        elm_object_signal_emit(obj, "noflowers", "");
        elm_object_signal_emit(obj, "nomushrooms", "");
        elm_object_signal_emit(obj, "digging", "");
        elm_object_signal_emit(obj, "clean", "");
        /* add some stones */
        scenery = (int)((double)100 * rand() / RAND_MAX + 1);
        if (scenery < 15)
          elm_object_signal_emit(obj, "stones", "");
        matrix[x][y].uncover = 1;
        /* at least 1 neighbour */
        if (matrix[x][y].neighbours != 0)
          {
             snprintf(str, sizeof(str), "%d", matrix[x][y].neighbours);
             elm_object_signal_emit(obj, str, "");
             elm_object_part_text_set(obj, "hint", str);
          }
        /* no neighbour */
        else
          {
             int i, j;
             for (i=x-1; i<=x+1; i++)
               {
                  for (j=y-1; j<=y+1; j++)
                    {
                       _clean(i, j, matrix[i][j].layout);
                    }
               }
          }
        /* keep track of this empty spot */
        game.datas.counter--;
        if (game.datas.counter == 0)
          _finish(x, y, EINA_TRUE);
     }
   else /* BOOM! */
     {
        _finish(x, y, EINA_FALSE);
     }
   return;

}

void
click(void *data, Evas *e __UNUSED__, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   int x, y;
   int coord[2];
   char str[16];

   /* get back the coordinates of the cell */
   memcpy(coord, data, sizeof(coord));

   x = coord[0];
   y = coord[1];

   /* if we push 1st mouse button and there is no flag */
   if (ev->button == 1 && matrix[x][y].flag == 0)
     {
        if (game.clock.started == EINA_FALSE)
          {
             game.clock.started = EINA_TRUE;
             t0 = ecore_time_get();
             game.clock.etimer = ecore_timer_add(dt, _timer, NULL);
          }

        _clean(x, y, obj);
     }

   /* second button: put a flag */
   if (ev->button == 3)
     {
        if (matrix[x][y].uncover != 1)
          {
             if (matrix[x][y].flag == 0) /* set flag */
               {
                  elm_object_signal_emit(obj, "flag", "");
                  matrix[x][y].flag = 1;
                  game.datas.remain--;
               }
             else /* already a flag, remove it */
               {
                  elm_object_signal_emit(obj, "default", "");
                  matrix[x][y].flag = 0;
                  game.datas.remain++;
               }
          }

        /* show the remaining mines */
        snprintf(str, sizeof(str), "%d/%d", game.datas.remain,
                 game.datas.mines_total);

        elm_object_part_text_set(game.ui.mines, "mines", str);
     }

   /* middle button: open rest if we have enough mines */
   if ( (ev->button == 2) && (matrix[x][y].uncover == 1) )
     {
        int i, j;
        int flags = 0;

        /* count surrounding flags */
        for (i=x-1; i<=x+1; i++)
          {
             for (j=y-1; j<=y+1; j++)
               {
                  if (!((i == x) && (j == y)) && (matrix[i][j].flag == 1))
                    flags++;
               }
          }
        /* open surrounding squares if correct number of flags is set */
        if (flags == matrix[x][y].neighbours)
          {
             for (i=x-1; i<=x+1; i++)
               {
                  for (j=y-1; j <=y+1; j++)
                    {
                       _clean(i, j, matrix[i][j].layout);
                    }
               }
          }
     }
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
