CC = gcc

all : bless.c blessdriver.c paddriver.c
	$(CC) -g -o blessapp -I../include bless.c md5c.c
	$(CC) -g -o blessdriver -I../include blessdriver.c md5c.c
	$(CC) -g -o paddriver -I../include paddriver.c
	$(CC) -g -o blessdisk -I../include blessdisk.c md5c.c
	$(CC) -g -o padapp -I../include padapp.c

blessapp : bless.c
	$(CC) -g -o $@ -I../include bless.c md5c.c

blessdisk : blessdisk.c
	$(CC) -g -o $@ -I../include blessdisk.c md5c.c

blessdriver : blessdriver.c
	$(CC) -g -o $@ -I../include blessdriver.c md5c.c

paddriver : paddriver.c
	$(CC) -g -o $@ -I../include paddriver.c

padapp : padapp.c
	$(CC) -g -o $@ -I../include padapp.c

clean:
	rm blessapp blessdriver paddriver blessdisk padapp
