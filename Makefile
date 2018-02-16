#gcc main.c $(pkg-config --cflags --libs gtk+-3.0) -o DidItChange -lcurl

NAME = DidItChange
CC = gcc
LIBCURL = -lcurl
CFLAGS = -g -c -Wall -Werror
LD_FLAGS = -Wall $(LIBCURL)
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
