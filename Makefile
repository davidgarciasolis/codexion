NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -Icoders

SRCS = main.c parse.c simulation.c cleanup.c time.c state.c log.c monitor.c \
	queue.c dongle.c scheduler.c developer.c
OBJ_DIR = coders
OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.c coders/codexion.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
