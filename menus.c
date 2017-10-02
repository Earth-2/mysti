/* Functions for initializing menus and execution of programs   */

//initialize all menu items
MENU *initmenu(ITEM ***items, Entry list[], int n)
{
//    int n = ARRAY_SIZE(list);
    *items = (ITEM **)calloc(n + 1, sizeof(ITEM *));

    int i;
    for(i=0; i<n; i++) {
        (*items)[i] = new_item(list[i].name, list[i].desc);
        set_item_userptr((*items)[i], list[i].cmds);
    }
    (*items)[n] = (ITEM *)NULL;

    return new_menu((ITEM **)*items);
}

//execute command associated with selected menu item
int execute(char **cmds)
{
    int ncmds = ARRAY_SIZE(cmds);

    int i;

    //return menu number if need be 
    if(cmds[0][0] >= '0' && cmds[0][0] <= '9')
        return atoi(cmds[0]);

    //temporarily leave curses mode and run all commands
    def_prog_mode();
    endwin();
    for(i=0; i<=ncmds; i++) {
        system(cmds[i]);
    }
    reset_prog_mode();
    //refresh();

    return -1;
}
