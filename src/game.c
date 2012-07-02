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

double t0;
double dt = 0.1;

static Eina_Bool
_timer(void *data __UNUSED__)
{
   char str[8] = { 0 };
   int min = 0;
   double t;

   if (started == EINA_FALSE)
     return EINA_FALSE;

   t = ecore_loop_time_get() - t0;
   while (t >= 60)
     {
        t -= 60;
        min++;
     }

   snprintf(str, sizeof(str), "%02d:%04.1f", min, t);
   if (counter != 0)
     elm_object_part_text_set(timer, "time", str);

   return EINA_TRUE;
}

static void
game_win(Evas_Object *obj)
{
   started = EINA_FALSE;
   elm_object_signal_emit(obj, "fanfare", "");
   printf("You win!\n");
   if (etimer)
     ecore_timer_del(etimer);
}

static void
game_over(int x, int y)
{
   int i,j;

   started = EINA_FALSE;
   /* show bombs */
   for (i = 1; i < SIZE_X+1; i++)
     {
        for (j = 1; j < SIZE_Y+1; j++)
          {
             if (matrix[i][j][0] == 1)
               elm_object_signal_emit(table_ptr[i][j], "bomb", "");
          }
     }

   /* highlight the fatal bomb */
   elm_object_signal_emit(table_ptr[x][y], "boom", "");
   printf("You lose.\n");
   if (etimer)
     ecore_timer_del(etimer);
}

static void
clean_around(int x, int y, Evas_Object *obj)
{
   int i, j, scenery;
   char str[8];

   /* we are out of board */
   if (x == 0 || x == SIZE_X+1 || y == 0 || y == SIZE_Y+1)
     return;

   /* nothing here and not already uncovered */
   if (matrix[x][y][0] == 0 && matrix[x][y][3] == 0)
     {
        /* clean scenery */
        elm_object_signal_emit(obj, "noflowers", "");
        elm_object_signal_emit(obj, "nomushrooms", "");
        elm_object_signal_emit(obj, "digging", "");
        elm_object_signal_emit(obj, "clean", "");
        /* add some stones */
        scenery = (int)((double)100 * rand() / RAND_MAX + 1);
        if (scenery < 15)
          elm_object_signal_emit(obj, "stones", "");
        matrix[x][y][3] = 1;
        /* at least 1 neighbour */
        if (matrix[x][y][1] != 0)
          {
             snprintf(str, sizeof(str), "%d", matrix[x][y][1]);
             elm_object_signal_emit(obj, str, "");
             elm_object_part_text_set(obj, "hint", str);
          }
        /* no neighbour */
        else
          {
             for (i=x-1; i<=x+1; i++)
               {
                  for (j=y-1; j<=y+1; j++)
                    {
                       if (!(i == x && j == y))
                         {
                            clean_around(i, j, table_ptr[i][j]);
                         }
                    }
               }
          }
        /* keep track of this empty spot */
        counter--;
        if (counter == 0)
          game_win(obj);
     }
   return;

}

void
click(void *data, __UNUSED__ Evas *e, Evas_Object *obj, void *event_info)
{
   int coord[2] = { 0, 0 };
   int x, y;
   char str[8] = { 0 };
   Evas_Event_Mouse_Down *ev = event_info;

   /* get back the coordinates of the cell */
   memcpy(&coord, &data, sizeof(data));

   x = coord[0];
   y = coord[1];

   /* if we push 1st mouse button and there is no flag */
   if (ev->button == 1 && matrix[x][y][2] == 0)
     {
        if (started == EINA_FALSE)
          {
             started = EINA_TRUE;
             t0 = ecore_time_get();
             etimer = ecore_timer_add(dt, _timer, NULL);
          }

        /* OMG IT'S A BOMB! */
        if (matrix[x][y][0] == 1)
          {
             game_over(x, y);
             return;
          }
        else
          {
             clean_around(x, y, obj);
          }
     }

   /* second button: put a flag */
   if (ev->button == 3)
     {
        /* there was no flag and we didn't digg */
        if (matrix[x][y][2] == 0 && matrix[x][y][3] != 1)
          {
             elm_object_signal_emit(obj, "flag", "");
             matrix[x][y][2] = 1;
             remain--;
          }
        /* already a flag, remove it */
        else
          {
             elm_object_signal_emit(obj, "default", "");
             matrix[x][y][2] = 0;
             remain++;
          }

        /* show the remaining mines */
        if (remain >= 0 && remain <= MINES)
          {
             snprintf(str, sizeof(str), "%03d", remain);
          }
        else if (remain < 0)
          {
             snprintf(str, sizeof(str), "%03d", 0);
          }
        else if (remain > MINES)
          {
             snprintf(str, sizeof(str), "%03d", MINES);
          }

         elm_object_part_text_set(mines, "mines", str);
     }
}

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
