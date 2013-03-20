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

static Eina_Bool
_generate(void)
{
   int i, j, x, y;

   /* empty the matrix */
   memset(matrix, 0, sizeof(matrix));

   /* 1st table: the mines */
   srand(time(NULL));
   for (i = 0; i < game.datas.mines_total; i++)
     {
        /* random coordinates */
        x = (int)((double)SIZE_X * rand() / RAND_MAX + 1);
        y = (int)((double)SIZE_Y * rand() / RAND_MAX + 1);

        if ( matrix[x][y].mine == 0 )
          {
             matrix[x][y].mine = 1;
          }
        else /* if there is already a bomb here, try again */
          {
             i--;
          }
     }

   /* 2nd table: neighbours */
   for (x = 1; x < SIZE_X+1; x++)
     {
        for (y = 1; y < SIZE_Y+1; y++)
          {
             /* count neighbours */
             for (i = -1; i < 2; i++)
               {
                  for (j = -1; j < 2; j++)
                    {
                       if (!((j == 0) && (i == 0)))
                         matrix[x][y].neighbours += matrix[x+i][y+j].mine;
                    }
               }
              /* mark a mine place with a 9 */
               if (matrix[x][y].mine == 1) matrix[x][y].neighbours = 9;
          }
     }
   return EINA_TRUE;
}

static Eina_Bool
_board(void)
{
   Evas_Object *cell;
   Evas_Object *edje;
   int x, y, scenery;

   edje = elm_layout_edje_get(game.ui.table);
   edje_object_signal_callback_del_full(edje, "mouse,clicked,*", "board\\[*\\]:overlay", _click, NULL);
   edje_object_signal_callback_add(edje, "mouse,clicked,*", "board\\[*\\]:overlay", _click, NULL);
   edje_object_signal_emit(edje, "reset", "");

   /* prepare the board */
   for (x = 1; x < SIZE_X+1; x++)
     {
        for (y = 1; y < SIZE_Y+1; y++)
          {
	     /* remove any existing cell */
             cell = edje_object_part_table_child_get(edje, "board", x, y);

             /* add some random scenery */
             scenery = (int)((double)100 * rand() / RAND_MAX + 1);
             edje_object_signal_emit(cell, "reset", "");
             if (scenery < 15)
               edje_object_signal_emit(cell, "flowers", "");
             if ((scenery > 12) && (scenery < 18))
               edje_object_signal_emit(cell, "mushrooms", "");
 
             matrix[x][y].layout = cell;
          }
     }
   return EINA_TRUE;
}

void
init(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
     void *event_info __UNUSED__)
{
   char str[8];

   /* init variables */
   game.clock.started = EINA_FALSE;
   game.clock.delay = 0;
   game.datas.remain = game.datas.mines_total;
   game.datas.counter = SIZE_X * SIZE_Y - game.datas.mines_total;

   _generate();
   _board();

   /* reinit widgets if needed */
   if (game.ui.timer)
     elm_object_part_text_set(game.ui.timer, "time", "00:00.0");
   snprintf(str, sizeof(str), "%d/%d", game.datas.mines_total,
            game.datas.mines_total);
   if (game.ui.mines)
     elm_object_part_text_set(game.ui.mines, "mines", str);

}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
