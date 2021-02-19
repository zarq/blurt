#include <assert.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

pid_t pid;

static int cmd_fd;
static int slave;

struct passwd *pw;

char *ptydev, *ttydev;

void sigchld_handler(int a)
{
  fprintf(stderr, "Child killed\n");
  exit(0);
}

void find_user(void)
{
  uid_t uid;

  if((uid = getuid()) == -1)
    {
      fprintf(stderr, "Couldn't find current uid: %m\n");
      exit(2);
    }

  if((pw = getpwuid(uid)) == NULL)
    {
      fprintf(stderr, "Couldn't find password entry for user %d: %m\n",
	      uid);
      exit(2);
    }
}

int get_pty(void)
{
  extern char *ptsname();
  int fd;

  if((fd = getpt()) >= 0)
    {
      if(grantpt(fd) == 0 && unlockpt(fd) == 0)
	{
	  ptydev = ptsname(fd);
	  if((slave = open(ptydev, O_RDWR | O_NOCTTY)) < 0)
	    {
	      fprintf(stderr, "Error opening slave pty: %m\n");
	      return -1;
	    }
	  fcntl(fd, F_SETFL, O_NDELAY);
	  fprintf(stderr, "Opened pty %s\n", ptydev);
	  return fd;
	}
      close(fd);
    }
  fprintf(stderr, "Can't open a pseudo-tty\n");
  return -1;
}

int get_tty(void)
{
  int i;
  
  for(i = 0; i < 100 ; i++)
    if(i != slave)
      close(i);

  cmd_fd = slave;

  setsid(); /* create a new process group */

  if(ioctl(cmd_fd, TIOCSCTTY, NULL) < 0)
    {
      fprintf(stderr, "Couldn't set controlling terminal: %m\n");
      return -1;
    }
  
  dup2(cmd_fd, 0);
  dup2(cmd_fd, 1);
  dup2(cmd_fd, 2);

  return 0;
}

int execute_command(int argc, const char **argv)
{
  int e;
  char **args;

  find_user();

  if((cmd_fd = get_pty()) < 0)
    return -1;

  if((pid = fork()) < 0)
    {
      fprintf(stderr, "Couldn't fork: %m\n");
      return -1;
    }

  if(!pid)
    {
      /* child */
      get_tty();

      args = calloc(3, sizeof(char*));
      args[0] = malloc(strlen(pw->pw_shell) + 1);
      strcpy(args[0], pw->pw_shell);
      args[1] = "-i";
      args[2] = NULL;
      fprintf(stderr, "Yay!\n");
      
      execvp(pw->pw_shell, args);

      /* shouldn't be here */
      fprintf(stderr, "Error executing %s: %m\n", pw->pw_shell);
      exit(1);
    }

  /* parent */
  close(slave);
  signal(SIGCHLD, sigchld_handler);

  return 0;
}

int get_cmd_filedes(void)
{
  return cmd_fd;
}

int cmd_write(const char *buf, int len)
{
  if(write(cmd_fd, buf, len) < 0)
    {
      fprintf(stderr, "Error writing to process: %m\n");
      exit(2);
    }

  return 0;
}
