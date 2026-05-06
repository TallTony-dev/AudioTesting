#Full llm use sorry
# ── Toolchain ────────────────────────────────────────────────
CXX       := clang++
CXXSTD    := -std=c++23
CXXFLAGS  := -fPIC -Wall -Wextra $(CXXSTD)

# ── Directories ─────────────────────────────────────────────
SRCDIR    := .
OBJDIR    := obj
BINDIR    := bin

# ── Detect OS & set shared-library extension / flags ────────
ifeq ($(OS),Windows_NT)                  # Windows
	SHARED_EXT  := .dll
	LDFLAGS     := -shared
	LDLIBS      := ../../../bin/libAudioTesting.a
	MKDIR       = mkdir $(subst /,\,$1) 2> NUL || echo "" > NUL
	RMDIR       = rmdir /s /q $(subst /,\,$1) 2> NUL || echo "" > NUL
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)                 # macOS
		SHARED_EXT  := .dylib
		LDFLAGS     := -dynamiclib -undefined dynamic_lookup
	else                                      # Linux / other
		SHARED_EXT  := .so
		LDFLAGS     := -shared
	endif
	LDLIBS      :=
	MKDIR       = mkdir -p $1
	RMDIR       = rm -rf $1
endif

PLUGIN_NAME := template
TARGET      := $(BINDIR)/$(PLUGIN_NAME)$(SHARED_EXT)

# ── Sources & objects ────────────────────────────────────────
# Recursively find every .cpp under this directory
CPPFILES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS  := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))

# ── Include paths (parent plugin headers, raylib, etc.) ─────
INCLUDES := -I.. -I../include

# ── Targets ──────────────────────────────────────────────────
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)