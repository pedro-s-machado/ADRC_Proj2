main: main.c network.c network.h
	gcc -Wall main.c network.c network.h -o main	

clean:
	rm -f *.o main main.exe.stackdump