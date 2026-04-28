include .env
export

CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lcurl -lcjson

OBJS = profile_visualizer.o http.o api.o menu.o

profile_visualizer: $(OBJS)
	$(CC) $(OBJS) -o profile_visualizer $(LIBS)

profile_visualizer.o: profile_visualizer.c
	$(CC) $(CFLAGS) -c profile_visualizer.c -o profile_visualizer.o

http.o: http.c
	$(CC) $(CFLAGS) -c http.c -o http.o

api.o: api.c
	$(CC) $(CFLAGS) -c api.c -o api.o

menu.o: menu.c
	$(CC) $(CFLAGS) -c menu.c -o menu.o

run: profile_visualizer
	./profile_visualizer

clean:
	rm -f *.o profile_visualizer