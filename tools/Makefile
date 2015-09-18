CXX     = clang++
CXFLAGS = -Wall -Wextra -Iinclude -I../include -std=c++11 -O3 -DVERSION_MAJOR="$(VERSION_MAJOR)"
LDFLAGS =

%.o: %.cpp
	@echo -e "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CXX) $(CXFLAGS) -c -o $@ $<
all: mkinitrd
mkinitrd: #src/mkinitrd.o
	@echo -e "\033[1;37mLinking `basename $@`... \033[0m"
	@$(CXX) $(CXFLAGS) $(LDFLAGS) -o "$@.bin" $^
clean:
	rm -rf `find src/ -name "*.o")
.PHONY: all clean
