TARGET    := tcp_server

CXX       := g++
CXXFLAGS  := -Wall -Werror
PREF_OBJS := ./obj/
PREF_SRCS := ./src/
MACROS    := -D DEBUG

# SRCS = $(wildcard $(PREF_SRCS)*.cpp)
# OBJS = $(patsubst $(PREF_SRCS)%.cpp, $(PREF_OBJS)%.o, $(SRCS))
SRCS      := $(PREF_SRCS)tcp_server.cpp $(PREF_SRCS)server.cpp
OBJS      := $(PREF_OBJS)tcp_server.o $(PREF_OBJS)server.o

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

$(PREF_OBJS)tcp_server.o: $(PREF_SRCS)tcp_server.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

$(PREF_OBJS)server.o: $(PREF_SRCS)server.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

clean:
	rm -f $(TARGET) $(PREF_OBJS)*.o
