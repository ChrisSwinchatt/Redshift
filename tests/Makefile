override CXX     := clang++
override CXFLAGS += -I ../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args

%.o: %.cpp
	@$(CXX) $(CXFLAGS) -c $@ $<

all:

clean:

.PHONY: all
