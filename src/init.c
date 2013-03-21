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

struct cell_struct matrix[SIZE_X+2][SIZE_Y+2];

static void
_mine_count(const char *target EINA_UNUSED, unsigned char x, unsigned char y, void *data)
{
   int *count = data;

   if (matrix[x][y].mine)
     (*count)++;
}

static void
_update_mine_count(const char *target EINA_UNUSED, unsigned char x, unsigned char y, void *data EINA_UNUSED)
{
   int neighbours = 9;

   /* mark a mine place with a 9 */
   if (!matrix[x][y].mine)
     {
        neighbours = 0;
        _walk(x - 1, y - 1, 3, 3, _mine_count, &neighbours);
     }

   matrix[x][y].neighbours = neighbours;
}

static Eina_Bool
_generate(void)
{
   int i, x, y;

   /* empty the matrix */
   memset(matrix, 0, sizeof(matrix));

   /* 1st table: the mines */
   srand(time(NULL));
   for (i = 0; i < game.datas.mines_total; i++)
     {
        /* random coordinates */
        x = (int)((double)SIZE_X * rand() / RAND_MAX + 1);
        y = (int)((double)SIZE_Y * rand() / RAND_MAX + 1);

        if (matrix[x][y].mine == 0 )
          matrix[x][y].mine = 1;
        else /* if there is already a bomb here, try again */
          i--;
     }

   /* 2nd table: neighbours */
   _walk(1, 1, SIZE_X, SIZE_Y, _update_mine_count, NULL);
   return EINA_TRUE;
}

static void
_reset(const char *target, unsigned char x EINA_UNUSED, unsigned char y EINA_UNUSED, void *data EINA_UNUSED)
{
   char tmp[128];
   int scenery;

   sprintf(tmp, "%s:reset", target);
   elm_object_signal_emit(game.ui.table, tmp, "");

   /* add some random scenery */
   scenery = (int)((double)100 * rand() / RAND_MAX + 1);
   if (scenery < 15)
     {
        sprintf(tmp, "%s:flowers", target);
        elm_object_signal_emit(game.ui.table, tmp, "");
     }
   if ((scenery > 12) && (scenery < 18))
     {
        sprintf(tmp, "%s:mushrooms", target);
        elm_object_signal_emit(game.ui.table, tmp, "");
     }
}

static Eina_Bool
_board(void)
{
   Evas_Object *edje;

   edje = elm_layout_edje_get(game.ui.table);
   elm_object_signal_emit(game.ui.table, "reset", "");

   edje_object_signal_callback_del_full(edje, "mouse,clicked,*", "board\\[*\\]:overlay", _click, NULL);
   edje_object_signal_callback_add(edje, "mouse,clicked,*", "board\\[*\\]:overlay", _click, NULL);

   /* prepare the board */
   _walk(1, 1, SIZE_X, SIZE_Y, _reset, NULL);
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

   memset(matrix, 0, sizeof (matrix));

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
