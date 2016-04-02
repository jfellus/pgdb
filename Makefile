
###### INSTALLATION GUIDE ######
# make external_libs
# make
# ./coeos++
################################


APT_GET_DEPENDENCIES:=libwebsockets-dev


REQUIRED_PACKAGES:=libwebsockets
REQUIRED_LIBS:= -lpthread 


INCLUDE_PATHS:=$(HOME)/.pgcc/includes/

SRC_DIR:=./src

LIBRARY:=libpgdb.so







########################## DON'T EDIT BELOW THIS LINE (unless you are a gnu make's expert ##############

SRC := $(shell find $(SRC_DIR) -name '*.cpp') 
OBJS := $(addprefix bin/,$(SRC:.cpp=.o))

all: ./include $(LIBRARY) 
$(LIBRARY): $(OBJS)

CXXFLAGS := -std=c++11 -fPIC -g -rdynamic -Wall -MMD $(addprefix -I,$(INCLUDE_PATHS))
LDFLAGS := -std=c++11 -fPIC -rdynamic 
DEPENDS = $(OBJS:.o=.d)    


./include: $(OBJS)
	@rm -rf ./include
	@cp -r ./src ./include
	@find ./include -name '*.cpp' -delete

install: ./include $(LIBRARY)
	@ln -fs `pwd`/libpgdb.so /usr/lib/libpgdb.so 
	@ln -fs `pwd`/include/ /usr/include/pgdb 
	@echo "Installation successful"
	
	
$(LIBRARY) : $(OBJS)          
	@echo "Build library $@"
	@$(CXX) -shared $(OBJS) -o $@ $(LDFLAGS) 
	


bin/%.o: %.cpp
	@mkdir -p `dirname $(@:.o=.d)`
	@touch $(@:.o=.d)
	@echo "Compilation : $< "
	@g++ $(CXXFLAGS) -MMD -c $< -o $@

bin: 
	@mkdir -p bin

clean:
	@rm -f $(EXECUTABLE)
	@rm -rf bin
	


external_libs:
	@echo "We need your password for this : "
	@sudo echo "thank you"
	@sudo apt-get install $(APT_GET_DEPENDENCIES)
	@echo "DONE"
	

-include $(DEPENDS) 
