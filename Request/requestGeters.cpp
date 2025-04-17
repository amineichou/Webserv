/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestGeters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymomen <ymomen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:19 by ymomen            #+#    #+#             */
/*   Updated: 2025/03/04 23:23:20 by ymomen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "request.hpp"

std::string Http_request::get_method(void)
{
    return (this->method);
}

std::string Http_request::get_query_string(void)
{
    return (this->query_string);
}

std::string Http_request::get_path(void)
{
    return (this->path);
}

std::string Http_request::get_path_info(void)
{
    return (this->path_info);
}

std::map<std::string, std::string> Http_request::get_header(void)
{
    return (this->header);
}

std::string Http_request::getFileName(void)
{
    return (this->file_name);
}
int Http_request::getEndOfRead(void)
{
    return (this->end_of_read);
}
Response *Http_request::getResponse(void) const
{
    return this->response;
}

ServerConfig *Http_request::getConfig(void) const
{
    return this->server_config;
}
std::string Http_request ::get_cookie(void)
{
    return (this->cookie_id);
}
std::string Http_request::getCGI_extetion()
{
    return this->CGI_extetion;
}

std::string Http_request::getCGI_interpreter()
{
    return this->CGI_interpreter;
}

bool Http_request::getIsCGI(void)
{
    return this->is_CGI;
}

size_t Http_request::getbodycounter(void)
{
    return this->bodycounter;
}

bool Http_request::getIsBinary(void)
{
    return this->isbinary;
}

std::string Http_request::getCgiScriptPath(void) const
{
    return this->cgiScriptPath;
}
std::string Http_request:: getFN_body_receved()
{
    return this->FN_body_receved;
}
