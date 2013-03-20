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

#include <Ecore_Getopt.h>

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

static const Ecore_Getopt optdesc = {
  "elemines",
  "%prog [options]",
  PACKAGE_VERSION,
  "(C) 2010 Enlightenment",
  "BSD with advertisement clause",
  "Simple application to view edje files.",
  0,
  {
    ECORE_GETOPT_STORE_TRUE('d', "debug", "turn on debugging"),
    ECORE_GETOPT_STORE_TRUE('f', "fullscreen", "make the application fullscreen"),
    ECORE_GETOPT_STORE_STR('t', "theme", "change theme"),
    ECORE_GETOPT_STORE_INT('m', "mines", "define the number of mines on the grid"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   char *theme = "default";
   int args;
   Eina_Bool debug = EINA_FALSE;
   Eina_Bool fullscreen = EINA_FALSE;
   Eina_Bool quit_option = EINA_FALSE;
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(debug),
     ECORE_GETOPT_VALUE_BOOL(fullscreen),
     ECORE_GETOPT_VALUE_STR(theme),
     ECORE_GETOPT_VALUE_INT(game.datas.mines_total),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   textdomain(PACKAGE);
#endif

   /* Set default values */
   game.datas.mines_total = MINES;

   /* Get user values */
   args = ecore_getopt_parse(&optdesc, values, argc, argv);
   if (args < 0)
     {
       fputs("Could not parse arguments.\n", stderr);
       goto end;
     }
   else if (quit_option)
     {
       goto end;
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

 end:
   elm_shutdown();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
