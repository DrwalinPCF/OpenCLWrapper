
CXX = g++
CXXFLAGS = -m64 -Ofast -s -w
LIBS = -L"C:\Intel\OpenCL\sdk\lib\x64" -lOpenCL
INCLUDES = -I"C:\Intel\OpenCL\sdk\include"

OpenCLWrapper.dll: .\\OpenCLWrapper.o
	$(CXX) -shared $< -o $@ $(CXXFLAGS) $(LIBS) $(INCLUDES)

.\\OpenCLWrapper.o: .\\src\\OpenCLWrapper.cpp .\\src\\OpenCLWrapper.h
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS) $(INCLUDES)
