# make -C ./libft/ all bonus

all :
	make -C ./server all
	make -C ./client all

clean :
	make -C ./server clean
	make -C ./client clean


fclean :
	make -C ./server fclean
	make -C ./client fclean
				
re : 
	make -C ./server re
	make -C ./client re