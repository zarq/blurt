#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "screen.h"

Display *Xdisplay;
Window MainWindow;
Font Xfont;
GC Xgc;

int colors[3];

int screen_width = 200/8;
int screen_height = 100/14;

int draw_ptr_x = 0;
int draw_ptr_y = 10;

int init_display(void)
{
  Xdisplay = XOpenDisplay(NULL);
  if(!Xdisplay)
    {
      fprintf(stderr, "Couldn't open display\n");
      return -1;
    }

  return 0;
}

int init_window(void)
{
  MainWindow = XCreateSimpleWindow(Xdisplay, DefaultRootWindow(Xdisplay), 0, 0, 
				   200, 100, 0, DEFAULT_BACKGROUND_COLOR, DEFAULT_BACKGROUND_COLOR);

  XSelectInput(Xdisplay, MainWindow, StructureNotifyMask);
  XMapWindow(Xdisplay, MainWindow);

  /* Wait for the window to be mapped */
  wait_for_specific_event(MapNotify);

  Xgc = XCreateGC(Xdisplay, MainWindow, 0, NULL);
  XSetForeground(Xdisplay, Xgc, DEFAULT_FOREGROUND_COLOR);
  XSetBackground(Xdisplay, Xgc, DEFAULT_BACKGROUND_COLOR);

  return 0;
}

int init_font(void)
{
  Xfont = XLoadFont(Xdisplay, DEFAULT_FONT);
  if(!Xfont)
    {
      fprintf(stderr, "Couldn't open font `%s'\n",
	      DEFAULT_FONT);
      return -1;
    }
  return 0;
}

void wait_for_specific_event(int event_type)
{
  XEvent e;
  
  for(;;)
    {
      XNextEvent(Xdisplay, &e);
      if(e.type == event_type)
	break;
    }
}

int init_colors(void)
{
  DEFAULT_BACKGROUND_COLOR = BlackPixel(Xdisplay, DefaultScreen(Xdisplay));
  DEFAULT_FOREGROUND_COLOR = WhitePixel(Xdisplay, DefaultScreen(Xdisplay));
  return 0;
}

int write_text(const char *buf, int len)
{
  XSetFont(Xdisplay, Xgc, Xfont);

  XDrawImageString(Xdisplay, MainWindow, Xgc, draw_ptr_x, draw_ptr_y, buf ,len);
  XFlush(Xdisplay);
  draw_ptr_x += len * 8;
  return 0;
}

int exit_display(void)
{
  XCloseDisplay(Xdisplay);
  return 0;
}

int get_x_filedes(void)
{
  return XConnectionNumber(Xdisplay);
}

int handle_x_events(void)
{
  XEvent xev;
  char kbuf[4];
  KeySym keysym;
  static XComposeStatus compose = {NULL, 0};
  int len;

  /* Input from the X server */
  while (XPending(Xdisplay))
    {
      /* process pending X events */
      XNextEvent(Xdisplay, &xev);
      fprintf(stderr, "Got X event %d\n", xev.type);

      switch(xev.type)
	{
	case KeyPress:
	  len = XLookupString(&xev.xkey, &kbuf, sizeof(kbuf), &keysym, &compose);
	  kbuf[3] = 0;
	  fprintf(stderr, "key %s (%d)\n", kbuf, len);
	  cmd_write(kbuf, len);
	  break;
	}
    }
}

int set_x_event_mask(void)
{
  XSelectInput(Xdisplay, MainWindow, KeyPressMask | ButtonPressMask);
}
