CXX = g++
CXXFLAGS = -std=c++14 -Wno-deprecated-register

SRCS = SimpleAnimModern.cpp ShaderMgrSAM.cpp LinearR3.cpp LinearR4.cpp
TARGET = main

BREW_PREFIX = /opt/homebrew

INCLUDES = -I$(BREW_PREFIX)/include
LDFLAGS  = -L$(BREW_PREFIX)/lib -lglfw -lGLEW -lglad -framework OpenGL \

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)



