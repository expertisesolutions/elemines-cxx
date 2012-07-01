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

void init(void);
void gui(char *theme);
void show_help(void);
void show_version(void);

EAPI_MAIN int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   int opt, x, y;
   char *theme = "default";

   while ((opt = getopt(argc, argv, "hvt:")) != -1)
     {
        switch (opt)
          {
             case 'h':
               {
                  show_help();
                  return EXIT_SUCCESS;
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

   init();
   started = EINA_FALSE;
   gui(theme);

   /* print this out */
   printf(" ===================== \n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y][0]);
          }
        printf("\n");
     }
   printf(" ===================== \n");
   for (y = 0; y < SIZE_Y+2; y++)
     {
        for (x = 0; x < SIZE_X+2; x++)
          {
             printf("%d ", matrix[x][y][1]);
          }
        printf("\n");
     }
   printf(" ===================== \n");

   elm_run();
   elm_shutdown();

   return 0;
}
ELM_MAIN()

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
