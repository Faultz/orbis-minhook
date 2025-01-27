TOOLCHAIN    := $(OO_PS4_TOOLCHAIN)

PROJ_DIR     := source
BUILD_DIR    := build
OBJS_DIR     := $(BUILD_DIR)/objects

LIBNAME      := libMinhook
TARGETMODULE := $(LIBNAME).prx
TARGETSTATIC := $(BUILD_DIR)/$(LIBNAME).a

LIBS         := -lc -lc++ -lz -lkernel -lSceLibcInternal 

CFILES       := $(wildcard $(PROJ_DIR)/*.c) \
                $(wildcard $(PROJ_DIR)/hde/*.c)
CPPFILES     := $(wildcard $(PROJ_DIR)/*.cpp) \
                $(wildcard $(PROJ_DIR)/hde/*.cpp)
OBJS         := $(patsubst $(PROJ_DIR)/%.c, $(OBJS_DIR)/%.o, $(CFILES)) \
                $(patsubst $(PROJ_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(CPPFILES))

STUBOBJS     := $(patsubst $(PROJ_DIR)/%.c, $(INTDIR)/%.o, $(CFILES)) \
                $(patsubst $(PROJ_DIR)/%.cpp, $(INTDIR)/%.o.stub, $(CPPFILES))

EXTRAFLAGS   := -Wall -Wno-int-to-pointer-cast -Werror -Wno-for-loop-analysis \
                -fcolor-diagnostics -Iinclude -Wall -D__ORBIS__ -D__PS4__ \
                -Wno-unused-function -Wno-misleading-indentation -Wno-invalid-noreturn

CFLAGS       := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c \
                $(EXTRAFLAGS) -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include \
                -Iinclude -std=c11 -D_DEFAULT_SOURCE -DFMT_HEADER_ONLY

CXXFLAGS     := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1 \
                -std=c++11 -Iinclude -DFMT_HEADER_ONLY

LDFLAGS      := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr \
                -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crtlib.o

$(shell mkdir -p $(OBJS_DIR))

UNAME_S      := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  CC         := clang
  CCX        := clang++
  LD         := ld.lld
  CDIR       := linux
  ASMLR      := as
endif

ifeq ($(UNAME_S),Darwin)
  CC         := /usr/local/opt/llvm/bin/clang
  CCX        := /usr/local/opt/llvm/bin/clang++
  LD         := /usr/local/opt/llvm/bin/ld.lld
  CDIR       := macos
  ASMLR      := as
endif

$(TARGETMODULE): $(OBJS)
	$(LD) $(OBJS) -o $(OBJS_DIR)/$(PROJ_DIR).elf $(LDFLAGS) -e main
	# Explicitly specify the path to output the .prx to $(BUILD_DIR)
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(OBJS_DIR)/$(PROJ_DIR).elf \
	-out=$(BUILD_DIR)/$(TARGETMODULE) --lib=$(TARGETMODULE) --paid 0x3800000000000011

$(OBJS_DIR)/%.o: $(PROJ_DIR)/%.s
	$(shell mkdir -p $(dir $@))
	$(ASMLR) -o $@ $<

$(OBJS_DIR)/%.o: $(PROJ_DIR)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) -o $@ $<

$(OBJS_DIR)/%.o: $(PROJ_DIR)/%.cpp
	$(shell mkdir -p $(dir $@))
	$(CCX) $(CXXFLAGS) -o $@ $<

.PHONY: clean all
.DEFAULT_GOAL := all

$(TARGETSTATIC): $(OBJS) $(AR) rcs $@ $^

all: clean $(TARGETMODULE) $(TARGETSTATIC)

clean:
	rm -f $(TARGETMODULE) $(TARGETSTATIC) \
		  $(OBJS_DIR)/$(PROJ_DIR).elf $(OBJS_DIR)/$(PROJ_DIR).oelf $(OBJS)
		  $(shell rm -rf $(OBJS_DIR))
