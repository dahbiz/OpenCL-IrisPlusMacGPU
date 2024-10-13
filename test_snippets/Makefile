# GCC COMPILER AND FLAGS
CXX = g++
CXXFLAGS = -framework OpenCL -std=c++17 -O3

# TARGETS
SRC = comp_capbl.cpp dev_info.cpp vec_add.cpp vec_sum.cpp mat_mul.cpp
TARGETS = $(SRC:.cpp=) 

# MAKE RULES
all: $(TARGETS)

# BUILD A CUSTOM TARGET
$<: $<.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# CLEAN ALL CREATED FILES
clean:
	rm -f $(TARGETS)

# PHONY RULES
.PHONY: all clean