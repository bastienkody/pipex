HEADER = ./inc/pipex.h

SRC_DIR = ./src/

SRCS_NAME =	main.c\
		parsing.c\
		parser.c\
		files.c\
		printers.c\
		t_cmd_utils.c\
		execution.c\
		aux.c\
		aux2.c

SRCS = $(addprefix ${SRC_DIR}, ${SRCS_NAME})

OBJS =	${SRCS:.c=.o}

NAME =	pipex

LIBFT = ./libft/libft.a 

CC =	cc

CFLAGSDEV =	-Wall -Wextra -Werror -g3

CFLAGS =	-Wall -Wextra -Werror

LDFLAGS =	-L./libft -lft

.c.o:	
		@echo "\033[32m\c"
		${CC} ${CFLAGS} -c $< -o ${<:.c=.o}
		@echo "\033[0m\c"

all:	${NAME}

${NAME}:	${OBJS} ${HEADER} ${LIBFT}
			@echo "\033[32m\c"
			${CC} -o ${NAME} ${OBJS} ${LDFLAGS}
			@echo "Link complete for exec --> \033[4;36;1m${NAME}\033[0m"

${LIBFT}:
			@echo "\033[33mlibft compilation ...\033[0m"
			@make --no-print-directory -C libft/
			@echo "\033[33mlibft compiled\033[0m"

clean:
		@rm -rf ${OBJS}
		@echo "\033[32m${NAME} obj cleaned"
		@make --no-print-directory clean -C libft/

fclean:		clean
		@rm -rf ${NAME}
		@echo "\033[32m${NAME} fcleaned"
		@make --no-print-directory fclean -C libft/

re:		fclean all

.PHONY:	all clean re
