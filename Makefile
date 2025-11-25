# Makefile for cse4001_sync

CXX = g++
CXXFLAGS = -pthread -Wall -Wextra -std=c++11

TARGET = cse4001_sync

SRCS = cse4001_sync.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o

run:
	./$(TARGET)

.PHONY: all clean run
