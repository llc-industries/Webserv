/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 21:13:24 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/21 00:42:26 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <sstream>
#include <cstdlib>

class HttpResponse {
private:
    int _status_code;
    std::string _status_message;
    std::map<std::string, std::string> _headers;
    std::string _body;
    
    std::string getReasonPhrase(int code) const;//ex:200=OK 404="NOT FOUND"
public:
    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    //MIME
    void autoDetectContentType(const std::string& path);
    std::string toString() const;
};