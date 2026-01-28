#include "../inc/logs.hpp"
#include "../inc/server.hpp"

#include <iostream>
#include <stdlib.h>

int main(int argc, char **argv, char **env) {
	if (argc != 2) {
		std::cerr << "Arguments error\n";
		return EXIT_FAILURE;
	}
	std::cout << "Hello, world!\n";
	return EXIT_SUCCESS;
}
