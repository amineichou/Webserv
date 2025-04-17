/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:06 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:41:47 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "request.hpp"


Http_request::Http_request()
{

     this->error = 0;
     this->body.clear();
     this->file_name.clear();
     this->crlf_counter = 0;
     this->chunked_body.clear();
     this->end_of_read = 0;
     this->is_header = 0;
     this->chunked_size = 0;
     this->check_1 = 0;
     this->is_CGI = false;
    this->isCGIrunning = false;
    this->isbinary = false;
    this->bodycounter = 0;

    this->response = new Response;
    this->lastModified = time(NULL);
}   

void Http_request::updatesockettime(void)
{
   this->lastModified = time(NULL);
}

void Http_request::pars_request(std::string request_line, std::vector<ServerConfig *>&configs)
{
      try
    {
        std::string &request = request_line;
        if (this->is_header == 0)
            {
                this->pars_header(request);
                this->is_header = 1;
                this->validatingData(configs);
            }
        if (this->get_method() == "POST")
        {
            if (header.find("Transfer-Encoding") != header.end() && header["Transfer-Encoding"].find("chunked") != std::string::npos)
            {
                if (header.find("Content-Type") != header.end() && header["Content-Type"].find("multipart/form-data") != std::string::npos)
                {
                    this->chunked_and_boundary(request);
                }
                else
                {
                    this->BinaryChunked(request);
                }

            }
            else if (header.find("Content-Type") != header.end() && header["Content-Type"].find("multipart/form-data") != std::string::npos)
            {
                this->BoundaryBody(request);
            }
            else if (header.find("Content-Type") != header.end() && header["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
            {
                this->SimpleForm(request_line);
            }
            else
                this->Binary(request);

        }
}
    catch(const char* err )
    {
        std::string error_code(err);
        DEBUG_MODE && std::cout <<"error number : " << error_code<<std::endl;
        this->end_of_read = 1;
        this->getResponse()->setStatusCode(error_code);
    }
}
int issspace(std::string str, int *i)
{
    int f = 1;
    while (str[*i] == '\n' || str[*i] == '\r'
            || str[*i] == ' ' || str[*i] == '\t'
            || str[*i] == '\v' || str[*i] == '\f')
        {
            (*i)++;
            f = 0;
        }
    return (f);
    
}

/**** [ Host  :localhost |Error| Host: localhost]  ******/

std::string  skip_spaces(std::string str)
{
    int i = 0;
   
    std::string result;
    result.clear();
    int  j = str.size();
    while (str[i])
    {
        if (str[i] == '\t' || str[i] == ' ' || str[i] == '\v')
         break ;
        i++;
    }
    if (i != j)
        return (result);
    return (str.substr(0 , i - 1));

    return (str.substr(0, i));
}

std::string get_key_value(std::string line , char c)
{
    size_t pos;
    std::string header_key;
    std::string header_value;
    if (!line.empty())
    {
        pos = line.find(':');
    if (pos != std::string::npos && pos != 0)
    {
        if (c == 'K')
        {
            header_key = line.substr(0, pos);                                                                                                                                                                                                                                                  
            return (skip_spaces(header_key + ":"));
        }
        else
            {
                 header_value = line.substr(pos + 1);
                 return (header_value);
            }
    }
   }
    return (header_key);

}


int Http_request::get_header(std::string header_key, std::string header_value)
{
    std::map<std::string, std::string>::iterator it = this->header.find(header_key);
    if (it != this->header.end()) 
    {
        this->error = BAD_REQUEST_400;
        throw "400";
    }
    else 
    {
        this->header[header_key] = header_value;
    }
    return 1;

}
void Http_request::set_cookie(void)
{
    std::string cookie;
    size_t pos;
   
    if (!this->header["Cookie"].empty())
        {
            cookie = this->header["Cookie"].substr(0, this->header["Cookie"].size() - 1);
            //this->cookie_id = cookie;
    }
    pos = cookie.find("SESSIONID=");
    if (pos != std::string::npos)
      {
            cookie = cookie.substr(pos + 10);
           
        }
    pos = cookie.find(";");
    if (pos != std::string::npos)
        cookie = cookie.substr(0, pos);
    this->cookie_id = cookie;
}

/*Every line in request must end with \r\n*/


void Http_request::pars_header(std::string& request_line)
{
    std::istringstream request_stream(request_line);
    std::string line;
    int flag = 0;
    size_t i = 0;
    size_t pos = 0;
    std::getline(request_stream, line);
    i = line.size();
    if (request_line[i - 1] == '\r' && request_line[i] == '\n')
        this->start_line =  line ;
   // I have to parse the start ligne;
    this->check_start_line();
    while (std::getline (request_stream, line) && !error)
    {
        if (line.empty())
            break;
        i += (line.size() + 1);
        if (flag == 0 && !line.empty() && (i > 0 && i < request_line .length ())
            && request_line[i - 1] == '\r' && request_line[i] == '\n')
        {
            if (line == "\r")
                {
                    flag = 1;
                    break;
                }
            pos = line.find('\r');
            if (pos != std::string::npos)
                {

                    std::string header_key = get_key_value(line, 'K');
                   std::string header_value = get_key_value(line, 'V');
                    if (header_key.empty() || header_value.empty())
                    {
                        this->error = BAD_REQUEST_400;
                        throw "400";
                    }
                // this check for the key that appear more thane once
                if (!this->get_header(header_key, header_value))
                    {
                        this->error =  BAD_REQUEST_400;
                        throw "400";
               
                    }
                }
        }
    }
    if (this->header["Host"].empty() && !error)
         {
            this->error = BAD_REQUEST_400;
            throw "400";
        }
    if (flag == 0 && !error)
    {
        this->error = BAD_REQUEST_400;
        throw "400";
    }
    set_cookie();
    request_line = request_line.substr(i - 1, request_line.length());
}


//check method is POST after I have to get the body ;


int get_path_f_sl(std::string& str , size_t i, size_t pos , std::string& path)
{
    if (str[i] != '/')
    {
        DEBUG_MODE && std::cout << "Bad request path1\n";
        throw "400";
    }
    else
    {
        pos = str.find(" ");
        if (pos != std::string::npos)
        {
            path = str.substr(i , pos);
            str = str.substr(pos + 1, str.size());
        }
        else 
            {
                DEBUG_MODE && std::cout << "Bad request path2\n";
                throw "400";
            }
    }
    return (1);
}

int check_http_version(std::string& str)
{

    str = str.substr(0, (str.size() - 1));
    if (str == "HTTP/1.1")
        {
            return (1);
        }
    else
    {
        DEBUG_MODE && std::cout << "Bad request http version\n";
        throw "400";
    }    
}


void ft_replace(std::string s1, std::string s2, std::string& buffer)
{
    int pos;
    pos = buffer.find(s1, 0);
    while (std::string::npos != (size_t)pos)
    {
        buffer.erase(pos, s1.length());
        buffer.insert(pos, s2);
        pos = buffer.find(s1, pos + s2.length());
    }
}



std::string& url_encoding(std::string path)
{
    std::string &url = path;

    ft_replace("%20", " ", url);
    ft_replace("%21", "!", url);
    ft_replace("%22", "\"", url);
    ft_replace("%23", "#", url);
    ft_replace("%24", "$",  url);
    ft_replace("%25", "%", url);
    ft_replace("%26", "&", url);
    ft_replace("%27", " \'", url);
    ft_replace("%28", "(", url);
    ft_replace("%29", ")", url);
    ft_replace("%2A", "*", url);
    ft_replace("%2B", "+", url);
    ft_replace("%2C", ",", url);
    ft_replace("%2D", "-", url);
    ft_replace("%2E", ".", url);
    ft_replace("%2F", "/", url);
    ft_replace("%3A", ":", url);
    ft_replace("%3B", ";", url);
    ft_replace("%3C", "<", url);
    ft_replace("%3D", "=", url);
    ft_replace("%3E", ">", url);
    ft_replace("%3F", "?", url);
    ft_replace("%40", "@", url);
    ft_replace("%5B", "[", url);
    ft_replace("%5C", "\\", url);
    ft_replace("%5D", "]", url);
    ft_replace("%5E", "^", url);
    ft_replace("%5F", "_", url);
    ft_replace("%60", "`", url);
    ft_replace("%7B", "{", url);
    ft_replace("%7C", "|", url);
    ft_replace("%7D", "}", url);
    ft_replace("%7E", "~", url);
    return (url);
}



std::string& ignore_fragment(std::string path)
{
    std::string &url = path;
    size_t pos = path.find('#');
    if (pos != std::string::npos)
        url = url.substr(0 , pos);
    return (url);
}

std::string find_query(std::string& url)
{
    std::string query_string = url;
    size_t      len = url.length();
    size_t pos = query_string.find('?');
    if (pos != std::string::npos)
        query_string = query_string.substr(pos + 1, len);
    else
        return ("");
    url = url.substr(0, pos);
    return (query_string);
}


void Http_request::extract_query_string()
{
    std::string& url = this->path;
    /* To add if there is  more than one ?*/
    this->query_string = find_query(url);
}



/*  Everrthing after the fragment must be ignored #wefwe  */

void Http_request::parse_path()
{
    this->path = url_encoding(this->path);
    this->path = ignore_fragment(this->path);
    this->extract_query_string();
}


void Http_request::check_start_line()
{
 
    int             i   = 0;
    size_t         pos = 0;
   
    std::string path;
    std::string str;
    str = this->start_line;
    if (!issspace(str, &i))
        {
           // DEBUG_MODE && std::cout << "Bad REquest start line\n";
            this->error = BAD_REQUEST_400;
            throw "400";

        }
    pos = str.find(" ");
    if (pos != std::string::npos)
       {
            this->method = str.substr(0, pos);
            str = str.substr(pos + 1, str.size());
       }
    else
    {
        this->error = BAD_REQUEST_400;
        throw "400";
    }

    issspace(str, &i);
    if (get_path_f_sl(str, i, pos, path))
        {
            this->path = path;
            this->parse_path();
        }
    else
        {
            this->error = BAD_REQUEST_400;
            throw "400";
        } ;
    if (!check_http_version(str))
        throw "400";
}


Http_request::~Http_request()
{
    delete this->response;
}
