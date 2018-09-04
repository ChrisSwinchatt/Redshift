override CXX     := clang++
override CXFLAGS += -I ../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args

%.o: %.cpp
	@$(CXX) $(CXFLAGS) -c $@ $<

all: klist ksorted_array

klist: libk/test_klist.o

ksorted_array: libk/test_ksorted_array.o

clean:
	rm -f libk/*.o

.PHONY: all clean klist ksorted_array
