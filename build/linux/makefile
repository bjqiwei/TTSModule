-include project.txt

######################################################################
#--------------以下内容不需要修改------------------------------------#
######################################################################

export PROJECT_CC= g++
export PROJECT_LINKER= g++
export PROJECT_AR= ar rv
export PWD = $shell('pwd')

DLL_FLAG=
ifeq ($(PROJECT_TYPE),so)
DLL_FLAG= -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
endif

ifeq ($(PROJECT_TYPE),dll)
DLL_FLAG= -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
endif

#自动判断32位、64位系统
SYSBYTE := $(shell uname -r | grep 'x86_64')
ifeq ($(strip $(SYSBYTE)),) 
override SYSBYTE = -m32 
#-std=gnu++0x
else
override SYSBYTE = -m64 
#-std=gnu++0x
endif 

override SYSBYTE = -m32
#编译用的编译器参数
export PROJECT_CPP_FLAGS = $(PROJECT_INCLUDE) $(DLL_FLAG) -Wall $(SYSBYTE) $(PROJECT_DEF)

#链接用的参数
export PROJECT_EXE_LINK_FLAGS = $(PROJECT_LINK_DIR) -Wall $(SYSBYTE) -Wl,-rpath -Wl,.
export PROJECT_DLL_LINK_FLAGS = $(PROJECT_LINK_DIR) -Wall $(SYSBYTE) -shared -fPIC

#PROJECT源文件列表 
PROJECT_CPP_FILES :=$(foreach dir,$(PROJECT_SOURCE),$(wildcard $(dir)/*.cpp))
PROJECT_CXX_FILES :=$(foreach dir,$(PROJECT_SOURCE),$(wildcard $(dir)/*.cxx))
PROJECT_C_FILES :=$(foreach dir,$(PROJECT_SOURCE),$(wildcard $(dir)/*.c))

PROJECT_CPP_DEP := $(PROJECT_CPP_FILES:%.cpp=%.d)
PROJECT_CXX_DEP := $(PROJECT_CXX_FILES:%.cxx=%.d)
PROJECT_C_DEP := $(PROJECT_C_FILES:%.c=%.d)

export PROJECT_SRC_FILES :=$(PROJECT_CPP_FILES) $(PROJECT_CXX_FILES) $(PROJECT_C_FILES)
export PROJECT_DEPS := $(PROJECT_CPP_DEP) $(PROJECT_CXX_DEP) $(PROJECT_C_DEP)
export PROJECT_OBJS := $(PROJECT_DEPS:%.d=%.o)

default: debug

all: debug release

.PHONY :debug
debug: config
	@-mkdir -p debug
	@-mkdir -p $(PROJECT_PATH)
	@-unzip -n ../../log4cplus-1.1.3-rc2.zip -d ../../
	#@-cd ../../log4cplus-1.1.3-rc2 && ./configure CXXFLAGS=-m32 CPPFLAGS=-m32 CFLAGS=-m32
	#@-cd ../../log4cplus-1.1.3-rc2 && make 
	@-make -f makefile.debug
	@-cp ../../log4cplus-1.1.3-rc2/src/.libs/liblog4cplus-1.1.so.9.0.1  ../../work/bin/liblog4cplus-1.1.so.9
	@-cp /opt/intp60/bin/isp/libsem.so  ../../work/bin/
	@-cp ./TTSserver_check.sh  ../../work/bin/


.PHONY: release
release: config
	@-mkdir -p release
	@-mkdir -p $(PROJECT_PATH)
	@-unzip -n ../../log4cplus-1.1.3-rc2.zip -d ../../
	#@-cd ../../log4cplus-1.1.3-rc2 && ./configure CXXFLAGS=-m32 CPPFLAGS=-m32 CFLAGS=-m32
	#@-cd ../../log4cplus-1.1.3-rc2 && make 
	@-make -f makefile.release
	@-cp ../../log4cplus-1.1.3-rc2/src/.libs/liblog4cplus-1.1.so.9.0.1  ../../work/bin/liblog4cplus-1.1.so.9
	@-cp /opt/intp60/bin/isp/libsem.so  ../../work/bin/
	@-cp ./TTSserver_check.sh  ../../work/bin/


clean: clean-d clean-r

clean-d:
	#@-cd ../../log4cplus-1.1.3-rc2 && make clean
	@-make -f makefile.debug clean
	@-rm -rf debug
	@-rm -rf ../../work/bin
	

clean-r:
	#@-cd ../../log4cplus-1.1.3-rc2 && make clean
	@-make -f makefile.release clean
	@-rm -rf release
	@-rm -rf ../../work/bin
	
config: ${PROJCT_VERSION_H}
	
${PROJCT_VERSION_H}: FORCE
	@echo "#ifndef __VERSION_H__" > $@.tmp
	@echo "#define __VERSION_H__" >> $@.tmp
	@echo "" >> $@.tmp
	@echo "#define APPLICATION_PROGRAM_VERSION      \"${PROJECT_VERSION}\"" >> $@.tmp
	@echo "#define APPLICATION_PROGRAM_BUILD_YEAR   \"`date +%Y`\"" >> $@.tmp
	@echo "#define APPLICATION_PROGRAM_BUILD_DATE   \"`date +%F`\"" >> $@.tmp
	@echo "#define APPLICATION_PROGRAM_BUILD_TIME   \"`date +%T`\"" >> $@.tmp
	@echo "#define APPLICATION_PROGRAM_BUILD_INFO   \"`$(PROJECT_CC) --version | head -n 1`\"" >> $@.tmp
	@echo "" >> $@.tmp
	@echo "#endif" >> $@.tmp
	@echo "" >> $@.tmp
	@if cmp -s $@.tmp $@ ; then :; else \
		mv $@.tmp $@ ; \
	fi
	@rm -f $@.tmp

FORCE:

