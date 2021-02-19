/* $Id: main.c,v 1.5 2001/02/16 23:30:02 zarq Exp $ */

#include <X11/Xlib.h>

#include <stdio.h>

#include "process.h"

int main(int argc, char **argv)
{
  if(init_display())
    {fprintf(stderr, "%d\n", __LINE__);
    return 1;
    }
  if(init_colors())
    {fprintf(stderr, "%d\n", __LINE__);
    return 1;
    }
  
  if(init_window())
    {fprintf(stderr, "%d\n", __LINE__);
    return 1;
    }

  if(init_font())
    {fprintf(stderr, "%d\n", __LINE__);
    return 1;
    }

  set_x_event_mask();
  execute_command(argc, argv);

  main_loop();

  exit_display();
      
  return 0;
}
