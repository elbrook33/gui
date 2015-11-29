gcc main.c \
	nanovg/build/libnanovg.a \
	-Inanovg/src/ \
	-lGL -lm -lGLEW \
	-lglfw \
	--std=c99
