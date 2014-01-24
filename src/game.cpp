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

static int
_scoring(game_struct& game)
{
   int score;
   double end_time;
   char *user;

   /* compute score using time, board size and mines count */
   end_time = ecore_loop_time_get() - game.t0 - game.clock.delay;
   score = (game.datas.x_theme * game.datas.y_theme * game.datas.mines_total)
            - (10 * end_time);
   /* Don't be rude with bad players */
   if (score < 0) score = 0;

   /* get system username for name */
   user = getenv("USER");

   /* add the score */
   game.trophy.escore = etrophy_score_new(user, score);
   /* Level is Standard if using default board values */
   if (game.datas.mines_total == game.datas.mines_theme)
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
_timer(game_struct& game)
{
   char str[8] = { 0 };
   int min = 0;
   double t;

   if (game.clock.started == EINA_FALSE)
     return EINA_FALSE;

   t = ecore_loop_time_get() - game.t0 - game.clock.delay;
   while (t >= 60)
     {
        t -= 60;
        min++;
     }

   snprintf(str, sizeof(str), "%02d:%04.1f", min, t);
   if (game.datas.counter != 0)
     elm_object_part_text_set(game.ui->timer, "time", str);

   return EINA_TRUE;
}

static void
_finish(game_struct& game, unsigned char x, unsigned char y, Eina_Bool win)
{
   int score;
   char str[255];
   char target[255];

   snprintf(target, sizeof(target), "board[%i,%i]", x, y);

   game.clock.started = EINA_FALSE;

   /* disable click */           
   edje_object_signal_callback_del_full
     (game.ui->edje, "mouse,clicked,*", "board\\[*\\]:overlay"
      , efl::eina::call(edje_object_signal_callback_add), 0);


   /* show bombs */
   for(std::size_t x = 1; x != game.matrix.x()-1; ++x)
     for(std::size_t y = 1; y != game.matrix.y()-1; ++y)
       {
         if (win == EINA_TRUE)
           {
             sprintf(str, "board[%i,%i]:win", x, y);
           }
         else
           {
             if (game.matrix[x][y].mine == 1)
               {
                 sprintf(str, "board[%i,%i]:bomb", x, y);
                 elm_object_signal_emit(game.ui->table, str, "");
               }
             sprintf(str, "board[%i,%i]:lose", x,y);
           }
         elm_object_signal_emit(game.ui->table, str, "");
       }

   /* highlight the fatal bomb */
   if (win == EINA_FALSE)
     {
        sprintf(str, "%s:boom", target);
        elm_object_signal_emit(game.ui->table, str, "");
     }
   else
     {
        /* prepare the congratulation message */
        edje_object_signal_emit(game.ui->edje, "congrat", "");

        score = _scoring(game);
        snprintf(str, sizeof(str), _("Score: %d"), score);
        elm_object_part_text_set(game.ui->table, "congrat:score", str);

        if (score >= etrophy_gamescore_level_hi_score_get(game.trophy.gamescore,
                                                          game.trophy.game_type))
          {
             elm_object_part_text_set(game.ui->table, "congrat:best score",
                                      _("High Score!!"));
          }
        else
          {
             elm_object_part_text_set(game.ui->table, "congrat:best score", "");
          }

        elm_object_signal_emit(game.ui->table, "congrat:you win", "");
        elm_object_part_text_set(game.ui->table, "congrat:you win", _("You win!"));
     }

   if (game.clock.etimer)
     {
        ecore_timer_del(game.clock.etimer);
        game.clock.etimer = 0;
     }
}

struct coordinate
{
  unsigned char x, y;
  coordinate(unsigned char x, unsigned char y)
    : x(x), y(y) {}
};

static void
_clean_walk(game_struct& game, unsigned char x, unsigned char y)
{
   /* we are out of board */
  assert (!(x == 0 || x == game.datas.x_theme + 1
            || y == 0 || y == game.datas.y_theme + 1));

   /* no mine */
   if (game.matrix[x][y].mine == 0)
     {
        int scenery;
        char str[128];

        /* clean scenery */
        game.matrix[x][y].uncover = 1;

        /* add some stones */
        scenery = (int)((double)100 * rand() / RAND_MAX + 1);
        if (scenery < 15)
          {
             sprintf(str, "board[%i,%i]:stones", x, y);
             elm_object_signal_emit(game.ui->table, str, "");
          }

        sprintf(str, "board[%i,%i]:digg", x, y);
	elm_object_signal_emit(game.ui->table, str, "");

        /* at least 1 neighbour */
        if (game.matrix[x][y].neighbours != 0)
          {
            char str[128];
             printf("at least 1 neighbour\n");
             sprintf(str, "board[%i,%i]:%d", x, y, game.matrix[x][y].neighbours);
             elm_object_signal_emit(game.ui->table, str, "");
             game.datas.counter--;
             game.matrix[x][y].uncover = 1;
          }
        /* no neighbour */
        else
          {
            game.datas.counter--;
             printf("no neighbour %i x %i\n", x, y);

             efl::eina::inarray<coordinate> coordinates;

             bool left = (x > 1)
               , right = (x < game.datas.x_theme)
               , top = (y > 1)
               , bottom = (y < game.datas.y_theme)
               ;
             if(left)
                 coordinates.push_back(coordinate(x-1, y));
             if(right)
                 coordinates.push_back(coordinate(x+1, y));
             if(top)
               {
                 if(left)
                   coordinates.push_back(coordinate(x-1, y-1));
                 if(right)
                   coordinates.push_back(coordinate(x+1, y-1));
                 coordinates.push_back(coordinate(x, y-1));
               }
             if(bottom)
               {
                 if(left)
                   coordinates.push_back(coordinate(x-1, y+1));
                 if(right)
                   coordinates.push_back(coordinate(x+1, y+1));
                 coordinates.push_back(coordinate(x, y+1));
               }

             while(!coordinates.empty())
               {
                 coordinate c = coordinates.front();
                 coordinates.erase(coordinates.begin());

                 assert(!game.matrix[c.x][c.y].mine);

                 if(!game.matrix[c.x][c.y].uncover)
                   {
                     game.datas.counter--;
                     scenery = (int)((double)100 * rand() / RAND_MAX + 1);
                     if (scenery < 15)
                       {
                         sprintf(str, "board[%i,%i]:stones", c.x, c.y);
                         elm_object_signal_emit(game.ui->table, str, "");
                       }

                     sprintf(str, "board[%i,%i]:digg", c.x, c.y);
                     elm_object_signal_emit(game.ui->table, str, "");

                     if (game.matrix[c.x][c.y].neighbours != 0)
                       {
                         sprintf(str, "board[%i,%i]:%d", c.x, c.y, game.matrix[c.x][c.y].neighbours);
                         elm_object_signal_emit(game.ui->table, str, "");
                       }

                     game.matrix[c.x][c.y].uncover = 1;

                     if(game.matrix[c.x][c.y].neighbours == 0)
                       {
                         printf("no neighbours %i x %i %i x %i\n", c.x, c.y
                                , game.datas.x_theme, game.datas.y_theme);
                         assert (!(x == 0 || c.x == game.datas.x_theme+1
                                   || y == 0 || c.y == game.datas.y_theme+1));

                         bool left = (c.x > 1)
                           , right = (c.x < game.datas.x_theme)
                           , top = (c.y > 1)
                           , bottom = (c.y < game.datas.y_theme)
                           ;
                         if(left)
                           coordinates.push_back(coordinate(c.x-1, c.y));
                         if(right)
                           coordinates.push_back(coordinate(c.x+1, c.y));
                         if(top)
                           {
                             if(left)
                               coordinates.push_back(coordinate(c.x-1, c.y-1));
                             if(right)
                               coordinates.push_back(coordinate(c.x+1, c.y-1));
                             coordinates.push_back(coordinate(c.x, c.y-1));
                           }
                         if(bottom)
                           {
                             if(left)
                               coordinates.push_back(coordinate(c.x-1, c.y+1));
                             if(right)
                               coordinates.push_back(coordinate(c.x+1, c.y+1));
                             coordinates.push_back(coordinate(c.x, c.y+1));
                           }
                       }
                   }
               }
          }
        /* keep track of this empty spot */
        if (game.datas.counter == 0)
          {
            printf("won\n");
            _finish(game, x, y, EINA_TRUE);
          }
        else
          printf("counter %d\n", (int)game.datas.counter);
     }
   else /* BOOM! */
     {
       _finish(game, x, y, EINA_FALSE);
       printf("BOOM\n");
     }
   return;   
}

void
_click(game_struct& game
       , const char *emission, const char *source)
{
   // /* get back the coordinates of the cell */
   int x, y;
   sscanf(source, "board[%i,%i]:overlay", &x, &y);

   // /* if we push 1st mouse button and there is no flag */
   if (!strcmp(emission, "mouse,clicked,1") && game.matrix[x][y].flag == 0
       && game.matrix[x][y].uncover == 0)
     {
       char str[128];
        if (game.clock.started == EINA_FALSE)
          {
             game.clock.started = EINA_TRUE;
             game.t0 = ecore_time_get();
             static const double dt = 0.1;
             game.clock.etimer
               = ecore_timer_add
               (dt, efl::eina::call(ecore_timer_add)
                , efl::eina::wrap(ecore_timer_add)(std::bind(_timer, std::ref(game))));
          }

        // sprintf(str, "board[%i,%i]", x, y);
       _clean_walk(game, x, y);
     }

   /* second button: put a flag */
   if (!strcmp(emission, "mouse,clicked,3"))
     {
       char str[128];
        if (game.matrix[x][y].uncover != 1)
          {
             if (!game.matrix[x][y].flag) /* set flag */
               {
                  sprintf(str, "board[%i,%i]:flag", x, y);
                  game.datas.remain--;
               }
             else /* already a flag, remove it */
               {
                  sprintf(str, "board[%i,%i]:default", x, y);
                  game.datas.remain++;
               }
             game.matrix[x][y].flag = !game.matrix[x][y].flag;
             elm_object_signal_emit(game.ui->table, str, "");
          }

        /* show the remaining mines */
        snprintf(str, sizeof(str), "%d/%d", game.datas.remain,
                 game.datas.mines_total);

        elm_object_part_text_set(game.ui->mines, "mines", str);
     }

   // /* middle button: open rest if we have enough mines */
   // if (!strcmp(emission, "mouse,clicked,2") && (matrix[x][y].uncover == 1))
   //   {
   //      Elemines_Walker *walker;
   //      Eina_Iterator *it;
   //      int flags = 0;

   //      /* count surrounding flags */
   //      it = _walk(x - 1, y - 1, 3, 3);
   //      EINA_ITERATOR_FOREACH(it, walker)
   //        {
   //           if (walker->x == x && walker->y == y)
   //             continue ;
   //           if (walker->cell->flag == 1)
   //             flags++;
   //        }
   //      eina_iterator_free(it);

   //      /* open surrounding squares if correct number of flags is set */
   //      if (flags == matrix[x][y].neighbours)
   //        {
   //           it = _walk(x - 1, y - 1, 3, 3);
   //           EINA_ITERATOR_FOREACH(it, walker)
   //             _clean_walk(walker->target, walker->x, walker->y);
   //           eina_iterator_free(it);
   //        }
   //   }
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
