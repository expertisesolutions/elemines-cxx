
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Evas.h>
#include <Ecore_Getopt.h>
#include <Edje.h>
#include <Elementary.h>
#include <Etrophy.h>

#define COPYRIGHT "Copyright © 2012-2013 Jérôme Pinot <ngc891@gmail.com> and various contributors (see AUTHORS)."

Ecore_Getopt optdesc = {
  "elemines",
  "%prog [options]",
  PACKAGE_VERSION,
  COPYRIGHT,
  "BSD with advertisement clause",
  "An EFL minesweeper clone",
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

