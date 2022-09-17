TARGET_SERVER := server
TARGET_CLIENT := client

CXX       := g++
CXXFLAGS  := -Wall -Werror
PREF_OBJS := ./obj/
PREF_SRCS := ./src/
MACROS    := -D DEBUG

PREF_SRCS := ./src/
PREF_OBJS := ./obj/

SRCS_SERVER := $(PREF_SRCS)tcp_server.cpp $(PREF_SRCS)server.cpp
OBJS_SERVER := $(patsubst $(PREF_SRCS)%.cpp, $(PREF_OBJS)%.o, $(SRCS_SERVER))

SRCS_CLIENT := $(PREF_SRCS)tcp_client.cpp $(PREF_SRCS)client.cpp
OBJS_CLIENT := $(patsubst $(PREF_SRCS)%.cpp, $(PREF_OBJS)%.o, $(SRCS_CLIENT))

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CXX) $(OBJS_SERVER) -o $(TARGET_SERVER)

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(OBJS_CLIENT) -o $(TARGET_CLIENT)

$(PREF_OBJS)tcp_server.o: $(PREF_SRCS)tcp_server.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

$(PREF_OBJS)server.o: $(PREF_SRCS)server.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

$(PREF_OBJS)tcp_client.o: $(PREF_SRCS)tcp_client.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

$(PREF_OBJS)client.o: $(PREF_SRCS)client.cpp
	$(CXX) $(CXXFLAGS) $(MACROS) -c $< -o $@

clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT) $(PREF_OBJS)*.o
