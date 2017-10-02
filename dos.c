#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int dosselector(const struct dirent *entry);
int isexec(char *p);

int dos(void)
{
    struct dirent **namelist = malloc(sizeof(struct dirent));

    int nent;
    int i;
    int c;
    
    char *choices[255];
    ITEM **dositems = NULL;
    MENU *dosmenu = NULL;
    
    const char *rootpath = "/home/keegan/Games/dos";    //location of DOS folder
    char path[255];                                     //path of current folder
    
    strcpy(path, rootpath);                             //start routine at rootpath

    //repeats every time a new menu is navigated to
    while(TRUE) {
        int curspos = 1;

        //handy command for scanning directory and sorting the entries alphabetically
        if((nent = scandir(path, &namelist, dosselector, alphasort)) == -1) {
            mvwprintw(welcomewin, 1, 2, "Error scanning directory");
            return 1;
        }

        //place entry names into string array
        for(i=0; i<nent; i++)
            choices[i] = namelist[i]->d_name;      

        wrefresh(welcomewin);
        if(ARRAY_SIZE(choices) == 0) {
            mvwprintw(welcomewin, 1, 2, "ERROR: directory empty");
            return 1;
        }

        if(dositems == NULL)
            dositems = calloc(nent, sizeof(ITEM *));

        for(i=0; i<nent; i++)
            dositems[i] = new_item(choices[i], choices[i]);
        dositems[nent] = NULL;
        dosmenu = new_menu(dositems);
        
        //don't show menu description
        menu_opts_off(dosmenu, O_SHOWDESC);
        set_menu_win(dosmenu, welcomewin);
        set_menu_sub(dosmenu, derwin(welcomewin, 
                                     30, 126, 2, 2));
    //                                 welcomewinh-8, welcomewinw-8, welcomewinh/2-4, welcomewinw/2-4));
        set_menu_format(dosmenu, 22, 3);
        set_menu_mark(dosmenu, " > ");
        
        post_menu(dosmenu);
        wrefresh(welcomewin);
        
        keypad(welcomewin, TRUE);
    
        //input loop    
	    do {   
	        c = wgetch(welcomewin);    
	        switch(c) {	
	            case KEY_DOWN:
				    menu_driver(dosmenu, REQ_DOWN_ITEM);
				    break;
			    case KEY_UP:
				    menu_driver(dosmenu, REQ_UP_ITEM);
				    break;
			    case KEY_LEFT:
				    menu_driver(dosmenu, REQ_LEFT_ITEM);
				    break;
			    case KEY_RIGHT:
				    menu_driver(dosmenu, REQ_RIGHT_ITEM);
				    break;
			    case KEY_NPAGE:
				    menu_driver(dosmenu, REQ_SCR_DPAGE);
				    break;
			    case KEY_PPAGE:
				    menu_driver(dosmenu, REQ_SCR_UPAGE);
				    break;
		        case 10:
		            {
		                ITEM *cur;
		                
		                cur = current_item(dosmenu);
		                char *p = item_name(cur);

                        if(!strcmp(p, "..")) {
	                        if(!strcmp(path, rootpath)) {   //don't navigate below DOS folder
	                            c = 0;
	                            break;
	                        }	
	                        int len = strlen(path);      
	                        char x;                
	                        do {	         //instead of just daisy-chaining ../../etc together	  
	                            x=path[len];
	                            path[len--] = '\0';
	                        } while(x != '/');
	                        
	                    } else if(isexec(p)) {
                            char runme[255];
                            sprintf(runme, "startx /usr/bin/dosbox %s/%s -exit -- -dpi 192", path, p);
                            def_prog_mode();
                            endwin();
                            system(runme);
                            reset_prog_mode();
                            c = 0;  //keep the menu from resetting after execution
                            
	                    } else {                    //navigate to a new folder
	                        strcpy(path, strcat(path, "/"));
	                        strcpy(path, strcat(path, p));
	                    }
		            }
		            break;
		        case 'Q': case 'q':         //quit DOS navigator
		            unpost_menu(dosmenu);
                    for(i=0; i<nent; i++)
                        free_item(dositems[i]);
                    free_menu(dosmenu);
		            return 0;
		    }
                    wrefresh(welcomewin);
	    } while(c != 10);

        unpost_menu(dosmenu);
        for(i=0; i<nent; i++)
            free_item(dositems[i]);
        free_menu(dosmenu);
    }
}

//Note that the item will NOT be selected if the function returns zero!
//selects only exe, batch, or com files (or subdirectories which are not ".")
int dosselector(const struct dirent *entry)
{
    if((isexec(entry->d_name) || entry->d_type == DT_DIR) && strcmp(entry->d_name, ".") != 0)
        return 1;
    else
        return 0;
}

int isexec(char *p)
{
    if(
        strstr(p, ".exe") != NULL ||
        strstr(p, ".EXE") != NULL ||
        strstr(p, ".bat") != NULL ||
        strstr(p, ".BAT") != NULL ||
        strstr(p, ".com") != NULL ||
        strstr(p, ".COM") != NULL
      )
        return 1;
    else
        return 0;
}
