SRC=main.cpp\
	audio.cpp\
	gui.cpp

fmseq: ${SRC}
	g++ ${SRC} -lSDL2 -o fmseq