#include <menu.h>
#include <stdlib.h>
//#include "inttostr.c"

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#define MENUS           8

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
//#define ANSI_COLOR_BLUE    "\033[22;34m"
//#define ANSI_COLOR_BLUE   "^[[;34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

WINDOW *menuwin;
WINDOW *welcomewin;
WINDOW *statbarwin;
WINDOW *monitorwin;

int running = 1;

//struct type for menu entries
typedef struct {
    char *name;
    char *desc;
    char *cmds[2];
} Entry;

struct Menus {
    MENU **menu;
    ITEM ***items;
    Entry *list;
    int len;
    char *title;
};

//Function declarations
int     execute(char **cmds);
WINDOW *createwin(int height, int width, int starty, int startx, char *title, int color);
MENU   *initmenu(ITEM ***items, Entry list[], int n);
void    drawwin(WINDOW *win, char *title, int color);
//void statbar(WINDOW **localwin);
void statbar(void);
//void *statbar(void *localwin);
//void statbar(WINDOW **localwin);
char *inttostr(int n);

// WINDOW *menuwin;
// WINDOW *welcomewin;
// WINDOW *statbarwin;

//Constants for menu options
enum { MAIN=0, ENV, BROWS, GAMES, MEDIA, MISC, SYS, EXIT, QUIT=666, DOS, DOOM };


char *title[] =     { "Main Menu",                      //MAIN
                      "Desktop Environments",           //ENV
                      "Web Browsers",                   //BROWS
                      "Games",                          //GAMES
                      "Media Applications",             //MEDIA
                      "Miscellaneous Applications",     //MISC
                      "System Applications",            //SYS
                      "Leave",                          //EXIT
                     };


Entry mainlist[] = {
                    {"Desktop Environments", "-->", {"1"}},
                    {"Web Browsers", "-->", {"2"}},
                    {"Games", "-->", {"3"}},
                    {"Media", "-->", {"4"}},
                    {"Miscellaneous", "-->", {"5"}},
                    {"System", "-->", {"6"}},
                    {"TempleOS", "God's Perfect OS", {"startx /usr/bin/ratpoison -f ~/.rptempleosrc -- -dpi 192"}},
                    {"Leave", "-->", {"7"}},
                    {" ", "", {""}},
                    {(char *)NULL, (char *)NULL, {(char *)NULL}},
                   };

Entry envlist[] = {
                   {"Plasma KDE",
                    "State-of-the-art and shiny",
                                   {"gsettings set org.gnome.desktop.interface gtk-theme \"Breeze-Dark\" &",
                                    "startx /usr/bin/startkde -- -dpi 192"}},
                   {"GNOME 3",
                    "Ol' Reliable",
                                   {"gsettings set org.gnome.desktop.interface gtk-theme \"Ambiant-MATE\" &",
                                    "startx /usr/bin/gnome-session -- -dpi 192"}},
                   {"OB Electric",
                    "Openbox with XFCE",
                                   {"startx /usr/bin/openbox-session -- -dpi 192"}},
                   {"OB Dark",
                    "Dark Openbox config",
//                                   {"cp ~/.config/openbox/autostarts/autostart-dark ~/.config/openbox/autostart",
                                    {"startx /usr/bin/openbox --startup ~/.config/openbox/autostart-dark --config-file ~/.config/openbox/rc-dark.xml -- -dpi 192"}},
//                                    "startx /usr/bin/openbox-session -- -dpi 192"}},
                   {"OB Ice",
                    "Icy Openbox config",
//                                   {"cp ~/.config/openbox/autostarts/autostart-lite ~/.config/openbox/autostart",
                                    {"startx /usr/bin/openbox --startup ~/.config/openbox/autostart-lite --config-file ~/.config/openbox/rc-lite.xml -- -dpi 192"}},
                   {"i3",
                    "Tiled window manager",
                                   {"startx /usr/bin/i3 -- -dpi 192"}},
                   {"RatPoison",
                    "Bare bones as it gets",
                                   {"startx /usr/bin/ratpoison -- -dpi 192"}},
                   {"<--",
                    "Main Menu",
                                   {"0"}},
                   {(char *)NULL, (char *)NULL, {(char *)NULL}},
                  };

Entry browslist[] = {
                        {"Chromium",
                         "Open-source version of Chrome",
                                   {"startx /usr/bin/chromium -- -dpi 192"}},
                        {"YouTube",
                         "YouTube kiosk",
                                   {"startx /usr/bin/chromium www.youtube.com --kiosk --user-data-dir=/home/keegan/.config/chromium-yt"}},
                        {"Midori",
                         "Lightweight and fast",
                                   {"exec --no-startup-id /usr/lib64/gnome-settings-daemon/gnome-settings-daemon &",
                                    "startx /usr/bin/midori -- -dpi 192"}},
                        {"eLinks",
                         "Terminal-based web browser",
                                   {"elinks"}},
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                    };

Entry gameslist[] =  {
                        {"DOS Games", "DOS Browser", {"667"}},
                        {"DOOM", "Fuckin Doom", {"668"}},
                        {"NetHack",
                         "Complicated adventure game",
                                   {"nethack"}},
                        {"Rogue",
                         "NetHack's grandfather",
                                   {"rogue"}},
                        {"Nyancat",
                         "Nyancat console game",
                                   {"nyancat-console"}},
                        {"Braincurses",
                         "Master Mind basically",
                                   {"braincurses"}},
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                    };

Entry medialist[] = {
                        {"Krita",
                         "Drawing program",
                                   {"startx /usr/bin/krita -- -dpi 192"}},
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                    };

Entry misclist[] = {
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                    };

Entry syslist[] =   {
                        {"Network Manager",
                         "Select network",
                                   {"nmtui"}},
                        {"ALSA Mixer",
                         "Volume control",
                                   {"alsamixer"}},
                        {"Bluetooth Control",
                         "Command line bluetooth tool",
                                   {"bluetoothctl"}},
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                    };

Entry exitlist[] =  {
                        {"Quit to shell",
                         "",
                                    {"666"}},
                        {"Shut down",
                         "",
                                   {"systemctl -i poweroff"}},
                        {"Reboot",
                         "",
                                   {"systemctl -i reboot"}},
                        {"Hibernate",
                         "",
                                   {"systemctl -i hibernate"}},
                        {"<--",
                         "Main Menu",
                                   {"0"}},
                        {(char *)NULL, (char *)NULL, {(char *)NULL}},
                     };


Entry *list[MENUS] = {mainlist, envlist, browslist, gameslist, medialist, misclist, syslist, exitlist};



/*
Entry *list[] = {
                   {
                    {"Desktop Environments", "-->", "1"},
                    {"Web Browsers", "-->", "2"},
                    {"Games", "-->", "3"},
                    {"Media", "-->", "4"},
                    {"Miscellaneous", "-->", "5"},
                    {"System", "-->", "6"},
                    {"TempleOS", "God's Perfect OS", "startx /usr/bin/ratpoison -f ~/.rptempleosrc -- -dpi 192"},
                    {"Quit to shell", "", "666"},
                    {(char *)NULL, (char *)NULL},
                   },

                  {
                   {"Plasma KDE",
                    "State-of-the-art and shiny",
                                    "gsettings set org.gnome.desktop.interface gtk-theme \"Arc-Darker\" &",
                                    "startx /usr/bin/startkde -- -dpi 192"},
                   {"GNOME 3",
                    "Ol' Reliable",
                                    "gsettings set org.gnome.desktop.interface gtk-theme \"Arc-Darker\" &",
                                    "startx /usr/bin/gnome-session -- -dpi 192"},
                   {"Openbox",
                    "Lightweight, very customized",
                                    "startx /usr/bin/openbox-session -- -dpi 192"},
                   {"AfterStep",
                    "Work in progress",
                                    "gsettings set org.gnome.desktop.interface gtk-theme \"Arc\" &",
                                    "startx /usr/bin/afterstep -- -dpi 192"},
                   {"i3",
                    "Tiled window manager",
                                    "startx /usr/bin/i3 -- -dpi 192"},
                   {"RatPoison",
                    "Bare bones as it gets",
                                    "startx /usr/bin/ratpoison -- -dpi 192"},
                   {"<--",
                    "Main Menu",
                                    "0"},
                   {(char *)NULL, (char *)NULL},
                  },

                   {  
                        {"Chromium",
                         "Open-source version of Chrome",
                                    "startx /usr/bin/chromium -- -dpi 192"},
                        {"Midori",
                         "Lightweight and fast",
                                    "exec --no-startup-id /usr/lib64/gnome-settings-daemon/gnome-settings-daemon &",
                                    "startx /usr/bin/midori -- -dpi 192"},
                        {"eLinks",
                         "Terminal-based web browser",
                                    "elinks"},
                        {"<--",
                         "Main Menu",
                                    "0"},
                        {(char *)NULL, (char *)NULL},
                    },

                    {
                        {"DOS Games", "DOS Browser", "667"},
                        {"NetHack",
                         "Complicated adventure game",
                                    "nethack"},
                        {"Rogue",
                         "NetHack's grandfather",
                                    "rogue"},
                        {"Braincurses",
                         "Master Mind basically",
                                    "braincurses"},
                        {"<--",
                         "Main Menu",
                                    "0"},
                        {(char *)NULL, (char *)NULL},
                    },

                    {
                        {"Krita",
                         "Drawing program",
                                    "startx /usr/bin/krita -- -dpi 192"},
                        {"<--",
                         "Main Menu",
                                    "0"},
                        {(char *)NULL, (char *)NULL},
                    },

                    {
                        {"<--",
                         "Main Menu",
                                    "0"},
                        {(char *)NULL, (char *)NULL},
                    },

                    {
                        {"Network Manager",
                         "Select network",
                                    "nmtui"},
                        {"<--",
                         "Main Menu",
                                    "0"},
                        {(char *)NULL, (char *)NULL},
                    },
                };
*/


