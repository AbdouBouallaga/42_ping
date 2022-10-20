NAME = ft_ping
SRC = ./src/ft_ping.c
OBJ	= $(SRC:.c=.o)
CFLAGS	=-Wall -Wextra -Werror
# CFLAGS	=
P_HEADER = ./inc/ft_ping.h
FT_LIB = ./libft/libft.a

all: $(FT_LIB) $(NAME)

$(FT_LIB): 
	@make -C  ./libft

$(OBJ): $(SRC) $(P_HEADER)
	gcc $(CFLAGS) -o $(OBJ) -c $(SRC)

$(NAME): $(OBJ) $(P_HEADER)
	gcc $(CFLAGS) -o $(NAME) $(OBJ) $(FT_LIB)


clean:
	@make -C  ./libft clean
	-rm $(OBJ)

fclean: clean
	@make -C  ./libft fclean
	-rm $(NAME)

re: fclean all
