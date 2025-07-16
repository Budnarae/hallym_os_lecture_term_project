SRC = channel.cpp chat_server.cpp command.cpp exception.cpp parse.cpp safe_syscall.cpp shared_memory.cpp sig_handler.cpp
OBJ = $(SRC:.cpp=.o)

CC = c++
CFLAGS = -Wextra -Wall -Werror -g -fsanitize=address

NAME = ../chat_server

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