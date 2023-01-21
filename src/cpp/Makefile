srcs := ostream_utility.cpp diagnose.cpp spectrum_data.cpp \
					 entities.cpp visualize.cpp k_path.cpp latexify.cpp \
					 physics_and_chemistry.cpp irreps.cpp
objs := $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

link_flags := -lLLVM-14
compile_flags := -g -O3 -std=c++20 -Wall -Wpedantic -Wextra

CC := g++

diagnose.exe: $(objs)
	$(CC) $^ -o $@  -lfmt $(link_flags)

$(objs): %.o:%.cpp
	$(CC) -MMD -MP $(compile_flags) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o

-include $(deps)
