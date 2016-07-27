CC = gcc-3.3
COPT = -O2 -ffast-math -DBLKSIZE=1024 -Wall
OBJ = syna.o main.o osa.o biisi.o

yellow_rose: $(OBJ)
	$(CC) -o $@ $(OBJ) `sdl-config --libs` -lGL -lm
	strip $@

%.o: %.c
	$(CC) $(COPT) -c $<
       
clean:
	rm *.o yellow_rose *~

