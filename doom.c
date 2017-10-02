#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int doomselector(const struct dirent *entry);
int isdoom(char *file);

int doom(void)
{

}

int doomselector(const struct dirent *entry)
{
    if(isdoom(entry->d_name))
        return 1;
    else
        return 0;
}

int isdoom(char *file)
{
    if(
        strstr(file, ".wad") != NULL ||
        strstr(file, ".WAD") != NULL ||
        strstr(file, ".deh") != NULL ||
        strstr(file, ".DEH") != NULL ||
        strstr(file, ".pk3") != NULL ||
        strstr(file, ".PK3") != NULL
      )
        return 1;
    else
        return 0;
}
