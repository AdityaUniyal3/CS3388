# Makefile for Assignment 4

# Compiler and flags
CXX = clang++
CXXFLAGS = -Wall -std=c++11 -O2 -I/usr/local/include -I/opt/homebrew/include

# Linker flags (adjust paths for your system)
LDFLAGS = -L/usr/local/lib -L/opt/homebrew/lib -lglew -lglfw -framework OpenGL

# Source files
SRCS = asn4.cpp Camera.cpp TexturedMesh.cpp LoadBitmap.cpp
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = asn4

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
