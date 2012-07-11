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

void gui(char *theme);
void show_help(void);
void show_version(void);

static void
_debug(void)
{
   int x, y;

   printf("== bomb positions =====\n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y][0]);
          }
        printf("\n");
     }

   printf("== neighbours count ===\n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y][1]);
          }
        printf("\n");
     }
}

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   int opt;
   char *theme = "default";
   Eina_Bool debug = EINA_FALSE;

   mines_total = 0;

   while ((opt = getopt(argc, argv, "dhm:vt:")) != -1)
     {
        switch (opt)
          {
             case 'd':
               {
                  debug = EINA_TRUE;
                  break;
               }
             case 'h':
               {
                  show_help();
                  return EXIT_SUCCESS;
                  break;
               }
             case 'm':
               {
                  mines_total = atoi(optarg);
                  break;
               }
             case 'v':
               {
                  show_version();
                  return EXIT_SUCCESS;
                  break;
               }
             case 't':
               {
                  theme = optarg;
                  break;
               }
             case '?':
               {
                  show_help();
                  return EXIT_SUCCESS;
                  break;
               }
             default: return EXIT_FAILURE;
          }
     }

   /* initialize the mines number */
   if (mines_total == 0) mines_total = MINES;

   started = EINA_FALSE;
   gui(theme);
   init(NULL, NULL, NULL);

   if (debug == EINA_TRUE)
     _debug();

   elm_run();
   elm_shutdown();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
