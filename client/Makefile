SRC = client.cpp
OBJ = $(SRC:.cpp=.o)

CC = c++
CFLAGS = -Wextra -Wall -Werror

NAME = ../chat_client

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all : $(NAME)

clean :
	rm -rf $(OBJ)

fclean :
	rm -rf $(OBJ) $(NAME)
				
re : 
	make fclean all