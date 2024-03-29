# ------------------------------------------------
#originates from stackoverflow user 'qroberts' post
#http://stackoverflow.com/a/27794283
#http://ubuntuforums.org/showthread.php?t=1879827
# ------------------------------------------------

# Compiler
CC			:= g++
# The Target Binary Program
TARGET      := program
# The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
# Extensions
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

# Flags, Libraries and Includes
CFLAGS      := -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
-Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual \
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused-parameter -std=c++11 -O3
#this links against shared libraries
#LIB			:= -lSDL2 -lSDL2_image -lSDL2_ttf -Llib/shared
#this makes links against static libraries, a.k.a creates program with no need of .so files
LIB         := -Llib/static -Wl,-rpath,./lib/static -lSDL2 -lSDL2_image -lSDL2_ttf -lfreetype -lz -lpng12 -lpthread -Wl,--no-undefined -lm -ldl -lpthread -lrt -Llib
#if use static linking, remember include lib/static
INC         := -I$(INCDIR)
INCDEP      := -I$(INCDIR)

SOURCES		:=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#SOURCES		:= $(wildcard $(addsuffix /*.$(SRCEXT), $(SRCDIR)))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Defauilt Make
all: directories $(TARGET)

run: all
	$(TARGETDIR)/$(TARGET)
# Remake
remake: cleaner all

# Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

# Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Link
$(TARGET): $(OBJECTS)
	@echo "Linking " $(TARGETDIR)/$(TARGET)
	@$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

# Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo "Building " $@
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources
