DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRRES := resources/

CFLAGS := -I$(DIRHEA) -c -std=c++20
LDLIBS := -lpthread -lrt 
CC := g++

all : dirs ssooiicle

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

ssooiicle: $(DIROBJ)ssooiigle.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

test:
	./$(DIREXE)ssooiicle 

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
