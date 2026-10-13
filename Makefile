# StonePSP360 v8.7b — libXenon cloud build
.SUFFIXES:

ifeq ($(strip $(DEVKITXENON)),)
$(error "DEVKITXENON is not set; configure the Free60 libXenon toolchain")
endif

include $(DEVKITXENON)/rules

TARGET := StonePSP360
BUILD := build
SOURCES := source core/psp
DATA :=
INCLUDES := core/psp

CFLAGS := -g -O2 -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS := $(CFLAGS)
LDFLAGS := -g $(MACHDEP) -Wl,--gc-sections -Wl,-Map,$(notdir $@).map
LIBS := -lxenon -lm
LIBDIRS :=

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export PROJECTDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(PROJECTDIR)/$(dir))
export DEPSDIR := $(PROJECTDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(PROJECTDIR)/$(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(PROJECTDIR)/$(dir)/*.cpp)))
sFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(PROJECTDIR)/$(dir)/*.s)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(PROJECTDIR)/$(dir)/*.S)))
BINFILES :=

ifeq ($(strip $(CPPFILES)),)
export LD := $(CC)
else
export LD := $(CXX)
endif

export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(PROJECTDIR)/$(dir)) -I$(PROJECTDIR)/$(BUILD) -I$(LIBXENON_INC)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib) -L$(LIBXENON_LIB)

.PHONY: all clean
all: $(BUILD) $(OUTPUT).elf32

$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(PROJECTDIR)/Makefile

clean:
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).elf32 $(OUTPUT).elf32.map xenon.elf

else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).elf32: $(OUTPUT).elf
-include $(DEPENDS)
endif
