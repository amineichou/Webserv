/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:12 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:04:58 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
#define REQUEST_HPP
 
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <algorithm> 
#include <map>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#define BAD_REQUEST_400 1
#define DEBUG_MODE 0
#include <ctime>
#include "../Response/Response.hpp"

class Http_request 
{
    private:
        int                                 crlf_counter;
        int                                 end_of_read;
        int                                 is_header;
        int                                 check_1;
        int                                 again;
        size_t                              chunked_size;
        std::string                         file_name;
        std::string                         method;
        std::string                         request_line;
        std::string                         start_line;
        std::string                         path;
        std::string                         query_string;
        std::string                         path_info;
        std::string                         boundryKey;
        std::string                         endBoundryKey;
        std::ofstream                       file;
        std::ofstream                       cookie_file;
        std::map<std::string, std::string>  header;
        std::string                         body;
        std::string                         chunked_body;
        ServerConfig*                       server_config;
        std::string                         ChunkedFileExtention;
        std::string                         uploadDir;
        std::map<std::string, std::string>  SimpleFormMap;
        std::string                         cookie_id;
        Response*                           response;
        size_t                              bodycounter;
        std::string                         FN_body_receved; // file name of the body recived!!!
        int  get_header(std::string header_key, std::string header_value);
        void check_start_line();
        void parse_path();
        void extract_query_string();
        
        //addby youssef
        void chooseRightconfig(std::vector<ServerConfig *>configs);
        bool isMethodAllowed(const std::vector<std::string> &AcceptedMtd);
        bool isCGIRequest(std::string root);
        Route                               matchedroute;
        std::string                         CGI_extetion;
        std::string                         CGI_interpreter;
        std::string                         cgiScriptPath;
        bool                                isbinary;
        
        public:
        int                                 error;
        bool isCGIrunning;
        void pars_request(std::string request_line, std::vector<ServerConfig *>&configs);
        void pars_header(std::string& request_line);
        void BoundaryBody(std::string request_line);
        void check_boundry_header(size_t pos);
        std::string get_method(void);
        std::string get_query_string(void);
        std::string get_path(void);
        std::string get_body(void);
        void set_boundryKey(void);
        size_t set_crlf_counter(std::string str);
        std::map<std::string, std::string> get_header();
        std::string get_path_info();
        void chunked_and_boundary(std::string request_line);
        size_t set_crlf_ChunkedCounter(std::string body);
        void ChunkedBoundaryBody(std::string request_line);
        void set_ChunkedBoundryKey(void);
        void BinaryChunked(std::string request);
        std::string get_ChunkedFileExtention(void);
        std::string getFileName(void);
        int get_chunked_size();
        int  getEndOfRead(void);
        void SimpleForm(std::string request_line);
        Response *getResponse(void) const;
        ServerConfig *getConfig(void) const;
        void Binary(std::string request_line);
        void set_cookie(void);
        std::string  get_cookie(void);
        size_t getbodycounter();
        bool getIsBinary();
        Http_request();
        ~Http_request();
        
        
        //add by youssef
        void validatingData(std::vector<ServerConfig *>configs);
        Route GetMatchedRoute();
        std::string getCGI_interpreter();
        std::string getCGI_extetion();
        bool getIsCGI();
        std::string getCgiScriptPath(void) const;
        std::string getFN_body_receved();
        time_t                              lastModified;
        void updatesockettime();
        bool                                is_CGI;

};

std::vector<std::string> split(std::string str, char c);
std::string strtrim(std::string str, char c);
std::string get_file_name(std::vector<std::string> v);
std::string generateRandomfileName(size_t length);
#endif