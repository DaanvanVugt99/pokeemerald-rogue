TOOLCHAIN := $(DEVKITARM)
COMPARE ?= 0
RELEASE ?= 0
EXPANSION := 1
ifeq ($(OS),Windows_NT)
PYTHON ?= python
else
PYTHON ?= python3
endif

ifeq (compare,$(MAKECMDGOALS))
  COMPARE := 1
endif

# don't use dkP's base_tools anymore
# because the redefinition of $(CC) conflicts
# with when we want to use $(CC) to preprocess files
# thus, manually create the variables for the bin
# files, or use arm-none-eabi binaries on the system
# if dkP is not installed on this system

ifneq (,$(TOOLCHAIN))
ifneq ($(wildcard $(TOOLCHAIN)/bin),)
export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif
endif

PREFIX := arm-none-eabi-
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
AS := $(PREFIX)as

LD := $(PREFIX)ld

ARMCC := $(PREFIX)gcc
PATH_ARMCC := PATH="$(PATH)" $(ARMCC)
GCC_INCLUDE_DIR := $(shell $(PATH_ARMCC) -print-file-name=include)
GCC_FIXED_INCLUDE_DIR := $(shell $(PATH_ARMCC) -print-file-name=include-fixed)
NEWLIB_INCLUDE_DIR := $(abspath $(dir $(shell $(PATH_ARMCC) -print-file-name=libc.a))/../include)
TOOLCHAIN_INCLUDE_DIRS := -I $(GCC_INCLUDE_DIR) -I $(GCC_FIXED_INCLUDE_DIR) -I $(NEWLIB_INCLUDE_DIR)

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

ifeq ($(OS),Windows_NT)
PORYSCRIPT := tools/poryscript/poryscript-windows/poryscript$(EXE)
else ifeq ($(shell uname -s),Darwin)
ifneq ($(wildcard tools/poryscript/poryscript),)
PORYSCRIPT := tools/poryscript/poryscript
else ifneq ($(wildcard tools/poryscript/poryscript-macos/poryscript),)
PORYSCRIPT := tools/poryscript/poryscript-macos/poryscript
else
PORYSCRIPT := poryscript
endif
else
PORYSCRIPT := tools/poryscript/poryscript-linux/poryscript$(EXE)
endif

ROGUEPORYSCRIPTSDIR := data/scripts/Rogue
PORYSCRIPTARGS := -cc tools/poryscript/command_config.json -fc $(ROGUEPORYSCRIPTSDIR)/Strings/poryscript_font_config.json

ifeq ($(EXPANSION), 1)
PORYSCRIPTARGS += -s ROGUE_VERSION=ROGUE_VERSION_EXPANSION
endif

ifeq ($(EXPANSION), 0)
PORYSCRIPTARGS += -s ROGUE_VERSION=ROGUE_VERSION_VANILLA
endif

ifeq ($(RELEASE), 1)
PORYSCRIPTARGS += -s ROGUE_RELEASE=1
BUILD_CONFIG := release
endif

ifeq ($(RELEASE), 0)
PORYSCRIPTARGS += -s ROGUE_DEBUG=1
BUILD_CONFIG := debug
endif

TITLE        := PKMN DIVERGE
GAME_CODE    := BPEE
MAKER_CODE   := 01
REVISION     := 0
TEST         ?= 0
ANALYZE      ?= 0
UNUSED_ERROR ?= 0
# Enables link-time optimization for smaller, more efficient release ROMs.
LTO          ?= 0

ifeq (check,$(MAKECMDGOALS))
  TEST := 1
endif

# The split test harness uses partial linking and does not use the LTO path.
ifeq ($(TEST),1)
override LTO := 0
endif

CPP := $(PREFIX)cpp

IS_WSL := $(findstring microsoft,$(shell uname -r 2>/dev/null | tr '[:upper:]' '[:lower:]'))
ifneq ($(IS_WSL),)
OBJ_BASE_DIR_NAME := build_wsl
else
OBJ_BASE_DIR_NAME := build
endif

ifneq ($(LTO),0)
LTO_SUFFIX := _lto
endif

ROM_NAME := pokeemerald.gba
ELF_NAME := $(ROM_NAME:.gba=.elf)
MAP_NAME := $(ROM_NAME:.gba=.map)
OBJ_DIR_NAME := $(OBJ_BASE_DIR_NAME)/modern_$(BUILD_CONFIG)$(LTO_SUFFIX)
TEST_OBJ_DIR_NAME := $(OBJ_BASE_DIR_NAME)/modern_test

SHELL := /bin/bash
.SHELLFLAGS := -o pipefail -c

ROM := $(ROM_NAME)
OBJ_DIR := $(OBJ_DIR_NAME)
ELF = $(ROM:.gba=.elf)
MAP = $(ROM:.gba=.map)
SYM = $(ROM:.gba=.sym)

TESTELF = $(ROM:.gba=-test.elf)
HEADLESSELF = $(ROM:.gba=-test-headless.elf)

C_SUBDIR = src
GFLIB_SUBDIR = gflib
ASM_SUBDIR = asm
DATA_SRC_SUBDIR = src/data
DATA_ASM_SUBDIR = data
SONG_SUBDIR = sound/songs
MID_SUBDIR = sound/songs/midi
SAMPLE_SUBDIR = sound/direct_sound_samples
CRY_SUBDIR = sound/direct_sound_samples/cries
TEST_SUBDIR = test

C_BUILDDIR = $(OBJ_DIR)/$(C_SUBDIR)
GFLIB_BUILDDIR = $(OBJ_DIR)/$(GFLIB_SUBDIR)
ASM_BUILDDIR = $(OBJ_DIR)/$(ASM_SUBDIR)
DATA_ASM_BUILDDIR = $(OBJ_DIR)/$(DATA_ASM_SUBDIR)
SONG_BUILDDIR = $(OBJ_DIR)/$(SONG_SUBDIR)
MID_BUILDDIR = $(OBJ_DIR)/$(MID_SUBDIR)
TEST_BUILDDIR = $(OBJ_DIR)/$(TEST_SUBDIR)

ASFLAGS := -mcpu=arm7tdmi -march=armv4t -meabi=5 --defsym MODERN=1

ifeq ($(EXPANSION), 1)
ASFLAGS += --defsym ROGUE_EXPANSION=1
endif

ifeq ($(RELEASE), 1)
ASFLAGS += --defsym ROGUE_RELEASE=1
endif

ifeq ($(RELEASE), 0)
ASFLAGS += --defsym ROGUE_DEBUG=1
endif

CC1              = $(shell $(PATH_ARMCC) --print-prog-name=cc1) -quiet
ifeq ($(TEST),1)
override CFLAGS += -O2
else ifeq ($(RELEASE),0)
override CFLAGS += -Og
else
override CFLAGS += -O2
endif
override CFLAGS += -gdwarf-4 -gstrict-dwarf -mthumb -mthumb-interwork -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -Wno-pointer-to-int-cast -std=gnu17 -Werror -Wall -Wno-strict-aliasing -Wno-attribute-alias -Woverride-init
ifneq ($(LTO),0)
override CFLAGS += -flto=auto -fno-fat-lto-objects -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections
else
override CFLAGS += -fno-toplevel-reorder
endif
ifeq ($(ANALYZE),1)
override CFLAGS += -fanalyzer
endif
# Only throw an error for unused elements if its RH-Hideout's repo
ifeq ($(UNUSED_ERROR),0)
#ifneq ($(GITHUB_REPOSITORY_OWNER),rh-hideout)
override CFLAGS += -Wno-error=unused-variable -Wno-error=unused-const-variable -Wno-error=unused-parameter -Wno-error=unused-function -Wno-error=unused-but-set-parameter -Wno-error=unused-but-set-variable -Wno-error=unused-value -Wno-error=unused-local-typedefs
#endif
endif
LIBPATH := -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libgcc.a))" -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libnosys.a))" -L "$(dir $(shell $(PATH_ARMCC) -mthumb -print-file-name=libc.a))"
LIB := $(LIBPATH) -lc -lnosys -lgcc -L../../libagbsyscall -lagbsyscall

ifeq ($(TESTELF),$(MAKECMDGOALS))
  TEST := 1
endif

ifeq ($(TEST),1)
OBJ_DIR := $(TEST_OBJ_DIR_NAME)
endif

CPPFLAGS := -iquote include -iquote $(GFLIB_SUBDIR) -Wno-trigraphs -DMODERN=1 -DTESTING=$(TEST) -std=gnu17

ifeq ($(EXPANSION), 1)
CPPFLAGS += -D ROGUE_EXPANSION=1
endif

ifeq ($(RELEASE), 1)
CPPFLAGS += -D ROGUE_RELEASE=1
endif

ifeq ($(RELEASE), 0)
CPPFLAGS += -D ROGUE_DEBUG=1
endif

SHA1 := $(shell { command -v sha1sum || command -v shasum; } 2>/dev/null) -c
GFX := tools/gbagfx/gbagfx$(EXE)
AIF := tools/aif2pcm/aif2pcm$(EXE)
MID := tools/mid2agb/mid2agb$(EXE)
SCANINC := tools/scaninc/scaninc$(EXE)
PREPROC := tools/preproc/preproc$(EXE)
FIX := tools/gbafix/gbafix$(EXE)
MAPJSON := tools/mapjson/mapjson$(EXE)
JSONPROC := tools/jsonproc/jsonproc$(EXE)
PATCHELF := tools/patchelf/patchelf$(EXE)
ifeq ($(shell uname -s),Darwin)
ROMTEST ?= $(shell command -v mgba-rom-test 2>/dev/null)
else
ROMTEST ?= $(shell { command -v mgba-rom-test || command -v tools/mgba/mgba-rom-test$(EXE); } 2>/dev/null)
endif
ROMTESTHYDRA := tools/mgba-rom-test-hydra/mgba-rom-test-hydra$(EXE)
MEMORYSTATS := tools/Pokabbie/Build/MemoryStats/memorystats$(EXE)
CUSTOMJSON := tools/Pokabbie/Build/CustomJson/customjson$(EXE)

PERL := perl

# Inclusive list. If you don't want a tool to be built, don't add it here.
TOOLDIRS := tools/aif2pcm tools/bin2c tools/gbafix tools/gbagfx tools/jsonproc tools/mapjson tools/mid2agb tools/preproc tools/rsfont tools/scaninc
CHECKTOOLDIRS = tools/patchelf tools/mgba-rom-test-hydra
TOOLBASE = $(TOOLDIRS:tools/%=%)
TOOLS = $(foreach tool,$(TOOLBASE),tools/$(tool)/$(tool)$(EXE))

MAKEFLAGS += --no-print-directory

# Clear the default suffixes
.SUFFIXES:
# Don't delete intermediate files
.SECONDARY:
# Delete files that weren't built properly
.DELETE_ON_ERROR:

# Secondary expansion is required for dependency variables in object rules.
.SECONDEXPANSION:

.PHONY: all rom clean compare tidy tools check-tools mostlyclean clean-tools clean-check-tools $(TOOLDIRS) $(CHECKTOOLDIRS) libagbsyscall modern tidymodern check species-report check-species-report rogue-bake-data check-rogue-bake-data FORCE

infoshell = $(foreach line, $(shell $1 | sed "s/ /__SPACE__/g"), $(info $(subst __SPACE__, ,$(line))))

# Build tools when building the rom
# Disable dependency scanning for clean/tidy/tools
# Use a separate minimal makefile for speed
# Since we don't need to reload most of this makefile
# Direct output targets are used by the documented device-build command. Keep
# dependency scanning enabled for them so header layout changes cannot leave a
# ROM linked from ABI-incompatible object files.
ifeq (,$(filter-out all rom compare modern check libagbsyscall syms $(ROM) $(ELF) $(TESTELF),$(MAKECMDGOALS)))
$(call infoshell, $(MAKE) -f make_tools.mk)
else
NODEP ?= 1
endif

# check if we need to scan dependencies based on the rule
ifeq (,$(MAKECMDGOALS))
  SCAN_DEPS ?= 1
else
  # clean, tidy, tools, check-tools, mostlyclean, clean-tools, clean-check-tools, $(TOOLDIRS), $(CHECKTOOLDIRS), tidymodern, tidycheck don't even build the ROM
  # libagbsyscall does its own thing
  ifeq (,$(filter-out clean tidy tools mostlyclean clean-tools $(TOOLDIRS) clean-check-tools $(CHECKTOOLDIRS) tidymodern tidycheck libagbsyscall species-report check-species-report,$(MAKECMDGOALS)))
    SCAN_DEPS ?= 0
  else
    SCAN_DEPS ?= 1
  endif
endif

ifeq ($(SCAN_DEPS),1)
C_SRCS_IN := $(wildcard $(C_SUBDIR)/*.c $(C_SUBDIR)/*/*.c $(C_SUBDIR)/*/*/*.c)
C_SRCS := $(foreach src,$(C_SRCS_IN),$(if $(findstring .inc.c,$(src)),,$(src)))
C_SRCS := $(filter-out \
	$(C_SUBDIR)/berry_fix_graphics.c \
	$(C_SUBDIR)/berry_fix_program.c \
	$(C_SUBDIR)/ereader_helpers.c \
	$(C_SUBDIR)/ereader_screen.c \
	$(C_SUBDIR)/mystery_event_menu.c \
	$(C_SUBDIR)/mystery_event_msg.c \
	$(C_SUBDIR)/mystery_event_script.c \
	$(C_SUBDIR)/mystery_gift.c \
	$(C_SUBDIR)/mystery_gift_client.c \
	$(C_SUBDIR)/mystery_gift_link.c \
	$(C_SUBDIR)/mystery_gift_menu.c \
	$(C_SUBDIR)/mystery_gift_scripts.c \
	$(C_SUBDIR)/mystery_gift_server.c \
	$(C_SUBDIR)/mystery_gift_view.c \
	$(C_SUBDIR)/wonder_news.c, \
	$(C_SRCS))
C_OBJS := $(patsubst $(C_SUBDIR)/%.c,$(C_BUILDDIR)/%.o,$(C_SRCS))

# Test sources are only needed for test ELF targets. Keep their source and
# dependency scans out of normal ROM builds, where they add no prerequisites.
ifeq ($(TEST),1)
TEST_SRCS_IN := $(wildcard $(TEST_SUBDIR)/*.c $(TEST_SUBDIR)/*/*.c $(TEST_SUBDIR)/*/*/*.c $(TEST_SUBDIR)/*/*/*/*.c)
TEST_SRCS_ALL := $(foreach src,$(TEST_SRCS_IN),$(if $(findstring .inc.c,$(src)),,$(src)))
TEST_HARNESS_SRCS := $(TEST_SUBDIR)/test_runner.c $(TEST_SUBDIR)/test_runner_args.c $(TEST_SUBDIR)/test_runner_battle.c
TEST_CASE_SRCS := $(filter-out $(TEST_HARNESS_SRCS),$(TEST_SRCS_ALL))
TEST_SUITE ?=
ifneq ($(strip $(TEST_SUITE)),)
ifeq ($(TEST_SUITE),ability)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/ability/%,$(TEST_CASE_SRCS))
TEST_CASE_SRCS := $(filter-out $(TEST_SUBDIR)/battle/ability/unique/%,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),ability_unique)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/ability/unique/%,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),moves)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/move.c $(TEST_SUBDIR)/battle/move_effect/% $(TEST_SUBDIR)/battle/move_flags/% $(TEST_SUBDIR)/battle/status1/% $(TEST_SUBDIR)/battle/terrain/% $(TEST_SUBDIR)/battle/weather/% $(TEST_SUBDIR)/battle/type_effectiveness_messages.c $(TEST_SUBDIR)/battle/crit_chance.c $(TEST_SUBDIR)/battle/damage_formula.c,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),items)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/hold_effect/% $(TEST_SUBDIR)/battle/item_effect/%,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),forms)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/form_change/% $(TEST_SUBDIR)/battle/gimmick/% $(TEST_SUBDIR)/species.c,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),rogue)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/rogue/% $(TEST_SUBDIR)/rogue_%.c,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),ai)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/battle/ai%.c $(TEST_SUBDIR)/battle/trainer_control.c,$(TEST_CASE_SRCS))
else ifeq ($(TEST_SUITE),core)
TEST_CASE_SRCS := $(filter $(TEST_SUBDIR)/fpmath.c $(TEST_SUBDIR)/random.c $(TEST_SUBDIR)/sprite.c $(TEST_SUBDIR)/battle/exp.c,$(TEST_CASE_SRCS))
else
$(error Unknown TEST_SUITE '$(TEST_SUITE)'. Expected one of: ability ability_unique moves items forms rogue ai core)
endif
endif
# TESTS is also used at runtime as a prefix filter. For focused runs, compile
# only source files containing that literal string so the test ELF stays small.
TEST_COMPILE_FILTER ?= $(TESTS)
ifneq ($(strip $(TEST_COMPILE_FILTER)),)
TEST_FILTERED_CASE_SRCS := $(shell find $(TEST_SUBDIR) -type f -name '*.c' ! -name 'test_runner*.c' -exec grep -lF -- "$(TEST_COMPILE_FILTER)" {} +)
TEST_CASE_SRCS := $(filter $(TEST_FILTERED_CASE_SRCS),$(TEST_CASE_SRCS))
endif
TEST_SRCS := $(TEST_HARNESS_SRCS) $(TEST_CASE_SRCS)
TEST_OBJS := $(patsubst $(TEST_SUBDIR)/%.c,$(TEST_BUILDDIR)/%.o,$(TEST_SRCS))
TEST_OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(TEST_OBJS))
endif

GFLIB_SRCS := $(wildcard $(GFLIB_SUBDIR)/*.c)
GFLIB_OBJS := $(patsubst $(GFLIB_SUBDIR)/%.c,$(GFLIB_BUILDDIR)/%.o,$(GFLIB_SRCS))

C_ASM_SRCS += $(wildcard $(C_SUBDIR)/*.s $(C_SUBDIR)/*/*.s $(C_SUBDIR)/*/*/*.s)
C_ASM_OBJS := $(patsubst $(C_SUBDIR)/%.s,$(C_BUILDDIR)/%.o,$(C_ASM_SRCS))

ASM_SRCS := $(wildcard $(ASM_SUBDIR)/*.s)
ASM_OBJS := $(patsubst $(ASM_SUBDIR)/%.s,$(ASM_BUILDDIR)/%.o,$(ASM_SRCS))

# get all the data/*.s files EXCEPT the ones with specific rules
REGULAR_DATA_ASM_SRCS := $(filter-out \
	$(DATA_ASM_SUBDIR)/maps.s \
	$(DATA_ASM_SUBDIR)/map_events.s \
	$(DATA_ASM_SUBDIR)/multiboot_berry_glitch_fix.s \
	$(DATA_ASM_SUBDIR)/multiboot_ereader.s \
	$(DATA_ASM_SUBDIR)/mystery_event_script_cmd_table.s \
	$(DATA_ASM_SUBDIR)/mystery_gift.s, \
	$(wildcard $(DATA_ASM_SUBDIR)/*.s))

DATA_ASM_SRCS := $(REGULAR_DATA_ASM_SRCS) $(DATA_ASM_SUBDIR)/maps.s $(DATA_ASM_SUBDIR)/map_events.s
DATA_ASM_OBJS := $(patsubst $(DATA_ASM_SUBDIR)/%.s,$(DATA_ASM_BUILDDIR)/%.o,$(DATA_ASM_SRCS))

SONG_SRCS := $(wildcard $(SONG_SUBDIR)/*.s)
SONG_OBJS := $(patsubst $(SONG_SUBDIR)/%.s,$(SONG_BUILDDIR)/%.o,$(SONG_SRCS))

MID_SRCS := $(wildcard $(MID_SUBDIR)/*.mid)
MID_OBJS := $(patsubst $(MID_SUBDIR)/%.mid,$(MID_BUILDDIR)/%.o,$(MID_SRCS))

OBJS     := $(C_OBJS) $(GFLIB_OBJS) $(C_ASM_OBJS) $(ASM_OBJS) $(DATA_ASM_OBJS) $(SONG_OBJS) $(MID_OBJS)
OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(OBJS))

SUBDIRS  := $(sort $(dir $(OBJS) $(dir $(TEST_OBJS))))
$(shell mkdir -p $(SUBDIRS))
endif

AUTO_GEN_TARGETS :=

all: rom

tools: $(TOOLDIRS)

check-tools: $(CHECKTOOLDIRS)

syms: $(SYM)

$(TOOLDIRS):
	@$(MAKE) -C $@

$(CHECKTOOLDIRS):
	@$(MAKE) -C $@

rom: $(ROM)
ifeq ($(COMPARE),1)
	@$(SHA1) rom.sha1
endif

# For contributors to make sure a change didn't affect the contents of the ROM.
compare: all

clean: mostlyclean clean-tools clean-check-tools

clean-tools:
	@$(foreach tooldir,$(TOOLDIRS),$(MAKE) clean -C $(tooldir);)

clean-check-tools:
	@$(foreach tooldir,$(CHECKTOOLDIRS),$(MAKE) clean -C $(tooldir);)

mostlyclean: tidymodern tidycheck
	find sound -iname '*.bin' -exec rm {} +
	rm -f $(MID_SUBDIR)/*.s
	find . \( -iname '*.1bpp' -o -iname '*.4bpp' -o -iname '*.8bpp' -o -iname '*.gbapal' -o -iname '*.lz' -o -iname '*.rl' -o -iname '*.latfont' -o -iname '*.hwjpnfont' -o -iname '*.fwjpnfont' \) -exec rm {} +
	rm -f $(DATA_ASM_SUBDIR)/layouts/layouts.inc $(DATA_ASM_SUBDIR)/layouts/layouts_table.inc
	rm -f $(DATA_ASM_SUBDIR)/maps/connections.inc $(DATA_ASM_SUBDIR)/maps/events.inc $(DATA_ASM_SUBDIR)/maps/groups.inc $(DATA_ASM_SUBDIR)/maps/headers.inc
	find $(DATA_ASM_SUBDIR)/maps \( -iname 'connections.inc' -o -iname 'events.inc' -o -iname 'header.inc' \) -exec rm {} +
	rm -f $(AUTO_GEN_TARGETS)
	rm -f $(patsubst %.pory,%.inc,$(shell find data/ -type f -name '*.pory'))
	@$(MAKE) clean -C libagbsyscall

tidy: tidymodern tidycheck

tidymodern:
	rm -f $(ROM_NAME) $(ELF_NAME) $(MAP_NAME)
	rm -rf $(OBJ_BASE_DIR_NAME)/modern_*

tidycheck:
	rm -f $(TESTELF) $(HEADLESSELF)
	rm -rf $(TEST_OBJ_DIR_NAME)

species-report:
	$(PYTHON) scripts/generate_species_change_report.py
	scripts/generate_rogue_bake_data.sh

check-species-report:
	$(PYTHON) scripts/generate_species_change_report.py --check
	scripts/generate_rogue_bake_data.sh --check

rogue-bake-data:
	scripts/generate_rogue_bake_data.sh

check-rogue-bake-data:
	scripts/generate_rogue_bake_data.sh --check

$(C_BUILDDIR)/berry_crush.o: override CFLAGS += -Wno-address-of-packed-member

# This table is included directly by item.c. Keep it as an explicit prerequisite
# even for NODEP builds, where scaninc dependency discovery is disabled.
$(C_BUILDDIR)/item.o: $(DATA_SRC_SUBDIR)/rogue_items.h
$(C_BUILDDIR)/credits.o: $(DATA_SRC_SUBDIR)/credits.h

# Adventure Paths assets are pulled in through nested headers and generated map
# includes. Keep them explicit so regenerating the tileset cannot leave a ROM
# linked against stale INCBIN data (especially in NODEP builds).
ADVENTURE_PATHS_TILESET_ASSETS := \
	data/tilesets/secondary/adventure_paths/tiles.4bpp.lz \
	data/tilesets/secondary/adventure_paths/palettes/06.gbapal \
	data/tilesets/secondary/adventure_paths/palettes/07.gbapal \
	data/tilesets/secondary/adventure_paths/palettes/08.gbapal \
	data/tilesets/secondary/adventure_paths/metatiles.bin \
	data/tilesets/secondary/adventure_paths/metatile_attributes.bin

ADVENTURE_PATHS_ANIM_ASSETS := \
	data/tilesets/secondary/adventure_paths/anim/stars/0.4bpp \
	data/tilesets/secondary/adventure_paths/anim/stars/1.4bpp \
	data/tilesets/secondary/adventure_paths/anim/stars/2.4bpp \
	data/tilesets/secondary/adventure_paths/anim/stars/3.4bpp

ADVENTURE_PATHS_MAP_ASSETS := \
	data/layouts/Rogue_AdventurePaths/map.bin \
	data/layouts/Rogue_AdventurePaths/border.bin \
	data/layouts/Rogue_Encounter_ItemRoom/map.bin \
	data/layouts/Rogue_Encounter_ItemRoom/border.bin

$(C_BUILDDIR)/tilesets.o: $(ADVENTURE_PATHS_TILESET_ASSETS)
$(C_BUILDDIR)/tileset_anims.o: $(ADVENTURE_PATHS_ANIM_ASSETS)
$(DATA_ASM_BUILDDIR)/maps.o: $(ADVENTURE_PATHS_MAP_ASSETS)

include $(ROGUEPORYSCRIPTSDIR)/rogue_poryscripts.mk
include graphics_file_rules.mk
include map_data_rules.mk
include spritesheet_rules.mk
include json_data_rules.mk
include songs.mk
include $(DATA_SRC_SUBDIR)/rogue/custom_json_rules.mk
include $(OBJEVENTGFXDIR)/pokemon_ow/include/spritesheet_rules_gen.mk

%.s: ;
%.png: ;
%.pal: ;
%.aif: ;
%.pory: ;

%.1bpp: %.png  ; $(GFX) $< $@
%.4bpp: %.png  ; $(GFX) $< $@
%.8bpp: %.png  ; $(GFX) $< $@
%.gbapal: %.pal ; $(GFX) $< $@
%.gbapal: %.png ; $(GFX) $< $@
%.lz: % ; $(GFX) $< $@
%.rl: % ; $(GFX) $< $@

$(CRY_SUBDIR)/uncomp_%.bin: $(CRY_SUBDIR)/uncomp_%.aif ; $(AIF) $< $@
$(CRY_SUBDIR)/%.bin: $(CRY_SUBDIR)/%.aif ; $(AIF) $< $@ --compress
sound/%.bin: sound/%.aif ; $(AIF) $< $@
data/%.inc: data/%.pory; $(PORYSCRIPT) -i $< -o $@ $(PORYSCRIPTARGS)

MAP_SCRIPT_PORYS := $(wildcard data/maps/*/scripts.pory)
MAP_SCRIPT_INCS := $(patsubst %.pory,%.inc,$(MAP_SCRIPT_PORYS))
$(DATA_ASM_BUILDDIR)/event_scripts.o: $(MAP_SCRIPT_INCS)


$(C_BUILDDIR)/librfu_intr.o: CFLAGS := -mthumb-interwork -O2 -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -fno-toplevel-reorder -Wno-pointer-to-int-cast
$(C_BUILDDIR)/pokedex_plus_hgss.o: CFLAGS := -mthumb -mthumb-interwork -O2 -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -Wno-pointer-to-int-cast -std=gnu17 -Werror -Wall -Wno-strict-aliasing -Wno-attribute-alias -Woverride-init

#ifeq ($(DINFO),1)
override CFLAGS += -g
#endif

# The dep rules have to be explicit or else missing files won't be reported.
# As a side effect, they're evaluated immediately instead of when the rule is invoked.
# It doesn't look like $(shell) can be deferred so there might not be a better way.

ifeq ($(SCAN_DEPS),1)
ifeq ($(NODEP),1)
$(C_BUILDDIR)/%.o: $(C_SUBDIR)/%.c
ifeq (,$(KEEP_TEMPS))
	@echo "$(CC1) <flags> -o $@ $<"
	@$(CPP) $(CPPFLAGS) $< | $(PREPROC) $< charmap.txt -i | $(CC1) $(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $(AS) $(ASFLAGS) -o $@ -
else
	@$(CPP) $(CPPFLAGS) $< -o $(C_BUILDDIR)/$*.i
	@$(PREPROC) $(C_BUILDDIR)/$*.i charmap.txt | $(CC1) $(CFLAGS) -o $(C_BUILDDIR)/$*.s
	@echo -e ".text\n\t.align\t2, 0\n" >> $(C_BUILDDIR)/$*.s
	$(AS) $(ASFLAGS) -o $@ $(C_BUILDDIR)/$*.s
endif
else
define C_DEP
$1: $2 $$(shell $(SCANINC) -I include -I gflib $(TOOLCHAIN_INCLUDE_DIRS) $2)
ifeq (,$$(KEEP_TEMPS))
	@echo "$$(CC1) <flags> -o $$@ $$<"
	@$$(CPP) $$(CPPFLAGS) $$< | $$(PREPROC) $$< charmap.txt -i | $$(CC1) $$(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $$(AS) $$(ASFLAGS) -o $$@ -
else
	@$$(CPP) $$(CPPFLAGS) $$< -o $$(C_BUILDDIR)/$3.i
	@$$(PREPROC) $$(C_BUILDDIR)/$3.i charmap.txt | $$(CC1) $$(CFLAGS) -o $$(C_BUILDDIR)/$3.s
	@echo -e ".text\n\t.align\t2, 0\n" >> $$(C_BUILDDIR)/$3.s
	$$(AS) $$(ASFLAGS) -o $$@ $$(C_BUILDDIR)/$3.s
endif
endef
$(foreach src, $(C_SRCS), $(eval $(call C_DEP,$(patsubst $(C_SUBDIR)/%.c,$(C_BUILDDIR)/%.o,$(src)),$(src),$(patsubst $(C_SUBDIR)/%.c,%,$(src)))))
endif

ifeq ($(NODEP),1)
$(GFLIB_BUILDDIR)/%.o: $(GFLIB_SUBDIR)/%.c $$(c_dep)
ifeq (,$(KEEP_TEMPS))
	@echo "$(CC1) <flags> -o $@ $<"
	@$(CPP) $(CPPFLAGS) $< | $(PREPROC) $< charmap.txt -i | $(CC1) $(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $(AS) $(ASFLAGS) -o $@ -
else
	@$(CPP) $(CPPFLAGS) $< -o $(GFLIB_BUILDDIR)/$*.i
	@$(PREPROC) $(GFLIB_BUILDDIR)/$*.i charmap.txt | $(CC1) $(CFLAGS) -o $(GFLIB_BUILDDIR)/$*.s
	@echo -e ".text\n\t.align\t2, 0\n" >> $(GFLIB_BUILDDIR)/$*.s
	$(AS) $(ASFLAGS) -o $@ $(GFLIB_BUILDDIR)/$*.s
endif
else
define GFLIB_DEP
$1: $2 $$(shell $(SCANINC) -I include -I gflib $(TOOLCHAIN_INCLUDE_DIRS) $2)
ifeq (,$$(KEEP_TEMPS))
	@echo "$$(CC1) <flags> -o $$@ $$<"
	@$$(CPP) $$(CPPFLAGS) $$< | $$(PREPROC) $$< charmap.txt -i | $$(CC1) $$(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $$(AS) $$(ASFLAGS) -o $$@ -
else
	@$$(CPP) $$(CPPFLAGS) $$< -o $$(GFLIB_BUILDDIR)/$3.i
	@$$(PREPROC) $$(GFLIB_BUILDDIR)/$3.i charmap.txt | $$(CC1) $$(CFLAGS) -o $$(GFLIB_BUILDDIR)/$3.s
	@echo -e ".text\n\t.align\t2, 0\n" >> $$(GFLIB_BUILDDIR)/$3.s
	$$(AS) $$(ASFLAGS) -o $$@ $$(GFLIB_BUILDDIR)/$3.s
endif
endef
$(foreach src, $(GFLIB_SRCS), $(eval $(call GFLIB_DEP,$(patsubst $(GFLIB_SUBDIR)/%.c,$(GFLIB_BUILDDIR)/%.o, $(src)),$(src),$(patsubst $(GFLIB_SUBDIR)/%.c,%, $(src)))))
endif

ifeq ($(NODEP),1)
$(C_BUILDDIR)/%.o: $(C_SUBDIR)/%.s
	$(PREPROC) $< charmap.txt | $(CPP) -I include - | $(AS) $(ASFLAGS) -o $@
else
define SRC_ASM_DATA_DEP
$1: $2 $$(shell $(SCANINC) -I include -I "" $2)
	$$(PREPROC) $$< charmap.txt | $$(CPP) -I include - | $$(AS) $$(ASFLAGS) -o $$@
endef
$(foreach src, $(C_ASM_SRCS), $(eval $(call SRC_ASM_DATA_DEP,$(patsubst $(C_SUBDIR)/%.s,$(C_BUILDDIR)/%.o, $(src)),$(src))))
endif

ifeq ($(NODEP),1)
$(ASM_BUILDDIR)/%.o: $(ASM_SUBDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $<
else
define ASM_DEP
$1: $2 $$(shell $(SCANINC) -I include -I "" $2)
	$$(AS) $$(ASFLAGS) -o $$@ $$<
endef
$(foreach src, $(ASM_SRCS), $(eval $(call ASM_DEP,$(patsubst $(ASM_SUBDIR)/%.s,$(ASM_BUILDDIR)/%.o, $(src)),$(src))))
endif

ifeq ($(NODEP),1)
$(DATA_ASM_BUILDDIR)/%.o: $(DATA_ASM_SUBDIR)/%.s
	$(PREPROC) $< charmap.txt | $(CPP) -I include - | $(AS) $(ASFLAGS) -o $@
else
$(foreach src, $(REGULAR_DATA_ASM_SRCS), $(eval $(call SRC_ASM_DATA_DEP,$(patsubst $(DATA_ASM_SUBDIR)/%.s,$(DATA_ASM_BUILDDIR)/%.o, $(src)),$(src))))
endif
endif

$(SONG_BUILDDIR)/%.o: $(SONG_SUBDIR)/%.s
	$(AS) $(ASFLAGS) -I sound -o $@ $<

# NOTE: Based on C_DEP above, but without NODEP and KEEP_TEMPS handling.
ifeq ($(TEST),1)
define TEST_DEP
$1: $2 $$(shell $(SCANINC) -I include -I gflib $(TOOLCHAIN_INCLUDE_DIRS) $2)
	@echo "$$(CC1) <flags> -o $$@ $$<"
	@$$(CPP) $$(CPPFLAGS) $$< | $$(PREPROC) $$< charmap.txt -i | $$(CC1) $$(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $$(AS) $$(ASFLAGS) -o $$@ -
endef
$(foreach src, $(TEST_SRCS), $(eval $(call TEST_DEP,$(patsubst $(TEST_SUBDIR)/%.c,$(TEST_BUILDDIR)/%.o,$(src)),$(src),$(patsubst $(TEST_SUBDIR)/%.c,%,$(src)))))
endif

ifneq ($(LTO),0)
LD_SCRIPT := ld_script_lto.ld
else
LD_SCRIPT := ld_script_modern.ld
endif

$(OBJ_DIR)/ld_script.ld: $(LD_SCRIPT)
	cd $(OBJ_DIR) && sed "s#tools/#../../tools/#g" ../../$(LD_SCRIPT) > ld_script.ld

ifneq ($(LTO),0)
ROM_LDFLAGS := -march=armv4t -mabi=apcs-gnu -mcpu=arm7tdmi -flto=auto
ROM_LDFLAGS += -Xlinker -Map=../../$(MAP) -Xlinker --print-memory-usage -Xlinker --gc-sections
ROM_LDFLAGS += -Xassembler -meabi=5 -Xassembler -march=armv4t -Xassembler -mcpu=arm7tdmi
$(ELF): $(OBJ_DIR)/ld_script.ld $(OBJS) libagbsyscall
	@echo "cd $(OBJ_DIR) && $(ARMCC) $(ROM_LDFLAGS) -T ld_script.ld -o ../../$@ <objects> <lib>"
	+@cd $(OBJ_DIR) && $(ARMCC) $(ROM_LDFLAGS) -T ld_script.ld -o ../../$@ $(OBJS_REL) $(LIB)
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent
else
ROM_LDFLAGS := -Map ../../$(MAP)
$(ELF): $(OBJ_DIR)/ld_script.ld $(OBJS) libagbsyscall
	@echo "cd $(OBJ_DIR) && $(LD) $(ROM_LDFLAGS) -T ld_script.ld -o ../../$@ <objects> <lib>"
	@cd $(OBJ_DIR) && $(LD) $(ROM_LDFLAGS) -T ld_script.ld --print-memory-usage -o ../../$@ $(OBJS_REL) $(LIB) | cat
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent
endif

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(FIX) $@ -p --silent
	@echo "ROM size (bytes):" $$(wc -c < $(ROM) | tr -d ' ')
	@echo $(MEMORYSTATS) -F $(MAP_NAME)

modern: all

LD_SCRIPT_TEST := ld_script_test.ld
PARTIAL_TEST_OBJ := test/partial_test_objs.o

# Work around command-line length limit
# On Msys2 the max command line is 32 000 characters.  --via option?
$(OBJ_DIR)/$(PARTIAL_TEST_OBJ): $(TEST_OBJS) FORCE
	@echo "$(LD) -r -o $(OBJ_DIR)/$(PARTIAL_TEST_OBJ) <test objects>"
	@$(LD) -r -o $(OBJ_DIR)/$(PARTIAL_TEST_OBJ) $(TEST_OBJS)

$(OBJ_DIR)/ld_script_test.ld: $(LD_SCRIPT_TEST)
	cd $(OBJ_DIR) && sed "s#tools/#../../tools/#g" ../../$(LD_SCRIPT_TEST) > ld_script_test.ld

$(TESTELF): $(OBJ_DIR)/ld_script_test.ld $(OBJS) $(OBJ_DIR)/$(PARTIAL_TEST_OBJ) libagbsyscall tools check-tools
	@echo "cd $(OBJ_DIR) && $(LD) -T ld_script_test.ld -o ../../$@ <objects> $(PARTIAL_TEST_OBJ) <lib>"
	@cd $(OBJ_DIR) && $(LD) $(TESTLDFLAGS) -T ld_script_test.ld -o ../../$@ $(OBJS_REL) $(PARTIAL_TEST_OBJ) $(LIB)
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) -d0 --silent
	$(PATCHELF) $(TESTELF) gTestRunnerArgv "$(TESTS)\0"

ifeq ($(GITHUB_REPOSITORY_OWNER),rh-hideout)
TEST_SKIP_IS_FAIL := \x01
else
TEST_SKIP_IS_FAIL := \x00
endif

check: $(TESTELF)
	@if [ -z "$(ROMTEST)" ]; then \
		echo "Error: mgba-rom-test not found in PATH."; \
		echo "Install it or run an interactive test build instead (./scripts/launch_build_test.sh --ui)."; \
		exit 127; \
	fi
	@cp $< $(HEADLESSELF)
	$(PATCHELF) $(HEADLESSELF) gTestRunnerHeadless '\x01' gTestRunnerSkipIsFail "$(TEST_SKIP_IS_FAIL)"
	$(ROMTESTHYDRA) $(ROMTEST) $(OBJCOPY) $(HEADLESSELF)

libagbsyscall:
	@$(MAKE) -C libagbsyscall TOOLCHAIN=$(TOOLCHAIN)

###################
### Symbol file ###
###################

$(SYM): $(ELF)
	$(OBJDUMP) -t $< | sort -u | grep -E "^0[2389]" | $(PERL) -p -e 's/^(\w{8}) (\w).{6} \S+\t(\w{8}) (\S+)$$/\1 \2 \3 \4/g' > $@
