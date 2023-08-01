CC=gcc

all: dfs s1 s2 s3 locals4 locals5 c

dfs: dfs.c
	$(CC) dfs.c -o dfs

s1: s1.c
	$(CC) s1.c -o s1

c: c.c
	$(CC) c.c -o c -pthread

s2: s2.c
	$(CC) s2.c -o s2

s3: s3.c
	$(CC) s3.c -o s3

locals4: locals4.c
	$(CC) locals4.c -o locals4

locals5: locals5.c
	$(CC) locals5.c -o locals5

clear: 
	rm dfs s1 s2 s3 locals4 locals5 c mysocket client_socket1 client_socket2