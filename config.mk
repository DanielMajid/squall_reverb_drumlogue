##############################################################################
# Configuration for Makefile
#

PROJECT := Squall
PROJECT_TYPE := revfx

##############################################################################
# Sources
#

# C sources
CSRC = src/header.c

# C++ sources
CXXSRC = src/unit.cc src/clouds_reverb.cc

# List ASM source files here
ASMSRC =

ASMXSRC =

# Mutable Instruments Eurorack source path
EURORACK_PATH ?= $(PROJECT_ROOT)/eurorack

##############################################################################
# Include Paths
#

UINCDIR = \
  $(PROJECT_ROOT)/src \
  $(EURORACK_PATH) \
  $(EURORACK_PATH)/clouds/dsp \
  $(EURORACK_PATH)/clouds/dsp/fx

##############################################################################
# Library Paths
#

ULIBDIR =

##############################################################################
# Libraries
#

ULIBS  = -lm
ULIBS += -lc

##############################################################################
# Macros
#

UDEFS =
