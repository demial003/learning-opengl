CXX = g++
CXXFLAGS = -std=c++14 -Wno-deprecated-register

SRCS = SolarModern.cpp ShaderMgrSLR.cpp LinearR3.cpp LinearR4.cpp GlGeomBase.cpp GlGeomSphere.cpp
TARGET = main

BREW_PREFIX = /opt/homebrew

INCLUDES = -I$(BREW_PREFIX)/include
LDFLAGS  = -L$(BREW_PREFIX)/lib -lglfw -lGLEW \
           -framework OpenGL -framework Cocoa -framework IOKit

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)



