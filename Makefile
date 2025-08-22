CXX = g++
CXXFLAGS = -g -std=c++11
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = riscv_sim
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(EXEC)
.PHONY: clean
