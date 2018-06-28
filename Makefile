#gcc main.c $(pkg-config --cflags --libs gtk+-3.0) -o DidItChange -lcurl

NAME = DidItChange
CC = gcc
GTK_FLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
LIBCURL = -lcurl
CFLAGS = -c -Wall -Werror $(GTK_FLAGS)
LD_FLAGS = -Wall $(GTK_LIBS) $(LIBCURL)
OBJS = main.o

OUTPUT_DIR = _build

MK = mkdir
RM = rm -rf

.PHONY = clean

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LD_FLAGS)

$(OUTPUT_DIR)/main.o: main.c
	$(CC) $(CFLAGS) main.c

#single line works
#$(NAME): main.c
	#$(CC) main.c $(CFLAGS) -o $(NAME) $(LD_FLAGS)

clean:
	$(RM) *.o *.html $(NAME)
