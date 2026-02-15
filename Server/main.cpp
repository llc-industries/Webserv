/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 15:22:58 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/11 15:26:38 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Server.hpp"

int main()
{
    std::vector<int> ports;
    ports.push_back(8080);
    ports.push_back(9090);
    
    Server webserv;

    std::cout << "[MAIN] Initialising webserv on ports (8080 and 9090)\n";
    webserv.setupServer(ports);
    webserv.run();
}