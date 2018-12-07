SOURCES := $(shell find -name "*.cpp")
OBJECTS := $(subst .cpp,.o,$(SOURCES))
BINARY  := $(LIB_DIR)/libk.a

kabi_stack_guard.o: CXFLAGS := $(filter-out -fstack-protector-all,$(CXFLAGS)) -fno-stack-protector

%.o: %.S
	@echo "\033[1;37mAssembling `basename $<`... \033[0m"
	@$(CXX) $(CXFLAGS) -c -o $@ $<

%.o: %.cpp
	@echo "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CXX) $(CXFLAGS) -c -o $@ $<

all: $(BINARY)

clean:
	@echo "\033[1;37mCleaning libk... \033[0m"
	@rm -f $(OBJECTS) $(BINARY)

$(BINARY): $(OBJECTS)
	@echo "\033[1;37mLinking `basename $@`"
	@ar -cvq $@ $^

.PHONY: all clean $(BINARY)
