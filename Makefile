NAME = codexion

SRCS = cleanup.c heap.c main.c monitor.c coder_work.c\
	   routine.c init.c parser.c utils.c routine_helpers.c\

OBJS = $(SRCS:.c=.o)

CC = gcc

CFLAGS = -Wall -Wextra -Werror -pthread

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -fsanitize=thread -g -O1 $(CFLAGS) $(OBJS) -o $(NAME)

clean :
	rm -f $(OBJS) $(OBJSB)

fclean : clean
	rm -f $(NAME) $(BNAME)

re : fclean all

.SECONDARY: $(OBJS) $(OBJSB)

.PHONY : all clean fclean re bonus