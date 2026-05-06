makefile:
#Heavy llm usage here :/
COMPILER = clang++
CC = clang

OBJDIR = obj
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CPPFILES := $(filter-out src/raygui/raylib/%, $(call rwildcard,src,*.cpp))
CFILES := $(filter-out src/raygui/raylib/% src/miniaudio/%, $(call rwildcard,src,*.c))
CPP_OBJECTS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))
C_OBJECTS := $(patsubst src/%.c,$(OBJDIR)/%.o,$(CFILES))
OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

RAYLIB_DIR = src/raygui/raylib/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a
RAYLIB_INCLUDE = -I$(RAYLIB_DIR) -Isrc/raygui

MACOS_OPT = -std=c++23
MACOS_FRAMEWORKS = -framework CoreAudio -framework AudioToolbox -framework CoreFoundation -framework IOKit -framework Cocoa -framework OpenGL
MACOS_OUT = -o "bin/build_osx"
WINDOWS_OPT = -std=c++23 -lwinmm -lgdi32 -luser32 -lshell32
WINDOWS_OUT = -Wl,--export-all-symbols -Wl,--out-implib,bin/libAudioTesting.a -o "bin/build_windows.exe"

all: build_osx build_windows ##build_web

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) CC=$(CC) AR=C:/msys64/ucrt64/bin/ar PLATFORM=PLATFORM_DESKTOP OS=$(OS)
	-C:/msys64/ucrt64/bin/ranlib $(RAYLIB_LIB) || echo "ranlib failed, but ignoring"

build_osx: $(OBJECTS) $(RAYLIB_LIB)
	$(COMPILER) $(OBJECTS) $(RAYLIB_LIB) $(MACOS_OUT) $(MACOS_OPT) $(MACOS_FRAMEWORKS)
	@rsync -au src/plugins/ bin/plugins/

build_windows: $(OBJECTS) $(RAYLIB_LIB)
	$(COMPILER) $(OBJECTS) $(RAYLIB_LIB) $(WINDOWS_OUT) $(WINDOWS_OPT)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	-@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo "" > NUL
	$(COMPILER) $(RAYLIB_INCLUDE) -std=c++23 -c $< -o $@

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	-@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo "" > NUL
	$(CC) $(RAYLIB_INCLUDE) -c $< -o $@

$(OBJDIR):
	-@mkdir $(subst /,\,$(OBJDIR)) 2> NUL || echo "" > NUL

clean:
	rm -rf $(OBJDIR) bin/build_osx bin/build_windows.exe
