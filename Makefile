NAME = codexion

SRCS = cleanup.c heap.c main.c monitor.c\
	   routine.c init.c parser.c utils.c

OBJS = $(SRCS:.c=.o)

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean :
	rm -f $(OBJS) $(OBJSB)

fclean : clean
	rm -f $(NAME) $(BNAME)

re : fclean all

.SECONDARY: $(OBJS) $(OBJSB)

.PHONY : all clean fclean re bonus