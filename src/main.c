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

static void
_debug(void)
{
   int x, y;

   printf("== bomb positions =====\n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y].mine);
          }
        printf("\n");
     }

   printf("== neighbours count ===\n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y].neighbours);
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
   Eina_Bool fullscreen = EINA_FALSE;

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   textdomain(PACKAGE);
#endif

   /* Set default values */
   game.datas.mines_total = MINES;

   /* Get user values */
   while ((opt = getopt(argc, argv, "dfhm:vt:")) != -1)
     {
        switch (opt)
          {
             case 'd':
               {
                  debug = EINA_TRUE;
                  break;
               }
             case 'f':
               {
                  fullscreen = EINA_TRUE;
                  break;
               }
             case 'h':
               {
                  show_help();
                  return EXIT_SUCCESS;
               }
             case 'm':
               {
                  game.datas.mines_total = atoi(optarg);
                  break;
               }
             case 'v':
               {
                  show_version();
                  return EXIT_SUCCESS;
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
               }
             default: return EXIT_FAILURE;
          }
     }

   /* Validate user values */
   if (game.datas.mines_total < 1)
     game.datas.mines_total = 1;
   if (game.datas.mines_total > (SIZE_X * SIZE_Y - 1))
     game.datas.mines_total = SIZE_X * SIZE_Y - 1;

   game.clock.started = EINA_FALSE;

   if (gui(theme, fullscreen) != EINA_TRUE)
     return -1;

   init(NULL, NULL, NULL);
   if (debug == EINA_TRUE) _debug();

   elm_run();
   elm_shutdown();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
