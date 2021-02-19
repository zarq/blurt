#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include <X11/Xlib.h>


void main_loop()
{
  int len;
  char buf[100];
  fd_set fset;
  struct timeval tv;
  int r;
  int cmd_fd;
  int X_fd;

  X_fd = get_x_filedes();
  cmd_fd = get_cmd_filedes();

  fprintf(stderr, "x = %d, cmd = %d\n", X_fd, cmd_fd);

  for(;;)
    {
      FD_ZERO(&fset);
      FD_SET(cmd_fd, &fset);
      FD_SET(X_fd, &fset);

      tv.tv_usec = 0;
      tv.tv_sec = 2;

      r = select(FD_SETSIZE, &fset, NULL, NULL, &tv);

      if(r < 0)
	{
	  /* error */
	  fprintf(stderr, "select: %m\n");
	  exit(1);
	}

      if(!r)
	{
	  /* timeout */
	  continue;
	}

      if(FD_ISSET(cmd_fd, &fset))
	{
	  /* Input from the command */
	  len =	read(cmd_fd, buf, 100);
	  write_text(buf, len);
	}

      if(FD_ISSET(X_fd, &fset))
	{
	  handle_x_events();
	}
    }
}
