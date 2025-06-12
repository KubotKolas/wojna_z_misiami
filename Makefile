SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.c=.hpp)
FLAGS=-DDEBUG -g
#FLAGS=-g

all: main tags

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o wojna_z_misiami

clear: clean

clean:
	rm wojna_z_misiami

tags: ${SOURCES} ${HEADERS}
	ctags -R .

run: main Makefile tags
	mpirun -oversubscribe -np 8 ./wojna_z_misiami
