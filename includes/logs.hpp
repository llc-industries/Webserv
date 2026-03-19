#ifndef LOGS_HPP
#define LOGS_HPP

#include <iostream>

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[97m"
#define GREY "\033[90m"
#define SILVER "\033[37m"

#define PRINT_HEADER                                                           \
  std::cout << '\n'                                                            \
            << WHITE "██     ██ ▄▄▄▄▄ ▄▄▄▄   ▄▄▄▄ ▄▄▄▄▄ ▄▄▄▄  ▄▄ ▄▄ \n"        \
            << GREY "██ ▄█▄ ██ ██▄▄  ██▄██ ███▄▄ ██▄▄  ██▄█▄ ██▄██ \n"         \
            << BLACK " ▀██▀██▀  ██▄▄▄ ██▄█▀ ▄▄██▀ ██▄▄▄ ██ ██  ▀█▀  \n"        \
            << RESET << std::endl;

// General logs

#define LOG_INFO(x)                                                            \
  std::cout << "[ " << BLUE << "INFO" << RESET << " ] " << x << '\n'

#define LOG_CONFIG(x)                                                          \
  std::cout << "[ " << GREY << "CONFIG" << RESET << " ] " << x << '\n'

#define LOG_WARN(x)                                                            \
  std::cout << "[ " << YELLOW << "WARNING" << RESET << " ] " << x << '\n'

#define LOG_ERR(x)                                                             \
  std::cerr << "[ " << RED << "ERROR" << RESET << " ] " << x << '\n'

// Network logs

#define LOG_ACCEPT(x, y)                                                       \
  std::cout << "[" << x << "] [" << GREEN << "ACCEPT" << RESET << "] " << y    \
            << '\n'

#define LOG_CLOSE(x, y)                                                        \
  std::cout << "[" << x << "] [" << YELLOW << "CLOSE" << RESET << "] " << y    \
            << '\n'

#define LOG_HTTP(fd, method, path, status, bytes, agent)                       \
  std::cout << "[" << fd << "] [" << BLUE << "HTTP" << RESET << "] " << method \
            << " " << path << " --> " << (status > 400 ? RED : GREEN)          \
            << status << RESET << " (" << bytes << "B) | " << agent << '\n'

#endif /* LOGS_HPP */
