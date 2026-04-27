include .env
export

CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lcurl -lcjson

OBJS = profile_visualizer.o http.o

profile_visualizer: $(OBJS)
	$(CC) $(OBJS) -o profile_visualizer $(LIBS)

profile_visualizer.o: profile_visualizer.c
	$(CC) $(CFLAGS) -c profile_visualizer.c -o profile_visualizer.o

http.o: http.c
	$(CC) $(CFLAGS) -c http.c -o http.o

run: profile_visualizer
	./profile_visualizer

clean:
	rm -f *.o profile_visualizer