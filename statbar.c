/*  The little status bar at the bottom of the screen   */

#include <time.h>

//void statbar(WINDOW **localwin)
void statbar(void)
{
    time_t curtime;
    char *timestring;


    curtime = time(NULL);
    timestring = ctime(&curtime);

    wmove(statbarwin, 0, 0);
    wclrtoeol(statbarwin);
    mvwprintw(statbarwin, 0, 0, "%s", timestring);
    wrefresh(statbarwin);
}
