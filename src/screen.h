#define DEFAULT_FONT "fixed"

extern int colors[3];
#define DEFAULT_BACKGROUND_COLOR colors[0]
#define DEFAULT_FOREGROUND_COLOR colors[1]
#define DEFAULT_CURSOR_COLOR colors[2]


extern int init_screen(void);
extern int init_window(void);
extern void wait_for_specific_event(int);
