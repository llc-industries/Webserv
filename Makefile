NAME := webserv
CC := c++

CFLAGS := -std=c++98 -Wall -Wextra -Werror -MMD -Iincludes

SRCS := $(shell find src -name "*.cpp")
OBJ_DIR := build
OBJ := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJ:.o=.d)

default: vg

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

vg: CFLAGS += -g3
san: CFLAGS += -g3 -fsanitize=address,undefined
eval: CFLAGS += -O2

vg: re
	valgrind --leak-check=full --track-fds=yes --trace-children=yes --show-leak-kinds=all ./$(NAME)

san: re
	./$(NAME)

eval: re

-include $(DEPS)

.PHONY: all clean fclean re debug
