
args = -Wno-sign-conversion `wx-config --cxxflags --libs` -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference -g3 -O0
# 

# -lstdc++ is needed when getting a bunch of undefined refrence error because it includes many basic C++ libraries
tot: main.o editor.o
	g++ -o pavea editor.o main.o `wx-config --libs`

# wx-config is super useful it probides all of our libraries and linker stuff
andRun: main.o editor.o
	rm main
	g++ -o main editor.o main.o `wx-config --libs`
	./main


editor.o: editor.cpp
	g++ -std=c++20 -c $(args) editor.cpp $(pkg-config --cflags libavcodec libavformat libavutil libswscale) $(pkg-config --libs libavcodec libavformat libavutil libswscale)

main.o: main.cpp
	g++ -c $(args) main.cpp

clean:
	rm main.o
	rm editor.o


# 