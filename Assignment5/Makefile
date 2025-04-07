# Compiler and flags
CXX       = clang++
CXXFLAGS  = -Wall -std=c++11 -O2 -I/usr/local/include -I/opt/homebrew/include

# Libraries: adjust if needed (this example links against OpenGL, GLEW, GLFW, and math)
LIBS = -framework OpenGL -lglew -lglfw -lm -L/opt/homebrew/lib

# Directories
SRCDIR    = src
OBJDIR    = obj
BINDIR    = bin

# List of source files (all .cpp files in the src folder)
SOURCES   = camera.cpp compute_normals.cpp main.cpp marching_cubes.cpp shader_utils.cpp write_ply.cpp 

# Object files corresponding to sources (placed in the obj folder)
OBJECTS   = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# The final executable
TARGET    = $(BINDIR)/Assignment5

# Default target
all: $(TARGET)

# Link all object files into the executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Rule to compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
