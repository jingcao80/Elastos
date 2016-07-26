##########################################################################
#
# File:    $XDK/DevKit/misc/makedefs_gnu.mk
# Purpose: Build macros for GNU gcc compiler
#

ifneq "$(XDK_TARGET_PRODUCT)" ""
  C_DEFINES:=$(C_DEFINES) -D_$(XDK_TARGET_PRODUCT)
endif

C_DEFINES:= -DELASTOS -D_$(XDK_TARGET_CPU) -D_$(XDK_TARGET_BOARD) \
            -D_$(XDK_TARGET_PLATFORM) $(C_DEFINES)

NATIVE_BUILD=1

ifeq "$(XDK_TARGET_PRODUCT)" "devtools"
  NATIVE_BUILD=1
endif

ifeq "$(XDK_TARGET_PRODUCT)" "linux"
  NATIVE_BUILD=1
endif

ifeq "$(XDK_TARGET_PLATFORM)" "android"
  C_DEFINES +=-D_linux
endif

ifeq "$(XDK_TARGET_PLATFORM)" "linux"
  C_DEFINES +=-D_linux
endif

ifeq "$(XDK_TARGET_CPU)" "arm"
  ifneq "$(XDK_ENABLE_WMMX)" ""
    C_DEFINES:=$(C_DEFINES) -D_wmmx
  endif
  C_DEFINES:=$(C_DEFINES) -D__stdcall= -D__cdecl= -D__fastcall=
  ifeq "$(XDK_TARGET_FORMAT)" "elf"
    C_DEFINES:=$(C_DEFINES) -D"__declspec(x)="
  else
    C_DEFINES:=$(C_DEFINES) -D"__declspec(x)=__attribute__((x))"
  endif
endif

ifneq "$(GRAFIX_MULTI_PROCESS)" ""
  C_DEFINES := -DGRAFIX_MULTI_PROCESS $(C_DEFINES)
endif

ifneq "$(GRAFIX_PURE_NATIVE)" ""
  C_DEFINES := -DGRAFIX_PURE_NATIVE $(C_DEFINES)
endif

ifneq "$(MULTI_THREAD_SAMPLES_ASSEMBLE)" ""
  C_DEFINES := -DMULTI_THREAD_SAMPLES_ASSEMBLE $(C_DEFINES)
endif

##########################################################################
#
# File extensions
#

OBJ_SUFFIX = .o

##########################################################################
#
# Select C/C++ compiler and assembler
#

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
ASM = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar


DLLTOOL = dlltool

ifndef NOSTDLIB
  LIBC_FLAGS = -Wl,--start-group,-lgcc,-lc,--end-group
endif

ifeq "$(DEPEND_OBJ_TYPE)" ""
  DEPEND_OBJ_TYPE = $(TARGET_TYPE)
  ifeq "$(TARGET_TYPE)" "lib"
    DEPEND_OBJ_TYPE = eco
  endif
endif

ifneq "$(SANDWICH)" ""
LUBE_FLAGS += -w
endif

ifeq "$(XDK_TARGET_CPU)" "arm"
ifneq "$(XDK_TARGET_FORMAT)" "elf"
  DLLTOOL_FLAGS := -C -f -mcpu=arm9 -f -march=armv7-a --no-export-all-symbols \
                   -D $(TARGET_NAME).$(DEPEND_OBJ_TYPE) \
                   -l $(XDK_USER_LIB)/$(TARGET_NAME).lib \
                   $(DLLTOOL_FLAGS)
else

ifeq "$(XDK_TARGET_PLATFORM)" "android"
  ifneq "$(DLLTOOL_FLAGS)" ""
    DLLTOOL_FLAGS := -Wl,-soname,$(strip $(DLLTOOL_FLAGS:-D=))
  endif
  DLLTOOL_FLAGS := -Wl,-soname,$(TARGET_NAME).$(DEPEND_OBJ_TYPE) -lstdc++ $(LIBC_FLAGS) -nostdlib -L$(PREBUILD_LIB) \
                   $(DLLTOOL_FLAGS)
else
  ifneq "$(DLLTOOL_FLAGS)" ""
    DLLTOOL_FLAGS := -Wl,-soname,/usr/com.elastos.runtime/elastos/$(strip $(DLLTOOL_FLAGS:-D=))
  endif
  DLLTOOL_FLAGS := -Wl,-soname,/usr/com.elastos.runtime/elastos/$(TARGET_NAME).$(DEPEND_OBJ_TYPE) \
                   $(DLLTOOL_FLAGS)
endif


endif
else
  DLLTOOL_FLAGS := -D $(TARGET_NAME).$(DEPEND_OBJ_TYPE)  \
                   -l $(XDK_USER_LIB)/$(TARGET_NAME).lib \
                   $(DLLTOOL_FLAGS)
endif

##########################################################################
#
# Linker and its flags
#
LD = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
PASS2LD = -Wl,

ifeq "$(ECX_ENTRY)" ""
  EXE_ENTRY = _start
  ECX_ENTRY = _start
endif

ifeq "$(DLL_ENTRY)" ""
  DLL_ENTRY = _DllMainCRTStartup

  ifeq "$(XDK_TARGET_CPU)" "x86"
    ifneq "$(findstring msvcrt,$(EXTERN_LIBS))" ""
      DLL_ENTRY = DllMainCRTStartup@12
    else
      DLL_ENTRY = _DllMainCRTStartup@12
    endif
  endif
endif

ifeq "$(DLL_BASE)" ""
  DLL_BASE = 0x10000000
endif

#ifeq "$(ECX_BASE)" ""
#  ECX_BASE = 0x400000
#endif

ifeq "$(32B_FLAGS)" ""
  32B_FLAG = -m32
endif

##########################################################################
#
# EXE_FLAGS and DLL_FLAGS
#
ifeq "$(XDK_COMPILER_VERSION)" "_gcc4"
  EXE_FLAGS := --entry $(EXE_ENTRY) $(EXE_FLAGS)
  ECX_FLAGS := --entry $(ECX_ENTRY) $(ECX_FLAGS)
else
  EXE_FLAGS := --entry $(EXE_ENTRY) $(EXE_FLAGS)
  ECX_FLAGS := --entry $(ECX_ENTRY) $(ECX_FLAGS)
endif

ifneq "$(XDK_TARGET_FORMAT)" "elf"
  ifeq "$(XDK_COMPILER_VERSION)" "_gcc4"
    DLL_FLAGS := $(DLL_FLAGS) --dll --entry $(DLL_ENTRY) -shared
  else
    DLL_FLAGS := $(DLL_FLAGS) --dll --entry _$(DLL_ENTRY) -shared
  endif

  ifneq "$(NATIVE_BUILD)" "1"
    ifeq "$(USE_STDLIB)" ""
      LINK_FLAGS := -nostdlib $(LINK_FLAGS)
    endif
  endif

  LINK_FLAGS:= -mno-cygwin $(LINK_FLAGS)

else # "$(XDK_TARGET_FORMAT)" "elf"
  DEF_FILE_NAME := $(patsubst %.car,%,$(filter %.car,$(SOURCES)))
  ifeq "$(DEF_FILE_NAME)" ""
    DEF_FILE_NAME := $(patsubst %.def,%,$(filter %.def,$(SOURCES)))
  endif

  ifneq "$(findstring .def,$(SOURCES))" ""
    DEF_FLAGS := -Wl,--retain-symbols-file,__$(DEF_FILE_NAME).sym  -Wl,--version-script,__$(DEF_FILE_NAME).vs
  endif

    ifeq "$(XDK_TARGET_PLATFORM)" "linux"
        DLL_FLAGS := $(32B_FLAG) $(DLL_FLAGS) -shared -Wl,-fPIC,--no-undefined,--no-undefined-version
        ifeq "$(EXPORT_ALL_SYMBOLS)" ""
            DLL_FLAGS := $(DLL_FLAGS) $(DEF_FLAGS)
        else
            DLL_DBGINFO_FLAGS := $(DLL_FLAGS)
            DLL_FLAGS := $(DLL_FLAGS) $(DEF_FLAGS)
        endif
          GCC_SYSROOT=$(XDK_SYSROOT_PATH)
          GCC_LIB_PATH=$(shell $(CC) -print-file-name=)

          EXE_FLAGS := $(EXE_FLAGS) $(32B_FLAG) -Bdynamic -Wl,--no-gc-sections -L$(XDK_TARGETS)
          ECX_FLAGS := $(ECX_FLAGS) $(32B_FLAG) -Bdynamic -Wl,--no-gc-sections -L$(XDK_TARGETS) -L$(PREBUILD_LIB)

          EXE_CRT_BEGIN=-Wl,-X
          ECX_CRT_BEGIN=-Wl,-X

          DLL_CRT_BEGIN=-Wl,-X
          DLL_CRT_END=$(GCC_LIB_PATH)/32/libgcc.a
    endif
    ifeq "$(XDK_TARGET_PLATFORM)" "android"
          DLL_FLAGS := $(DLL_FLAGS) $(LIBC_FLAGS) -nostdlib -shared -fPIC -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version
          ifeq "$(EXPORT_ALL_SYMBOLS)" ""
              DLL_FLAGS := $(DLL_FLAGS) $(DEF_FLAGS)
          else
              DLL_DBGINFO_FLAGS := $(DLL_FLAGS)
              DLL_FLAGS := $(DLL_FLAGS) $(DEF_FLAGS)
          endif

          GCC_SYSROOT=$(PREBUILD_PATH)
          GCC_LIB_PATH=$(shell $(CC) -print-file-name=)

          EXE_FLAGS := $(EXE_FLAGS) -lgcc -lc -lstdc++ -nostdlib -Bdynamic -pie -Wl,--no-gc-sections -Wl,-z,nocopyreloc -L$(PREBUILD_LIB) -L$(XDK_TARGETS)
          ECX_FLAGS := $(ECX_FLAGS) $(LIBC_FLAGS) -lstdc++ -nostdlib -Bdynamic -pie -Wl,--gc-sections -Wl,-z,nocopyreloc -L$(PREBUILD_LIB) -L$(XDK_TARGETS)

          EXE_CRT_BEGIN=--sysroot=$(GCC_SYSROOT) -Wl,-X -Wl,-dynamic-linker,/system/bin/linker $(PREBUILD_LIB)/crtbegin_dynamic.o
          ECX_CRT_BEGIN=--sysroot=$(GCC_SYSROOT) -Wl,-X -Wl,-dynamic-linker,/system/bin/linker $(PREBUILD_LIB)/crtbegin_dynamic.o

          EXE_CRT_END=$(PREBUILD_LIB)/crtend_android.o
          ECX_CRT_END=$(PREBUILD_LIB)/crtend_android.o

          DLL_CRT_BEGIN=--sysroot=$(GCC_SYSROOT) -Wl,-X -Wl,-dynamic-linker,/system/bin/linker $(PREBUILD_LIB)/crtbegin_so.o $(PREBUILD_LIB)/crtend_so.o
          DLL_CRT_END=$(GCC_LIB_PATH)/libgcc.a
      endif
endif # elf

##########################################################################
#
# C_FLAGS
#

ifeq "$(XDK_TARGET_CPU)" "x86"
  C_FLAGS := -mno-align-double $(C_FLAGS)
endif
ifeq "$(XDK_VERSION)" "rls"
  C_FLAGS:= -Os -fno-strict-aliasing $(C_FLAGS)
  C_DEFINES:= -D_RELEASE -DNDEBUG $(C_DEFINES)
  LINK_FLAGS:= -O3 $(LINK_FLAGS)
endif
ifeq "$(XDK_VERSION)" "pre"
  ifeq "$(XDK_TARGET_CPU)" "arm"
    C_FLAGS := -mapcs-frame -fno-omit-frame-pointer $(C_FLAGS)
  endif
  C_FLAGS:= -Os $(C_FLAGS)
  ifeq "$(NODBGSYMBOL)" ""
    C_FLAGS := -ggdb $(C_FLAGS)
  endif
  C_DEFINES:= -D_RELEASE -D_PRERELEASE -DNDEBUG $(C_DEFINES)
  LINK_FLAGS:= -O3 $(LINK_FLAGS)
endif
ifeq "$(XDK_VERSION)" "opt"
  ifeq "$(XDK_TARGET_CPU)" "arm"
    C_FLAGS := -mapcs-frame -fno-omit-frame-pointer $(C_FLAGS)
  endif
  C_FLAGS:= -Os $(C_FLAGS)
  ifeq "$(NODBGSYMBOL)" ""
    C_FLAGS := -ggdb $(C_FLAGS)
  endif
  C_DEFINES:= -D_DEBUG -D_OPTIMIZE $(C_DEFINES)
endif
ifeq "$(XDK_VERSION)" "dbg"
  C_FLAGS:= -O0 $(C_FLAGS)
  ifeq "$(NODBGSYMBOL)" ""
    C_FLAGS := -ggdb $(C_FLAGS)
  endif
  ifeq "$(XDK_TARGET_CPU)" "arm"
    C_FLAGS := -mapcs-frame -fno-omit-frame-pointer $(C_FLAGS)
  endif
  C_DEFINES:= -D_DEBUG $(C_DEFINES)
endif

ifneq "$(NOWARNING)" ""
  C_FLAGS := -c -fno-builtin -w -falign-functions=4 $(C_FLAGS)
else
  C_FLAGS := -c -fno-builtin -Wall -falign-functions=4 $(C_FLAGS)
endif

ifeq "$(USE_STDINC)" ""
  ifneq "$(NATIVE_BUILD)" "1"
    C_FLAGS := $(C_FLAGS) -nostdinc
  endif
endif

ifeq "$(XDK_TARGET_PLATFORM)" "linux"
  ifeq "$(NO_ANDROIDINC)" ""
    C_FLAGS := -I$(MAKEDIR) -I/usr/include -I/usr/include/dbus-1.0 -I$(XDK_USER_INC) $(C_FLAGS)
  endif
endif

C_DEFINES := -D_GNUC $(C_DEFINES)

ifeq "$(XDK_TARGET_PLATFORM)" "linux"
  ifneq "$(XDK_TARGET_CPU)" "arm"
    C_DEFINES := $(C_DEFINES) $(32B_FLAG)
  endif
endif

ifeq "$(XDK_TARGET_CPU)" "x86"
  C_FLAGS:= $(C_FLAGS) $(32B_FLAG)
  ifeq "$(XDK_TARGET_PLATFORM)" "linux"
    C_FLAGS:= -D_UNDEFDLLEXP $(C_FLAGS)
  endif
endif

ifeq "$(XDK_TARGET_CPU)" "arm"
  C_FLAGS:= -march=armv7-a \
            -fms-extensions -mstructure-size-boundary=8 \
            $(C_FLAGS)
  ifneq "$(XDK_TARGET_PLATFORM)" "linux"
  ifneq "$(XDK_TARGET_PLATFORM)" "android"
  ifneq "$(XDK_COMPILER_VERSION)" "_gcc4"
    C_FLAGS:= -mapcs-32 -msoft-float  $(C_FLAGS)
  endif
  endif
  endif
  ifeq "$(XDK_TARGET_PLATFORM)" "android"
    C_FLAGS:= -msoft-float -fPIC -mthumb-interwork -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wno-unused-local-typedefs $(C_FLAGS)
  endif
  ifeq "$(XDK_TARGET_PLATFORM)" "linux"
    C_FLAGS:= -msoft-float -fPIC -mthumb-interwork -ffunction-sections -fdata-sections -funwind-tables -fstack-protector $(C_FLAGS)
  endif
endif

# -fno-rtti only valid for C++ but not for C/ObjC since 3.3.3
CPP_FLAGS := -fno-rtti -fcheck-new -Wno-psabi $(CPP_FLAGS)

##########################################################################
#
# INCLUDES
#
ifneq "$(USE_STDINC)" ""
ifeq "$(SYSTEM_INCLUDES)" "$(INCLUDES)"
INCLUDES=
endif
endif

INCLUDES:= $(subst ;, ,$(INCLUDES))

# Remove white space and null include path
INCLUDES := $(strip $(INCLUDES))
INCLUDES:= $(patsubst %,-I %,$(INCLUDES))
INCLUDES := $(subst \,/,$(INCLUDES))

##########################################################################
#
# Other build tools and macros
#

CAR = carc
ifneq "$(XDK_VERSION)" "rls"
  CAR_FLAGS:= -I$(MAKEDIR) $(CAR_FLAGS)
else
  CAR_FLAGS:= -I$(MAKEDIR) -a $(CAR_FLAGS)
endif

LUBC= lubc
LUBE= lube

AR_FLAGS = rcs

CP = cp -f -u
CPDIR = cp -rf -u
MV = mv
MKDIR = mkdir -p
RM = rm -f
RMDIR = rm -f -r

ifneq "$(FILERESOURCES)" ""
FILERESOURCES:= $(strip $(FILERESOURCES))
FILERESOURCES:= $(subst ;, ,$(FILERESOURCES))
endif

ifneq "$(DIRRESOURCES)" ""
DIRRESOURCES:= $(strip $(DIRRESOURCES))
DIRRESOURCES:= $(subst ;, ,$(DIRRESOURCES))
endif

ifneq "$(LIBRESOURCES)" ""
LIBRESOURCES := $(strip $(LIBRESOURCES))
LIBRESOURCES := $(subst ;, ,$(LIBRESOURCES))
endif

##########################################################################
#
# Tidy up custom specified macros
#

ifneq "$(IMPORTS)" ""
  IMPORTS:= $(strip $(IMPORTS))       # remove multiple spaces
  IMPORTS:= $(filter-out \,$(IMPORTS))
  IMPORTHS = $(IMPORTS:.dso=.h)
  IMPORTHS = $(IMPORTS:.eco=.h)
endif

ifdef APPPACK
  APPPCK_LABLE=$(MAKEDIR)/R.cpp
ifndef APP_DIR_NAME
  APP_DIR_NAME=$(APP_PACKAGE_NAME)
endif
endif


ifneq "$(LIBRARIES)" ""
  LIBRARIES:= $(strip $(LIBRARIES))       # remove multiple spaces
  LIBRARIES:= $(filter-out \,$(LIBRARIES))
endif

.LIBPATTERNS = lib%.so lib%.a

ifneq "$(ELASTOS_LIBS)" ""
  ELASTOS_LIBS:= $(filter-out \,$(ELASTOS_LIBS))
  ELASTOS_LIBS:= $(strip $(ELASTOS_LIBS)) # remove multiple spaces
endif

EXPORT_HEADERS := $(subst \, ,$(EXPORT_HEADERS))

##########################################################################
#
# Create OBJECTS from SOURCES; modify INCLUDES to comply with compiler
#
ifneq "$(SOURCES)" ""
  ifeq "$(OBJECTS)" ""
    OBJECTS := $(SOURCES:.cpp=.o)
    OBJECTS := $(OBJECTS:.cc=.o)
    OBJECTS := $(OBJECTS:.c=.o)
    OBJECTS := $(OBJECTS:.S=.o)
    OBJECTS := $(OBJECTS:.asm=.o)
    OBJECTS := $(OBJECTS:.lub=.lbo)
    OBJECTS := $(OBJECTS:.rc=.res)
    OBJECTS := $(OBJECTS:.def=.exp)
    OBJECTS := $(OBJECTS:.car=.rsc)
    OBJECTS := $(strip $(OBJECTS))        # remove multiple spaces
    RESSECTION = __section.o
  endif
endif
ifdef APPPACK
    OBJECTS:= $(filter-out R.0,$(OBJECTS))
endif
ifneq "$(RESOURCES)" ""
  ifeq "$(RESOURCES_OBJECTS)" ""
    RESOURCES_OBJECTS := $(RESOURCES:.xml=.res)
    RESOURCES_OBJECTS := $(RESOURCES_OBJECTS:.loc=.res)
    RESOURCES_OBJECTS := $(RESOURCES_OBJECTS:.sql=.db)
    RESOURCES_OBJECTS := $(strip $(RESOURCES_OBJECTS))        # remove multiple spaces
  endif
endif

##########################################################################
#
# SEARCH_LIB
#
ifneq "$(XDK_TARGET_FORMAT)" "elf"
  SEARCH_LIB:= $(SEARCH_LIB)
endif

C_FLAGS:= $(C_DEFINES) $(C_FLAGS) $(INCLUDES)

##########################################################################
#
# FOR depend
#
DEPEND_CAR_FLAGS = -d

DEPEND_TOOL = $(CC)

DEPEND_FLAGS = -MMD

ifneq "$(XDK_TARGET_PLATFORM)" "android"
  ifneq "$(XDK_TARGET_PLATFORM)" "linux"
    DEPEND_FLAGS = -MD
  endif
endif

DEPEND_SOURCES := $(filter %.o %.rsc,$(OBJECTS))
DEPEND_SOURCES := $(patsubst %.o,%.d,$(DEPEND_SOURCES))
DEPEND_SOURCES := $(patsubst %.rsc,%.d,$(DEPEND_SOURCES))
