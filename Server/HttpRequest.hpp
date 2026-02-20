/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 21:13:05 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/20 23:04:29 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>

class HttpRequest{
    
private:
    std::string _raw_data;
    //attribue parse
    std::string _method;
    std::string _path;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    //etat du parsing
    bool _headers_parsed;
    bool _is_complete;
    size_t _content_length;
    //ft de parsing de requete
    void parseFirstLine(const std::string& line);
    void parseHeaderLine(const std::string& line);
    
public:
    HttpRequest();
    ~HttpRequest();
    //recv()
    void swallow(const char* buffer, size_t bytes);
    //getters
    bool isComplete() const;
    std::string getMethod() const;
    std::string getPath() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;
    
};