#include "debug.hpp"

#include <iostream>
#include <stdlib.h>

void handleArgvLogs(int argc, char **argv) {
	if (argc < 2)
		LOG_WARN("No arguments provided, looking for ./webserv.conf");
	else if (argc > 2)
		LOG_WARN("Too many arguments, using \"" << argv[1]
												<< "\" as config file");
}

int main(int argc, char **argv, char **env) {
	handleArgvLogs(argc, argv);
	return EXIT_SUCCESS;
}
