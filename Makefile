
all:
	gcc -Wall -O2 -o hide2xattr  hide2xattr.c xz.c scandir.c base64.c -static -llzma

clean:
	rm -rf hide2xattr 
