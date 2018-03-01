g++ -std=c++11 -c GFX.cpp
g++ -std=c++11 -c Entity.cpp
g++ -std=c++11 -c Util.cpp
g++ -std=c++11 -c Camera.cpp
g++ -std=c++11 -c Scene.cpp
g++ -std=c++11 -c Light.cpp

g++ -std=c++11 -lm -o osxray.exe engine.cpp Entity.o GFX.o Util.o Camera.o Scene.o Light.o -framework GLUT -framework OpenGL && ./osxray.exe
