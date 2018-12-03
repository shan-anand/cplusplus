
PROJ = sid_common
CC = g++
INCL = ../../include
CFLAGS = -g -c -MD -Wall -I$(INCL)
CPPFLAGS = -std=gnu++11

SRC =	\
			io_buffer.cpp

ODIR = obj
DEST=$(ODIR)/lib$(PROJ).a
OBJ = $(SRC:%.cpp=$(ODIR)/%.o)

all: dircreate $(SRC) $(DEST)
# Compile all source files to object files

dircreate:
	mkdir -p $(ODIR) 2>/dev/null

$(ODIR)/%.o : %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

# Link all the object files to a binary
$(DEST): $(OBJ)
	ar rcs $@ $(OBJ)
	cp $@ ../../lib/

.PHONY: clean
clean:
	rm -f *~ $(OBJ) $(DEST)

.PHONY: unmake
unmake:
	rm -Rf *~ $(ODIR)

# include dependency files for each source file
-include $(OBJ:%.o=%.d)
