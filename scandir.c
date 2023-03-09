#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int filter(const struct dirent* dent)
{
	int ret = 0;

	if (!dent)
		goto end;

	//if (dent->d_type != DT_REG && dent->d_type != DT_DIR)
	if (dent->d_type != DT_REG)
		goto end;
	if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
		goto end;
	ret = 1; 
		
end:
	return ret;	
}

int scan(struct dirent ***namelist, const char *dir)
{
	int ret = -1;

	if (!namelist || !dir)
		goto end;

	ret = scandir(dir, namelist, filter, alphasort);
end:
	return ret;
}

char *getfile(struct dirent **namelist, char *dir, int n)
{
	char *filename = NULL;

	if (!namelist || !dir || n <= 0)
		return NULL;

	while (n--) {
		if (namelist[n]->d_type == DT_REG) { 
			filename = calloc(PATH_MAX, 1);
			if (filename) {
				snprintf(filename, PATH_MAX, "%s/%s", dir, namelist[n]->d_name);
				free(namelist[n]);
			}
			break;
		}
	}

	return filename;
}
