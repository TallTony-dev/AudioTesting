makefile:
#Heavy llm usage here
COMPILER = clang++
CC = clang

OBJDIR = obj
CPPFILES := $(shell find src -name '*.cpp')
CFILES := $(shell find src -name '*.c')
CPP_OBJECTS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))
C_OBJECTS := $(patsubst src/%.c,$(OBJDIR)/%.o,$(CFILES))
OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

MACOS_OPT = -std=c++23
MACOS_FRAMEWORKS = -framework CoreAudio -framework AudioToolbox -framework CoreFoundation
MACOS_OUT = -o "bin/build_osx"
WINDOWS_OPT = -std=c++23
WINDOWS_OUT = -o "bin/build_windows"

all: build_osx build_windows ##build_web

build_osx: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(MACOS_OUT) $(MACOS_OPT) $(MACOS_FRAMEWORKS)

build_windows: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(WINDOWS_OUT) $(WINDOWS_OPT)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(COMPILER) -c $< -o $@

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) bin/*
