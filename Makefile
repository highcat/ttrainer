CXX=g++

CFLAGS= -O0 # best speed
#CFLAGS= -O0 -g -gstabs+ # best debuggin info for C++

.PHONY: all
.PHONY: all_d
.PHONY: system_depend

APP_NAME=ttrain


PLATFORM=LIN
#PLATFORM=WIN
#PLATFORM=MAC

# convention: platform-dependent files should be in SYS_DEPEND_DIR directory,
# and should end with _$(PLATFORM) before extension, like "foo_WIN.cpp" "foo_LIN.cpp" "foo_MAC.cpp", 
# "foo_WIN.h" "foo_LIN.h" "foo_MAC.h"
# 
# list of system-dependent files
# "_WIN.cpp" or "_LIN.cpp" or "_MAC.cpp" should be added after this names
SYS_DEPEND_FILES=time


SYS_DEPEND_DIR=sys_depend


# First, we create .d files for each cpp. Then we create the project itself.
all: system_depend all_d $(APP_NAME) # *!?!* order matters?


LIBS= -lcurses -ldl

clean:
	rm *.o *.d $(APP_NAME); rm $(SYS_DEPEND_DIR)/*.o $(SYS_DEPEND_DIR)/*.d; exit 0;

cleanbk:
	rm *~ \#*\#; exit 0

system_depend:
	cd $(SYS_DEPEND_DIR); make all; cd ..;


# links application; system-depend files only from selected operating system
$(APP_NAME): $(patsubst %.cpp, %.o, $(wildcard *.cpp)) 
	$(CXX) $^ $(patsubst %, $(SYS_DEPEND_DIR)/%.o, $(SYS_DEPEND_FILES)_$(PLATFORM) ) -o $@ $(LIBS) $(CFLAGS) -D$(PLATFORM)


# GGRRRRRR!! THERE IS DEFAUT RULE FOR %.o!!! try if you don't belive
# it may be disabled by passing -r option to make

%.o: %.cpp
	$(CXX) $(patsubst %.o, %.cpp, $@) -MMD -c $(CFLAGS) -D$(PLATFORM)

# creating .d files for all cpp-s
#
all_d: $(patsubst %.cpp, %.d, $(wildcard *.cpp))

%.d: %.cpp # without cpp target! need to process _before_ cpp targets
	$(CXX) -MM -MF $@ $^ -D$(PLATFORM)


include $(wildcard *.d)

