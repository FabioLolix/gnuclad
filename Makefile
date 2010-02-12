.PHONY: all debug clean install uninstall texinfo

# headers for which (when changed) all objects should be recompiled:
SHAREDHEADERS=source/gnuclad.h source/gnuclad-portability.h

SOURCEDIR = source/
OBJDIR = bin/obj/
BINDIR = bin/
BIN = $(BINDIR)gnuclad
INSTALLBIN = /usr/bin/gnuclad

INFODIR = MANUAL/source/

SMART_MKDIR = mkdir -p

CC = g++
CFLAGS   = -Wall -Wextra -Werror -pedantic-errors -ansi -O2
DBGFLAGS = -Wall -Wextra -Werror -pedantic-errors -ansi -O0 -g3 # -pg


# Automation starts here
################################################################################

VPATH = $(SOURCEDIR)
HEADERS=$(wildcard $(SOURCEDIR)*.h)
SOURCES=$(wildcard $(SOURCEDIR)*.cpp)
OBJS=$(subst $(SOURCEDIR),$(OBJDIR),$(SOURCES:.cpp=.o))
HEADERS_W_SOURCES=$(wildcard $(SOURCES:.cpp=.h))
OBJS_W_HEADERS=$(subst $(SOURCEDIR),$(OBJDIR),$(HEADERS_W_SOURCES:.h=))

all: $(BIN)

$(BIN): $(OBJS)
	@$(SMART_MKDIR) $(BINDIR)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS)
	@echo Shared headers are: $(SHAREDHEADERS)

# The following line isn't even required (but works!), which seems very
# illogical because the line below it already has OBJDIR in the %   ... magic
#$(OBJDIR)$(subst $(OBJDIR),,%.o): %.cpp $(SHAREDHEADERS)
$(OBJDIR)%.o: %.cpp $(SHAREDHEADERS)
	@$(SMART_MKDIR) $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Workaround for conditional prerequisites of header files
$(foreach file,$(OBJS_W_HEADERS),$(eval \
$(file).o: $(subst $(OBJDIR),$(SOURCEDIR),$(file).h)\
))


debug: $(SOURCES) $(HEADERS)
	$(CC) $(DBGFLAGS) -o $(BIN)-dbg $(SOURCES)

clean:
	rm -f $(BIN)
	rm -f $(BIN)-dbg
	rm -f $(BINS)
	rm -f $(OBJS)
	rm -rf $(OBJDIR)

install: $(BIN)
	sudo cp $(BIN) $(INSTALLBIN)

uninstall: $(INSTALLBIN)
	sudo rm $(INSTALLBIN)


texinfo:
	cd $(INFODIR) && $(MAKE)
