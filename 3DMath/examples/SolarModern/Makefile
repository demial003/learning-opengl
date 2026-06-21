CXX = g++
CXXFLAGS = -std=c++14 -Wno-deprecated-register

SRCS = SolarModern.cpp ShaderMgrSLR.cpp LinearR3.cpp LinearR4.cpp GlGeomBase.cpp GlGeomSphere.cpp
TARGET = main


LDFLAGS  = -lglfw -lGLEW -lglad \

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)



