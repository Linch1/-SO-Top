CC = gcc
CFLAGS = -w -Wall -Wextra -Werror -pedantic -std=c11
LDFLAGS = -lncurses -pthread
SRC = main.c pid_info.c pid_stats.c linked_list.c
OBJ = $(SRC:.c=.o)
TARGET = prova_fin

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	echo "lavo"
	rm -rf *.o *~ $(OBJ) $(TARGET)
