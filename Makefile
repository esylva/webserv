NAME	= webserv
TEST	= test.sh
LEAKS	= valgrind

INC		:= Webserv Server Location Connection HTTPStatus Request Response Logger util Lexer
INC		:= $(addsuffix .hpp, $(INC))

SRC		:= main Webserv Server Location Connection HTTPStatus Request Response Logger util Lexer
SRC		:= $(addsuffix .cpp, $(SRC))

CC		= c++
CFLAG	= -Wall -Werror -Wextra -std=c++98 -g
INC_P	= ./inc
SRC_P	= ./src
OBJ_P	= ./obj
OBJ		= $(addprefix $(OBJ_P)/, $(SRC:.cpp=.o))
TEST_P	= ./test

NOC		= \033[0m
BLUE	= \033[1;34m
GREEN	= \033[1;32m
RED		= \033[1;31m

all: $(NAME)

$(NAME): $(OBJ) Makefile
	@echo "$(BLUE)Link $@$(NOC)"
	@$(CC) $(OBJ) -o $@

$(OBJ): $(OBJ_P)/%.o: $(SRC_P)/%.cpp $(addprefix $(INC_P)/, $(INC)) Makefile
	@echo "$(BLUE)Compile $@$(NOC)"
	@mkdir -p $(OBJ_P)
	@$(CC) $(CFLAG) -I$(INC_P) -o $@ -c $<

clean:
	@echo "$(RED)rm -rf $(OBJ_P)/*$(NOC)"
	@rm -rf $(OBJ_P)/*

fclean:	clean
	@echo "$(RED)rm -rf $(OBJ_P)$(NOC)"
	@rm -rf $(OBJ_P)
	@echo "$(RED)rm -f $(NAME)$(NOC)"
	@rm -f $(NAME)

re: fclean all

run: $(NAME)
	rm -rf ./files/*
	@echo "$(GREEN)Run $(NAME)$(NOC)"
	@./$(NAME) ./webserv.conf

test: $(NAME)
	@-pkill $(NAME)
	@echo "$(GREEN)Run $(NAME)$(NOC)"
	@./$(NAME) ./webserv.conf 1>log/log.txt 2>&1 &
	@echo "$(GREEN)Run $(TEST_P)/$(TEST)$(NOC)"
	@bash $(TEST_P)/$(TEST)
	@-pkill $(NAME)

leaks: $(NAME)
	@echo "$(GREEN)Leaks $(NAME)$(NOC)"
	@$(LEAKS) ./$(NAME) ./webserv.conf

.PHONY: all clean fclean re run test leaks
