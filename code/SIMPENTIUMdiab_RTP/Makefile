# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))
WRVX_COMPBASE := $(subst \,/,$(WRVX_COMPBASE))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

JOBS?=1
TARGET_JOBS?=$(JOBS)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = SIMPENTIUMdiab_RTP
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := D:/ezbs_weithoglafe
PRJ_ROOT_DIR := $(WS_ROOT_DIR)/paketladeding



#Global Build Macros
PROJECT_TYPE = RTP
DEFINES = 
DO_STRIP = 0
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = simpentium
CPU = SIMPENTIUM
TOOL_FAMILY = diab
TOOL = diab
TOOL_PATH = 
CC_ARCH_SPEC = -tX86LH:rtpsim
LIBPATH = 
LIBS = -lstlstd
VSB_DIR = $(WIND_BASE)/target/lib

IDE_INCLUDES = -I$(WIND_BASE)/target/usr/h -I$(WIND_BASE)/target/usr/h/wrn/coreip 

IDE_LIBRARIES = 

IDE_DEFINES = -D_VX_CPU=_VX_$(CPU) -D_VX_TOOL_FAMILY=$(TOOL_FAMILY) -D_VX_TOOL=$(TOOL)   



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler =  -XO
DEBUGFLAGS_C++-Compiler =  -XO
DEBUGFLAGS_Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler =  -XO
endif


#Project Targets
PROJECT_TARGETS = paketladeding/$(MODE_DIR)/paketladeding.vxe


#Rules

# paketladeding
ifeq ($(DEBUG_MODE),1)
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Linker = 
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler =  -XO
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler =  -XO
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Linker = 
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
paketladeding/$(MODE_DIR)/% : DEBUGFLAGS_Assembler =  -XO
endif
paketladeding/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/usr/h -I$(WIND_BASE)/target/usr/h/wrn/coreip 
paketladeding/$(MODE_DIR)/% : IDE_LIBRARIES = 
paketladeding/$(MODE_DIR)/% : IDE_DEFINES = -D_VX_CPU=_VX_$(CPU) -D_VX_TOOL_FAMILY=$(TOOL_FAMILY) -D_VX_TOOL=$(TOOL)   
paketladeding/$(MODE_DIR)/% : PROJECT_TYPE = RTP
paketladeding/$(MODE_DIR)/% : DEFINES = 
paketladeding/$(MODE_DIR)/% : DO_STRIP = 0
paketladeding/$(MODE_DIR)/% : EXPAND_DBG = 0
paketladeding/$(MODE_DIR)/% : VX_CPU_FAMILY = simpentium
paketladeding/$(MODE_DIR)/% : CPU = SIMPENTIUM
paketladeding/$(MODE_DIR)/% : TOOL_FAMILY = diab
paketladeding/$(MODE_DIR)/% : TOOL = diab
paketladeding/$(MODE_DIR)/% : TOOL_PATH = 
paketladeding/$(MODE_DIR)/% : CC_ARCH_SPEC = -tX86LH:rtpsim
paketladeding/$(MODE_DIR)/% : LIBPATH = 
paketladeding/$(MODE_DIR)/% : LIBS = -lstlstd
paketladeding/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib
paketladeding/$(MODE_DIR)/% : OBJ_DIR := paketladeding/$(MODE_DIR)

paketladeding/$(MODE_DIR)/Objects/paketladeding/simulation.o : $(PRJ_ROOT_DIR)/simulation.c $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -Xansi -WDVSB_DIR=$(WIND_BASE)/target/lib -Xsystem-headers-warn -Xforce-declarations -ei4167,4092,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


OBJECTS_paketladeding = paketladeding/$(MODE_DIR)/Objects/paketladeding/simulation.o

ifeq ($(TARGET_JOBS),1)
paketladeding/$(MODE_DIR)/paketladeding.vxe : $(OBJECTS_paketladeding)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dplus $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -Xansi -WDVSB_DIR=$(WIND_BASE)/target/lib -Xsystem-headers-warn -Xforce-declarations -ei4167,4092,4144,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457   -Xmake-dependency=0xd -f 0x90,1,1 -o "$@" $(OBJECTS_paketladeding) $(ADDED_OBJECTS) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) -L$(VSB_DIR)/usr/lib/simpentium/SIMPENTIUM/common $(ADDED_LIBPATH) $(ADDED_LIBS)   && if [ "$(DO_STRIP)" = "1" ]; then mv -f "$@" "$@.unstripped" && strippentium -g -o "$@" "$@.unstripped";fi && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

else
paketladeding/$(MODE_DIR)/paketladeding.vxe : paketladeding/$(MODE_DIR)/paketladeding.vxe_jobs

endif
paketladeding/$(MODE_DIR)/paketladeding_compile_file : $(FILE) ;

_clean :: paketladeding/$(MODE_DIR)/paketladeding_clean

paketladeding/$(MODE_DIR)/paketladeding_clean : 
	$(TRACE_FLAG)if [ -d "paketladeding" ]; then cd "paketladeding"; rm -rf $(MODE_DIR); fi

force : 

TARGET_JOBS_RULE?=echo update makefile template
%_jobs : 
	$(TRACE_FLAG)$(TARGET_JOBS_RULE)

DEP_FILES := paketladeding/$(MODE_DIR)/Objects/paketladeding/simulation.d
-include $(DEP_FILES)


WIND_SCOPETOOLS_BASE := $(subst \,/,$(WIND_SCOPETOOLS_BASE))

clean_scopetools :
	$(TRACE_FLAG)rm -rf $(PRJ_ROOT_DIR)/.coveragescope/db

CLEAN_STEP := clean_scopetools


-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

TARGET_JOBS_RULE=$(MAKE) -f $(MAKEFILE) --jobs $(TARGET_JOBS) $(MFLAGS) $* TARGET_JOBS=1
ifeq ($(JOBS),1)
main_all : external_build  $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"
else
main_all : external_build 
	@$(MAKE) -f $(MAKEFILE) --jobs $(JOBS) $(MFLAGS) $(PROJECT_TARGETS) TARGET_JOBS=1;\
	echo "make: built targets of `pwd`"
endif 

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
