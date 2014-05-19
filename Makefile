all:
	g++ -o tb testbench_shkm.cpp syscvideo.cpp -lsystemc `sdl-config --cflags --libs`
