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
_finish(const char *cell, int x, int y, Eina_Bool win)
{
   Evas_Object *edje;
   int i,j, score;
   char str[255];

   game.clock.started = EINA_FALSE;

   edje = elm_layout_edje_get(game.ui.table);

   /* disable click */           
   edje_object_signal_callback_del_full(edje, "mouse,clicked,*", "board\\[*\\]:overlay", _click, NULL);

   /* show bombs */
   for (i = 1; i < SIZE_X+1; i++)
     {
        for (j = 1; j < SIZE_Y+1; j++)
          {
             if (win == EINA_TRUE)
               {
                  edje_object_signal_emit(matrix[i][j].layout, "win", "");
               }
             else
               {
                  if (matrix[i][j].mine == 1)
                    edje_object_signal_emit(matrix[i][j].layout, "bomb", "");
                  edje_object_signal_emit(matrix[i][j].layout, "lose", "");
               }
          }
     }
   /* highlight the fatal bomb */
   if (win == EINA_FALSE)
     edje_object_signal_emit(matrix[x][y].layout, "boom", "");
   else
     {
        /* prepare the congratulation message */
        edje_object_signal_emit(edje, "congrat", "");

        score = _scoring();
        snprintf(str, sizeof(str), "Score: %d", score);
        elm_object_part_text_set(game.ui.table, "congrat:score", str);
        if ( score >= 
             etrophy_gamescore_level_hi_score_get(game.trophy.gamescore,
                                                  game.trophy.game_type) )
          {
             elm_object_part_text_set(game.ui.table, "congrat:best score",
                                      "High Score!!");
          }
        else
          {
             elm_object_part_text_set(game.ui.table, "congrat:best score", "");
          }
        elm_object_signal_emit(game.ui.table, "congrat:you win", "");
     }

   if (game.clock.etimer)
     {
        ecore_timer_del(game.clock.etimer);
        game.clock.etimer = NULL;
     }
}

static void
_clean(const char *cell, int x, int y, Evas_Object *obj)
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
        matrix[x][y].uncover = 1;

        /* add some stones */
        scenery = (int)((double)100 * rand() / RAND_MAX + 1);
        if (scenery < 15)
          edje_object_signal_emit(obj, "stones", "");

	edje_object_signal_emit(obj, "digg", "");
        /* at least 1 neighbour */
        if (matrix[x][y].neighbours != 0)
          {
             snprintf(str, sizeof(str), "%d", matrix[x][y].neighbours);
             edje_object_signal_emit(obj, str, "");
          }
        /* no neighbour */
        else
          {
             int i, j;
             for (i=x-1; i<=x+1; i++)
               {
                  for (j=y-1; j<=y+1; j++)
                    {
                       char tmp[128];
                       sprintf(tmp, "board[%i/%i]:overlay", i, j);
                       _clean(tmp, i, j, matrix[i][j].layout);
                    }
               }
          }
        /* keep track of this empty spot */
        game.datas.counter--;
        if (game.datas.counter == 0)
          _finish(cell, x, y, EINA_TRUE);
     }
   else /* BOOM! */
     {
        _finish(cell, x, y, EINA_FALSE);
     }
   return;

}

void
_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   int x, y;
   char str[16];

   /* get back the coordinates of the cell */
   sscanf(source, "board[%i/%i]:overlay", &x, &y);

   /* if we push 1st mouse button and there is no flag */
   if (!strcmp(emission, "mouse,clicked,1") && matrix[x][y].flag == 0)
     {
        if (game.clock.started == EINA_FALSE)
          {
             game.clock.started = EINA_TRUE;
             t0 = ecore_time_get();
             game.clock.etimer = ecore_timer_add(dt, _timer, NULL);
          }

        _clean(source, x, y, matrix[x][y].layout);
     }

   /* second button: put a flag */
   if (!strcmp(emission, "mouse,clicked,3"))
     {
        if (matrix[x][y].uncover != 1)
          {
             if (matrix[x][y].flag == 0) /* set flag */
               {
                  edje_object_signal_emit(matrix[x][y].layout, "flag", "");
                  matrix[x][y].flag = 1;
                  game.datas.remain--;
               }
             else /* already a flag, remove it */
               {
                  edje_object_signal_emit(matrix[x][y].layout, "default", "");
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
   if (!strcmp(emission, "mouse,clicked,2") && (matrix[x][y].uncover == 1) )
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
                       char tmp[128];
                       sprintf(tmp, "board[%i/%i]:overlay", i, j);
                       _clean(tmp, i, j, matrix[i][j].layout);
                    }
               }
          }
     }
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
