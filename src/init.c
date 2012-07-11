/*
 * elemines: an EFL minesweeper
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

static Eina_Bool
_generate(void)
{
   int i, j, x, y;

   /* empty the matrix */
   memset(matrix, 0, sizeof(matrix));

   /* 1st table: the mines */
   srand(time(NULL));
   for (i = 0; i < mines_total; i++)
     {
        /* random coordinates */
        x = (int)((double)SIZE_X * rand() / RAND_MAX + 1);
        y = (int)((double)SIZE_Y * rand() / RAND_MAX + 1);

        if ( matrix[x][y][0] == 0 )
          {
             matrix[x][y][0] = 1;
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
                         matrix[x][y][1] += matrix[x+i][y+j][0];
                    }
               }
              /* mark a mine place with a 9 */
               if (matrix[x][y][0] == 1) matrix[x][y][1] = 9;
          }
     }
   return EINA_TRUE;
}

static Eina_Bool
_board(void)
{
   Evas_Object *cell;
   int x, y, scenery;
   int coord[2];
   void *data;

   /* prepare the board */
   for (x = 1; x < SIZE_X+1; x++)
     {
        for (y = 1; y < SIZE_Y+1; y++)
          {
             cell = elm_layout_add(window);
             elm_layout_file_set(cell, edje_file, "cell");
             evas_object_size_hint_weight_set(cell, EVAS_HINT_EXPAND,
                                                    EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(cell, EVAS_HINT_FILL,
                                                   EVAS_HINT_FILL);
             elm_table_pack(table, cell, x, y, 1, 1);

             /* add some random scenery */
             scenery = (int)((double)100 * rand() / RAND_MAX + 1);
             if (scenery < 15)
               elm_object_signal_emit(cell, "flowers", "");
             if ((scenery > 12) && (scenery < 18))
               elm_object_signal_emit(cell, "mushrooms", "");
 
             table_ptr[x][y] = cell;
             evas_object_show(cell);

             /* we need to feed the callback with coordinates */
             coord[0] = x;
             coord[1] = y;
             memcpy(&data, &coord, sizeof(coord));
             evas_object_event_callback_add(cell, EVAS_CALLBACK_MOUSE_DOWN,
                                            click, data);
          }
     }
   return EINA_TRUE;
}

void
init(__UNUSED__ void *data, __UNUSED__ Evas_Object *obj,
     __UNUSED__ void *event_info)
{
   char str[8];

   /* init variables */
   started = EINA_FALSE;
   remain = mines_total;
   counter = SIZE_X * SIZE_Y - mines_total;
   delay = 0;

   _generate();
   _board();

   /* set initial value for timer and mines */
   if (timer)
     elm_object_part_text_set(timer, "time", "00:00.0");
   snprintf(str, sizeof(str), "%d/%d", mines_total, mines_total);
   if (mines)
     elm_object_part_text_set(mines, "mines", str);

}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
