#certainly not the most efficient makefile, but it works for such a small program

macro: Entrypoint.o Keyboard.o KeyMap.o KeyTable.o VirtualKeyboard.o
	g++ -o macro Entrypoint.o Keyboard.o KeyMap.o KeyTable.o VirtualKeyboard.o
	chmod +x macro
	rm Entrypoint.o Keyboard.o KeyMap.o KeyTable.o VirtualKeyboard.o

Entrypoint.o: ./src/Entrypoint.cpp
	g++ -c ./src/Entrypoint.cpp -I./include

Keyboard.o: ./src/Keyboard.cpp
	g++ -c ./src/Keyboard.cpp -I./include

KeyMap.o: ./src/KeyMap.cpp
	g++ -c ./src/KeyMap.cpp -I./include

KeyTable.o: ./src/KeyTable.cpp
	g++ -c ./src/KeyTable.cpp -I./include

VirtualKeyboard.o: ./src/VirtualKeyboard.cpp
	g++ -c ./src/VirtualKeyboard.cpp -I./include