NAME := webserv
CC := c++
# TODO: Add -Werror when done
CFLAGS := -std=c++98 -Wall -Wextra -MMD -Iinc

SRCS := $(shell find src -name "*.cpp")
OBJ_DIR := obj
OBJ := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJ:.o=.d)

default: debug

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

-include $(DEPS)

debug: fclean
debug: CFLAGS += -DDEBUG -g3 # -fsanitize=address,undefined
debug: all

.PHONY: all clean fclean re debug
