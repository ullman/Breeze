# Add inputs and outputs from these tool invocations to the build variables


OS_NAME := $(shell $(UNAME))


#ifeq ($(origin BUILD_CONFIG), undefined)
BUILD_CONFIG ?= Debug
#endif

#ifeq ($(origin ARCH), undefined)
ARCH ?= i386
#endif

#ifeq ($(origin PROJPATH), undefined)
PROJPATH ?= .
#endif


#ifeq ($(origin PROJ_PATH), undefined)
PROJ_PATH ?= $(PROJPATH)
#endif

#ifeq ($(strip $(OUTPUT_DIR)),)
OUTPUT_DIR ?= $(PROJ_PATH)/$(BUILD_CONFIG)
#endif

#ifeq ($(strip $(BUILD_ARCH)),)
BUILD_ARCH ?= $(ARCH)
#endif

#ifeq ($(strip $(ENVENTOR_PATH)),)
ENVENTOR_PATH ?= $(SDK_TOOLPATH)/enventor
#endif
