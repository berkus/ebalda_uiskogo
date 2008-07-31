# add -DDEBUG for debug

ebalda: ebalda.cpp ebalda.h main.cpp config.h
	g++ -g -Wall -Werror -o ebalda main.cpp ebalda.cpp -lgd -lpng -lz -ljpeg -lfreetype -lm
