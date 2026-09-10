#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

size_t random_file(const char *directory)
{
    DIR *dir = opendir(directory);
    if (!dir)
        return 0;

    struct dirent *entry;
    size_t count = 0;

	while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
            count++;
    }

	closedir(dir);

    if (count == 0)
        return 0;

    return rand() % count;
}
