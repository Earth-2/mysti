/*  Draw the user interface, handle user input  */

#include "mysti.h"
#include "menus.c"
#include "statbar.c"
#include "dos.c"

#include <string.h>

int main(void)
{
    int c;
    int i, j;
    int nmenus;
    
    int cursy;
    int cursx;

    //pid_t statpid;

    FILE *output;

    MENU *currentmenu;      //whichever menu is currently displayed
//    char *currenttitle;
    int select;

    struct Menus menus[MENUS];

    //initialize menus[] array; contains allocated menu and item arrays, the list of menu items and associated commands,
    //and the length and title of each menu
    for(i=0; i<MENUS; i++) {
        menus[i].menu   = (MENU **)malloc(sizeof(MENU *));
        menus[i].items  = (ITEM ***)malloc(sizeof(ITEM **));
        menus[i].list   = list[i];
        menus[i].len    = sizeof(list) / sizeof(list[i]);
        menus[i].title  = title[i];
    }

    //start curses mode
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    //init color pairs
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_GREEN, COLOR_BLACK);
    
    const int MAGENTA = COLOR_PAIR(1);
    const int WHITE = COLOR_PAIR(2);
    const int YELLOW = COLOR_PAIR(3);
    const int RED = COLOR_PAIR(4);
    const int BLUE = COLOR_PAIR(5);
    const int GREEN = COLOR_PAIR(6);
 
    const int menwinw = COLS/2;
    const int menwinh = LINES/2-7;
    
    const int welcomewinw = COLS;
    const int welcomewinh = LINES/2 + 5;    

    //create windows
    menuwin = createwin(menwinh, menwinw, LINES/2+6, 0, title[MAIN], YELLOW);
    welcomewin = createwin(welcomewinh, welcomewinw, 0, 0, "Welcome to Arch Linux!", MAGENTA);
    monitorwin = createwin(LINES/2-7, COLS/2, LINES/2+6, COLS/2 + 1, "System Monitor", RED);


    statbarwin = newwin(1, COLS, LINES-1, 0);
    //statbar(&statbarwin);
    statbar();

    keypad(menuwin, TRUE);

    //set up the actual menu objects
    nmenus = ARRAY_SIZE(menus);
    for(i=0; i<nmenus; i++) {
        *(menus[i].menu) = initmenu(menus[i].items, menus[i].list, menus[i].len);
        set_menu_win(*(menus[i].menu), menuwin);
        set_menu_sub(*(menus[i].menu), derwin(menuwin, 10, 50, menwinh/2-5, menwinw/2-25));
    }

    currentmenu = *(menus[MAIN].menu);
//    currenttitle = title[MAIN];
    post_menu(currentmenu);

    wmove(welcomewin, 1, 1);
    //system("linux_logo -L 10");

    drawwin(menuwin, title[MAIN], COLOR_PAIR(3));
    drawwin(welcomewin, "Welcome to Arch Linux!", MAGENTA);
    drawwin(monitorwin, "System Monitor", RED);

    nodelay(menuwin, 1);

    //input loop
    while((c = wgetch(menuwin)) != KEY_F(4)) {
        //statbar(&statbarwin);   //refresh statbar
        statbar();

        switch(c) {
        case KEY_DOWN:
            menu_driver(currentmenu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(currentmenu, REQ_UP_ITEM);
            break;
        case 10:    //select menu item
            {
                ITEM *cur;
                char **p;
                cur = current_item(currentmenu);
                p = item_userptr(cur);

                select = execute(p);

                pos_menu_cursor(currentmenu);
            }
            break;
        case 'x':
            echo();
            curs_set(2);
            {
                scrollok(welcomewin, TRUE);

                char cmd[255];
                //char *cmds[2];
                
                cursy = 1;
                cursx = 2;
                
                char line[255];
                
                
                for(i=1; i<welcomewinh; i++) {
                    wmove(welcomewin, i, 2);
                    wclrtoeol(welcomewin);
                } 
                wclear(welcomewin);
                drawwin(welcomewin, "Executing", MAGENTA);
                wattron(welcomewin, WHITE);
                mvwprintw(welcomewin, cursy, cursx, "Execute: ");
                wrefresh(welcomewin);
                wgetnstr(welcomewin, cmd, 255);                     //take user input
                //cmds[0] = cmd;
                
                output = popen(cmd, "r");
                if(!output) {
                    mvwprintw(welcomewin, cursy, cursx, "Command failed.");
                    break;
                }
                //int k = 1;
                while(fgets(line, welcomewinw-2, output)) {
                    char ch;
                    cursx = 2;
                    if(cursy++ >= welcomewinh) {
                        scroll(welcomewin);
                        wrefresh(welcomewin);
                    }
                    for(i=0; (ch=line[i])!='\0'; i++) {
                        if(ch == '\n' || cursx > COLS) {
                            //cursy++;
                            cursx = 2;
                            continue;
                        }
                        if(ch == '\033') {
                            char col[255];
                            for(j=0; (col[j]=line[i])!='m' && j<255; i++, j++)
                                ;
                            //mvwprintw(monitorwin, k++, 2, "it is %s", col);
                            //wrefresh(monitorwin);
                            if(!strcmp(col, "\033[1;34m"))
                                wattron(welcomewin, BLUE);
                            else if(!strcmp(col, "\033[1;33m"))
                                wattron(welcomewin, YELLOW);
                            //if(!strcmp(col, "\033[0m34m"))
                            //        wattron(welcomewin, WHITE);
                            else if(!strcmp(col, "\033[1;31m"))
                                wattron(welcomewin, RED);
                            else if(!strcmp(col, "\033[1;35m"))
                                wattron(welcomewin, MAGENTA);
                            else if(!strcmp(col, "\033[1;32m"))
                                wattron(welcomewin, GREEN);
                            else
                                wattron(welcomewin, WHITE);
                            continue;
                        }
                        mvwaddch(welcomewin, cursy, cursx++, ch);
                    }
                        
                    //mvwprintw(welcomewin, cursy, cursx, "%s", line);
                    wrefresh(welcomewin);
                }

                //execute(cmds);
                
                //wmove(welcomewin, 1, 2);
                //wclrtoeol(welcomewin);
                drawwin(welcomewin, "Execution complete.", MAGENTA);
                pclose(output);
            }
            noecho();
            curs_set(0);
            pos_menu_cursor(currentmenu);
            break;

        case '.':                   //raise brightness
            system("light -A 5");
            break;
        case ',':                   //lower brightness
            system("light -U 5");
            break;
        }

        //process menu item selection
        if(select > -1) {
            switch(select) {
            case MAIN: case ENV: case BROWS: case GAMES: case MEDIA: case MISC: case SYS: case EXIT:
                unpost_menu(currentmenu);
                currentmenu = *(menus[select].menu);
//                currenttitle = title[select];
                break;
            //DOS game player
            case DOS:
                /*mvwprintw(monitorwin, 2, 2, "Mousie");
                wrefresh(monitorwin);*/
                select = dos();
                pos_menu_cursor(currentmenu);
                wclear(welcomewin);
                drawwin(welcomewin, "Arch Linux", MAGENTA);
                /*mvwprintw(monitorwin, 2, 2, "New mousie");
                wrefresh(monitorwin);*/
                break;
            case QUIT:
                goto quit;
                break;
            default:
                break;
            }
            post_menu(currentmenu);
        }
//        drawwin(menuwin, menus[select].title, COLOR_PAIR(3));
        if(select == -1) {
            //char prog[255];
            //*prog = *(menus[select].title);
            //char title[255];
            //mvwprintw(welcomewin, 1, 2, "%s", prog);
            //wrefresh(welcomewin);
            char *title = "Execution complete";
            //sprintf(title, "Execution of %s complete.", prog);
            drawwin(welcomewin, title, MAGENTA);
        }
    }

    //end this shit
quit:
    unpost_menu(currentmenu);

    for(i=0; i<nmenus; i++) {
        for(j=0; j<ARRAY_SIZE(menus[j].list); j++)
            free_item(*(menus[i].items)[j]);
        free_menu(*(menus[i].menu));
    }


    delwin(menuwin);
    delwin(welcomewin);
    delwin(statbarwin);

    clear();
    refresh();

    endwin();

    //display logo on way out
    system("clear ; linux_logo -L 10 -o 8"); //-F \"\\\\n\\\\n#EKeegan's Arch Linux Build Supremo\"");
    printf("You can return to this screen at any time by typing 'mysti' in the command line.\n");

    return 0;
}


//initialize window
WINDOW *createwin(int height, int width, int starty, int startx, char *title, int color)
{
    WINDOW *localwin;

    localwin = newwin(height, width, starty, startx);
    drawwin(localwin, title, color);
    wrefresh(localwin);

    return localwin;
}

//draw borders and titles and whatnot around windows
void drawwin(WINDOW *win, char *title, int color)
{
    wattron(win, color);
    //wborder(win, 186, 186, 205, 205, 201, 187, 200, 188);
    //wborder(win, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD);
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, 0, 4, "%s", title);

    wattron(win, color);
    wrefresh(win);
}

