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

#ifndef ELEMINES_H
#define ELEMINES_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <Eina.h>
#include <Evas.h>
#include <Edje.h>
#include <Elementary.h>
#include <Elementary_Cursor.h>
#include <Etrophy.h>

#define COPYRIGHT "Copyright © 2012-2013  Jérôme Pinot <ngc891@gmail.com>"

#define SIZE_X  10
#define SIZE_Y  10
#define MINES   12

/* 4 layers for the matrix of data:
 * 1st -> mines (0/1)
 * 2nd -> neighbours (0-8, 9 for bombs)
 * 3rd -> flags (0/1)
 * 4th -> uncover status (0/1)
 */
int matrix[SIZE_X+2][SIZE_Y+2][4];

/* We can't access easily elm table cell content, so keep
 * a table of pointer to edje layouts */
Evas_Object *table_ptr[SIZE_X+2][SIZE_Y+2];

/* global variables */
struct ui_struct {
   Evas_Object *window;
   Evas_Object *table;
   Evas_Object *timer;
   Evas_Object *mines;
   Evas_Object *congrat;
};

struct datas_struct {
   int mines_total;
   int remain;
   int counter;
};

struct clock_struct {
   Eina_Bool started;
   Ecore_Timer *etimer;
   double delay;
};

struct trophy_struct {
   Etrophy_Gamescore *gamescore;
   Etrophy_Level *level;
   Etrophy_Score *escore;
};

struct game_struct {
   char edje_file[PATH_MAX];
   struct ui_struct ui;
   struct datas_struct datas;
   struct clock_struct clock;
   struct trophy_struct trophy;
};
struct game_struct game;


void show_help(void);
void show_version(void);
void init(void *data, Evas_Object *obj, void *event_info);
Eina_Bool gui(char *theme, Eina_Bool fullscreen);
void click(void *data, Evas *e __UNUSED__, Evas_Object *obj, void *event_info);

#endif

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
