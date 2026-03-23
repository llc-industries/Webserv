NAME := webserv
CC := c++

CFLAGS := -std=c++98 -Wall -Wextra -Werror -MMD -Iincludes

SRCS := src/parser/ConfigParser.cpp \
	src/parser/ConfigParserUtils.cpp \
	src/parser/ConfigPrint.cpp \
	src/parser/ConfigTokenizer.cpp \
	src/http/HttpRequest.cpp \
	src/http/HttpResponse.cpp \
	src/main.cpp \
	src/server/CgiHandler.cpp \
	src/server/ClientUtils.cpp \
	src/server/Client.cpp \
	src/server/Server.cpp
OBJ_DIR := build
OBJ := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJ:.o=.d)

default: eval

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

eval: all

-include $(DEPS)

.PHONY: all clean fclean re debug
