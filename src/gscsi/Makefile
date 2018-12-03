
PROJ = sid_gscsi
CC = g++
INCL = -I../../include
CFLAGS = -g -c -MD -Wall $(INCL)
CPPFLAGS = -std=gnu++11

SRC =	Device.cpp

ODIR = obj
DEST=$(ODIR)/lib$(PROJ).a
OBJ = $(SRC:%.cpp=$(ODIR)/%.o)

all: dircreate $(SRC) $(DEST) $(POSTSUBDIRS)

# create object files directory
dircreate:
	mkdir -p $(ODIR) 2>/dev/null

# Compile all source files to object files
$(ODIR)/%.o : %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

# Link all the object files to a binary
$(DEST): $(OBJ)
	ar rcs $@ $(OBJ)
	cp $@ ../../lib/

.PHONY: $(POSTSUBDIRS)
$(POSTSUBDIRS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	rm -f *~ $(OBJ) $(DEST)
	for dir in $(POSTSUBDIRS); do $(MAKE) -C $$dir clean; done

.PHONY: unmake
unmake:
	rm -Rf *~ $(ODIR)
	for dir in $(POSTSUBDIRS); do $(MAKE) -C $$dir unmake; done

# include dependency files for each source file
-include $(OBJ:%.o=%.d)
