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
	ifeq ($(UNAME_S),Darwin)             # macOS
		SHARED_EXT  := .dylib
		LDFLAGS     := -dynamiclib -undefined dynamic_lookup
	else                                 # Linux / other
		SHARED_EXT  := .so
		LDFLAGS     := -shared
	endif
	LDLIBS      :=
	MKDIR       = mkdir -p $1
	RMDIR       = rm -rf $1
endif

PLUGIN_NAME := ambient1
TARGET      := $(BINDIR)/$(PLUGIN_NAME)$(SHARED_EXT)

# ── Sources & objects ────────────────────────────────────────
# Find every .cpp in SRCDIR
CPPFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))

# ── Include paths (parent plugin headers, raylib, etc.) ─────
INCLUDES := -I.. -I../include

# ── Targets ──────────────────────────────────────────────────
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BINDIR):
	@$(call MKDIR,$(BINDIR))

$(OBJDIR):
	@$(call MKDIR,$(OBJDIR))

clean:
	@$(call RMDIR,$(OBJDIR))
	@$(call RMDIR,$(BINDIR))