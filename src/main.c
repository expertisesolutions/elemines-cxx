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

void
click(void *data, __UNUSED__ Evas *e, __UNUSED__ Evas_Object *obj, void *event_info)
{
   int coord[2] = { 0, 0 };
   Evas_Event_Mouse_Down *ev = event_info;

   /* get back the coordinates of the cell */
   memcpy(&coord, &data, sizeof(data));

   if (ev->button == 1)
     printf("%d %d\n", coord[0], coord[1]);
}

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{

   Evas_Object *window, *background, *table, *cell, *blank;
   char edje_file[PATH_MAX];
   int i, j, x, y;
   int coord[2] = { 0, 0 };
   void *data = NULL;
   int matrix[SIZE_X+2][SIZE_Y+2][3];
   char *theme = "default";

   /* set general properties */
   window = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
   elm_win_title_set(window, PACKAGE);
   elm_win_autodel_set(window, EINA_TRUE);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   /* add a background */
   background = elm_bg_add(window);
   evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(window, background);
   evas_object_show(background);

   /* add the main table for storing cells */
   table = elm_table_add(window);
   elm_table_homogeneous_set(table, EINA_TRUE);
   elm_win_resize_object_add(window, table);
   evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(table);

   /* get the edje theme file */
   snprintf(edje_file, sizeof(edje_file), "%s/themes/%s.edj", PACKAGE_DATA_DIR, theme);
   if (access(edje_file, R_OK) != 0)
     {
        EINA_LOG_CRIT("Loading theme error: can not read %s", edje_file);
        return EXIT_FAILURE;
     }

   /* empty the matrix */
   memset(matrix, 0, sizeof(matrix));

   /* 1st table: the mines */
   srand(time(NULL));
   for (i = 0; i < MINES; i++)
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
             for (i=-1; i<2; i++)
               {
                  for (j=-1; j<2; j++)
                    {
                       if (!(j == 0 && i == 0))
                         matrix[x][y][1] += matrix[x+i][y+j][0];
                    }
               }
              /* mark a mine place with 9 */
               if ( matrix[x][y][0] == 1)
                 matrix[x][y][1] = 9;

               /* good timing for preparing the board */
               cell = elm_layout_add(window);
               elm_layout_file_set(cell, edje_file, "cell");
               evas_object_size_hint_weight_set(cell, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
               evas_object_size_hint_align_set(cell, EVAS_HINT_FILL, EVAS_HINT_FILL);
               elm_table_pack(table, cell, x, y, 1, 1);
               evas_object_show(cell);

               /* we need to feed the callback with coordinates */
               coord[0] = x;
               coord[1] = y;
               memcpy(&data, &coord, sizeof(coord));
               evas_object_event_callback_add(cell, EVAS_CALLBACK_MOUSE_DOWN, click, data);
          }
     }
    /* Add a blank cell on the bottom right to get a right/bottom margin */
    blank = elm_layout_add(window);
    elm_layout_file_set(blank, edje_file, "blank");
    evas_object_size_hint_weight_set(blank, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(blank, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_table_pack(table, blank, SIZE_X+1, SIZE_Y+1, 1, 1);
    evas_object_show(blank);

   /* print this out */
   for (x = 0; x < SIZE_X+2; x++)
     {
        for (y = 0; y < SIZE_Y+2; y++)
          {
             printf("%d ", matrix[x][y][0]);
          }
        printf("\n");
     }
        printf(" ===================== \n");
   for (x = 0; x < SIZE_X+2; x++)
     {
        for (y = 0; y < SIZE_Y+2; y++)
          {
             printf("%d ", matrix[x][y][1]);
          }
        printf("\n");
     }

   /* Get window's size from edje and resize it */
   x = atoi(edje_file_data_get(edje_file, "width"));
   y = atoi(edje_file_data_get(edje_file, "height"));

   evas_object_resize(window, x, y);
   elm_object_focus_set(window, EINA_TRUE);
   evas_object_show(window);

   elm_run();
   elm_shutdown();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
