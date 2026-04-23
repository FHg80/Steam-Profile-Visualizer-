include .env
export

profile_visualizer.o: profile_visualizer.c
	gcc -c profile_visualizer.c -o profile_visualizer.o

profile_visualizer: profile_visualizer.o
	gcc profile_visualizer.o -o profile_visualizer -lcurl -lcjson

run: profile_visualizer
	./profile_visualizer

clean:
	rm -f *.o profile_visualizer