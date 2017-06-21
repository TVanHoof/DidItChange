CC = gcc
GTK_FLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
LIBCURL = -lcurl
CFLAGS = -g -c -Wall -Werror $(GTK_FLAGS)
LD_FLAGS = -Wall $(GTK_LIBS) $(LIBCURL)
OBJS = main.o

OUTPUT_DIR = _build

MK = mkdir
RM = rm -rf

.PHONY = clean

DidItChange: $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) -o DidItChange $(LIBCURL)

$(OUTPUT_DIR)/main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	$(RM) *.o main
