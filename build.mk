ifndef SID_ROOT
	$(error You must set the environment variable SID_ROOT)
endif

# This is done to dereference if SID_ROOT is a symbolic link
PROJECT_ROOT=$(shell readlink -f $(SID_ROOT))

PWD:=$(CURDIR)
#PWD:=$(PWD)
ifeq ($(filter $(PROJECT_ROOT) $(PROJECT_ROOT)/%,$(PWD)),)
$(warning Current directory $(PWD) is not inside SID_ROOT $(PROJECT_ROOT); SID_ROOT may be incorrect.)
endif

######################################################
# Default flags
CC = gcc
CXX= g++
AR = ar
ARFLAGS = r
MAKEDEP = $(CC) -MM
MAKEDEPXX = $(CXX) -MM
MKDIRS = mkdir -p
LD=$(if $(strip $(PUREC)),$(CC),$(CXX))

CFLAGS = -Wall -g -O2 -fshort-wchar -Wno-frame-address -MD
#CFLAGS = -g -c -MD -Wall

CPPFLAGS = -D_GNU_SOURCE=1 -I$(PROJECT_ROOT)/include $(LOCAL_INCLUDES)
LDFLAGS = $(LOCAL_LIBS)

CFLAGS += -Werror
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-strict-aliasing

ifdef DEBUG_BUILD
CPPFLAGS += -DDEBUG -D_DEBUG -DR_DEBUG
else
CPPFLAGS += -D_RELEASE
endif

# well, we haven't done anything separate...
CXXFLAGS = $(CFLAGS) -std=gnu++11
######################################################

MODULE_PATH:=$(PROJECT_ROOT)/$(shell echo "$(PWD:$(PROJECT_ROOT)/%=%)" | cut -d/ -f1)

#OUT_LIB_PATH:=$(MODULE_PATH)/lib
OUT_LIB_PATH:=$(PROJECT_ROOT)/lib

ifndef MACH
MACH:=$(shell uname -m)
endif
ifndef OS
OS:=$(shell uname -s | awk '{ print tolower($$1) }')
endif
ifndef OS_LEVEL
OS_LEVEL:=$(shell uname -r | awk -F'.' '{ print $$1 "." $$2 }')
endif

ARCH=$(MACH)-$(OS)

OUT_DIR = obj
ifdef LIB_PROJ
DEST_FILE:=lib$(LIB_PROJ).a
endif
ifdef BIN_PROJ
DEST_FILE:=$(BIN_PROJ)
endif

ifdef DEST_FILE
OUT_FILE=$(OUT_DIR)/$(DEST_FILE)
endif
OBJECT_FILES = $(SOURCE_FILES:%.cpp=$(OUT_DIR)/%.o)

default: all

# create object files directory
dircreate:
ifdef DEST_FILE
	mkdir -p $(OUT_DIR) 2>/dev/null
endif

# rules
$(OUT_DIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_FLAGS) -o $@ -c $<
$(OUT_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_FLAGS) -o $@ -c $<
$(OUT_DIR)/%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_FLAGS) -o $@ -c $<
$(OUT_DIR)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $($*_FLAGS) -o $@ -c $<

$(OUT_DIR)/%.o: %.s
	$(AS) $(ASFLAGS) $($*_FLAGS) -o $@ $<

$(OUT_FILE): $(OBJECT_FILES)
ifdef LIB_PROJ
	ar rcs $@ $(OBJECT_FILES)
else
	$(LD) $(OBJECT_FILES) $(LDFLAGS) -o $@
endif
	cp -f $@ $(OUT_LIB_PATH)/

# make X in subdirs
$(PRE_SUBDIRS):
	$(MAKE) -C $@

# make X in subdirs
$(POST_SUBDIRS):
	$(MAKE) -C $@

subdir.%:
	@for d in $(PRE_SUBDIRS) $(POST_SUBDIRS) ; do \
		$(MAKE) -C $$d $* || exit 1 ; \
	done

# local.clean is extensible
local.clean::
	rm -f $(OUT_DIR)/*.o $(OUT_DIR)/*.d $(OUT_FILE)
ifdef DEST_FILE
	rm -f $(OUT_LIB_PATH)/$(DEST_FILE)
endif

local.unmake::
	rm -Rf $(OUT_DIR)
ifdef DEST_FILE
	rm -f $(OUT_LIB_PATH)/$(DEST_FILE)
endif

clean: subdir.clean local.clean

# clean .depend too
unmake: subdir.unmake local.unmake

# main target
all: $(PRE_SUBDIRS) dircreate $(SOURCE_FILES) $(OUT_FILE) $(POST_SUBDIRS)

.PHONY: all clean unmake $(PRE_SUBDIRS) $(POST_SUBDIRS)

# include dependency files for each source file
-include $(OBJECT_FILES:%.o=%.d)
