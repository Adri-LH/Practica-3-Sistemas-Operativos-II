DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRRES := resources/user_results

CFLAGS := -I$(DIRHEA) -c -std=c++20
LDLIBS := -lpthread -lrt 
CC := g++

all : dirs ssooiicle

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRRES)

ssooiicle: $(DIROBJ)ssooiigle.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

solucion:
	./$(DIREXE)ssooiicle 

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~ $(DIRRES)
