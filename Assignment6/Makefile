# Compiler and flags
CXX       = clang++
CXXFLAGS  = -Wall -std=c++11 -O2 -I/usr/local/include -I/opt/homebrew/include

# Libraries
LIBS = -framework OpenGL -lglew -lglfw -lm -L/opt/homebrew/lib

# Directories
SRCDIR    = src
OBJDIR    = obj

# List of source files
SOURCES   = A6-Water.cpp PlaneMesh.cpp ShaderLoader.cpp camera.cpp 

# Object files corresponding to sources
OBJECTS   = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SOURCES)))

# The final executable in the root folder
TARGET    = Assignment6

# Default target
all: $(TARGET)

# Link all object files into the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Rule to compile source files to object files
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
