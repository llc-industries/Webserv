#ifndef LOGS_HPP
#define LOGS_HPP

#include <iostream>

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#ifdef DEBUG

#define LOG_INFO(x)                                                            \
  std::cout << "[ " << BLUE << "INFO" << RESET << " ] " << x << '\n'

#define LOG_WARN(x)                                                            \
  std::cout << "[ " << YELLOW << "WARNING" << RESET << " ] " << x << '\n'

#define LOG_ERR(x)                                                             \
  std::cerr << "[ " << RED << "ERROR" << RESET << " ] " << x << " ("           \
            << __FILE__ << ":" << __LINE__ << ")" << std::endl

#else

#define LOG_INFO(x)
#define LOG_WARN(x)
#define LOG_ERR(x)

#endif

#endif /* LOGS_HPP */
