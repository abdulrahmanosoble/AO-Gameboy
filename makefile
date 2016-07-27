all:
	g++ -std=c++14 core.cpp execute.cpp -o emulator -lsfml-graphics -lsfml-window -lsfml-system