#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <dirent.h>
#include "base64.h"

#define XATTR_SIZE 4000

extern int xz_compress (FILE *in_file, FILE *out_file);
extern int xz_decompress (FILE *in_file, FILE *out_file);

extern int scan(struct dirent ***namelist, const char *dir);
extern char *getfile(struct dirent **namelist, char *dir, int n);

static struct dirent **namelist;
static int d_count = 0;

int main(int argc, char **argv)
{
	struct stat st;
	FILE *f_in = NULL;
	FILE *f_out = NULL;
	char *dstfile = NULL;
	char *buf = NULL;
	char *ptr = NULL;

	int count = 0;
	int i = 0;
	int len = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s file dir\n", argv[0]);
		return -1;
	}

	if (stat(argv[1], &st) < 0)
		goto end;

	f_in = fopen(argv[1], "r");
	if (!f_in)
		goto end;

	buf = calloc(st.st_size, 3);
	if (!buf)
		goto end;

	f_out = fmemopen(buf, st.st_size, "w+");
	if (!f_out)
		goto end;
	
	d_count = scan(&namelist, argv[2]);
	if (d_count <= 0) {
		fprintf(stderr, "scandir %s fail\n", argv[2]);
		goto end;
	}

	count = xz_compress (f_in, f_out);
	if (count <= 0)
		goto end;
	fprintf(stdout, "xz_compress size %d\n", count);

	fflush(f_out);
	ptr = buf + count;
	//for (i=0; i<count; i++)
	//	sprintf(ptr+2*i, "%02X", (unsigned char)buf[i]);
	base64_encode(buf, count, ptr, BASE64_LENGTH(count));

        //count *= 2;
	count = BASE64_LENGTH(count);
	i = 0;
	while (count > 0) {
		dstfile = getfile(namelist, argv[2], d_count);
		if (!dstfile) 
			goto end;
		d_count--;

		len = count > XATTR_SIZE ? XATTR_SIZE : count;
		if (setxattr(dstfile, "user.i", ptr+i, len, 0) != 0)  
			continue;
		fprintf(stdout, "setxattr %s xattr %d bytes\n", dstfile, len);
		free(dstfile);
		count -= len;
		i += len;
	}

end:
	fprintf(stdout, "setxattr total size %d\n", i);
	if (count > 0)
		fprintf(stdout, "Too less file in the dir: %s\n", argv[2]);
	//rewind(f_out);
        //xz_decompress(f_out, stdout);
	while (d_count > 0)
		free(namelist[--d_count]);
	free(namelist);

	if (f_out)
		fclose(f_out);
	if (buf)
		free(buf);
	if (f_in)
		fclose(f_in);
	return 0;
}
