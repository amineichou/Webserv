/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:48:07 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:05:58 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sstream>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include "../config/ServerConfig.hpp"
#include "../Request/request.hpp"
#define MAX_EVENTS 250
#define BUFFER 5120
#define DEBUG_MODE 0
#define CGI_TIMEOUT 10
#define SOCKET_TIMEOUT 70


struct s_cgi
{
    int pid;
    int clientfd;
    int status;
    std::string filename;
    std::string inputfilename;
    time_t start_time; // 5 sec timeout
};

class Server
{
    public:
        Server();
        Server(const Server&);
        ~Server();
        Server& operator=(const Server&);
        void creatsockets(std::vector<ServerConfig> &serverConfig);
        void listenonsockets();
        void configEpool();
    private:
        // int first;
        int epollfd;
        std::map<int , std::vector<ServerConfig*> > Socketlistenfds;  // key = socke
        std::map<int, std::vector<ServerConfig *> >clientsocketfds;
        std::map<int, Http_request*> Requests;
        std::map<int, s_cgi> cgis;
        // void remove_fdremain(int remainfd, std::vector<int> & remainfds);
        void eventsloop(int epollfd);
        void HundleNewConnection(int envfd);
        void HundleClientRequest(int evnfd);
        void HundleClientResponse(int evnfd);
        void connection_alive(int clientfd);

        void cleaningup(int clientfd);

        //change eventfd status to read
        void changeEventToRead(int eventfd);
        void changeEventToWrite(int eventfd);

        //CGI
        std::string GenerateEnvVar(std::string key, std::string value);
        std::vector<std::string> GenerateEnvVars(Http_request *request);
        std::string genratefilename();
        void executeCGI(int clientfd);
        void checksocketTimeout();
        void checkCGITimeout();
};

#endif /*server_hpp*/