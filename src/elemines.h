/*
 * elemines: an EFL mine sweeper
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

#include "../config.h"

#define COPYRIGHT "Copyright © 2012  Jérôme Pinot <ngc891@gmail.com>"

#define SIZE_X  10
#define SIZE_Y  10
#define MINES   10

/* 4 layers for the matrix of data:
 * 1st -> mines (0/1)
 * 2nd -> neighbours (0-8, 9 for bombs)
 * 3rd -> flags (0/1)
 * 4th ->uncover status (0/1)
 */
int matrix[SIZE_X+2][SIZE_Y+2][4];

/* We can't access easily elm table cell content, so keep
 * a table of pointer to edje layouts */
Evas_Object *table_ptr[SIZE_X+2][SIZE_Y+2];

/* the main elm window */
Evas_Object *window, *table, *timer, *mines;
int remain, counter;
char edje_file[PATH_MAX];
Eina_Bool started;
Ecore_Timer *etimer;
double delay;

void init(void *data, Evas_Object *obj, void *event_info);
void click(void *data, __UNUSED__ Evas *e, Evas_Object *obj, void *event_info);

/* vim: set ts=8 sw=3 sts=3 expandtab cino=>5n-3f0^-2{2(0W1st0 : */
