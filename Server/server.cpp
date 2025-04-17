/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:48:15 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:23:18 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(const Server &server)
{
    *this = server;
}

Server::~Server()
{
}

Server::Server()
{
    this->epollfd = -1;
}

Server &Server::operator=(const Server &server)
{
    if (this == &server)
        return *this;
    return *this;
}

// creat sockets and make sure to open only one socket per (port and host) means if
void Server::creatsockets(std::vector<ServerConfig> &serverConfig)
{
    std::map<std::pair<std::string, int>, int> socketMap;

    for (int i = 0; i < ServerConfig::serversCount; i++)
    {
        std::vector<std::pair<std::string, int> > config = serverConfig[i].getPorts();
        for (size_t k = 0; k < config.size(); k++)
        {
            std::pair<std::string, int> hostPortPair = config[k];
            if (socketMap.find(hostPortPair) == socketMap.end())
            {
                int socketfd = socket(AF_INET, SOCK_STREAM, 0);
                if (socketfd < 0)
                    throw std::runtime_error("socket failed to create fd");
                if (fcntl(socketfd, F_SETFL, O_NONBLOCK) < 0)
                    throw std::runtime_error("fcntl failed");
                int opt = 1;
                setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
                struct sockaddr_in serveradd;
                memset(&serveradd, 0, sizeof(serveradd));
                serveradd.sin_family = AF_INET;
                if (hostPortPair.first == "" || hostPortPair.first == "0.0.0.0")
                    serveradd.sin_addr.s_addr = INADDR_ANY;
                else
                {
                    serveradd.sin_addr.s_addr = inet_addr(hostPortPair.first.c_str());
                    if (serveradd.sin_addr.s_addr == INADDR_NONE)
                    {
                        close(socketfd);
                        throw std::runtime_error("invalid ip address");
                    }
                }
                serveradd.sin_port = htons(hostPortPair.second);
                if (bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
                {
                    close(socketfd);
                    throw std::runtime_error("bind failed");
                }

                socketMap[hostPortPair] = socketfd;
                Socketlistenfds[socketfd].push_back(&serverConfig[i]);
            }
            else
            {
                Socketlistenfds[socketMap[hostPortPair]].push_back(&serverConfig[i]);
            }
        }
    }
}

void Server::listenonsockets()
{
    for (std::map<int, std::vector<ServerConfig *> >::iterator it = Socketlistenfds.begin(); it != Socketlistenfds.end(); it++)
    {
        if (listen(it->first, SOMAXCONN) < 0)
        {
            close(it->first);
            throw std::runtime_error("listen failed");
        }
    }
}

void Server::configEpool()
{
    epollfd = epoll_create(1);
    if (epollfd < 0)
        throw std::runtime_error("epoll failed");
    struct epoll_event event;
    for (std::map<int, std::vector<ServerConfig *> >::iterator it = Socketlistenfds.begin(); it != Socketlistenfds.end(); it++)
    {
        int socketfd = it->first;
        event.events = EPOLLIN;
        event.data.fd = socketfd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event) < 0)
        {
            close(socketfd);
            throw std::runtime_error("epoll_ctl failed");
        }
    }
    eventsloop(epollfd);
}

void Server::changeEventToRead(int eventfd)
{
    struct epoll_event event;
    event.data.fd = eventfd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, eventfd, &event) < 0)
    {
        DEBUG_MODE &&std::cerr << "can not change event to read" << std::endl;
        close(eventfd);
        return;
    }
}

void Server::changeEventToWrite(int eventfd)
{
    struct epoll_event event;
    event.data.fd = eventfd;
    event.events = EPOLLOUT;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, eventfd, &event) < 0)
    {
        DEBUG_MODE &&std::cerr << "can not change event to write" << std::endl;
        close(eventfd);
        return;
    }
}

void Server::cleaningup(int clientfd)
{
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, NULL) < 0)
        DEBUG_MODE &&std::cerr << "can not delete fd from epoll" << std::endl;
    if (Requests.find(clientfd) == Requests.end())
    {
        DEBUG_MODE &&std::cerr << "can not find fd in requests <already deleted >." << std::endl;
        return;
    }
    delete this->Requests[clientfd];
    this->Requests.erase(clientfd);
    clientsocketfds.erase(clientfd);
    close(clientfd);
}
void Server::connection_alive(int clientfd)
{
    delete this->Requests[clientfd];
    this->Requests.erase(clientfd);
    this->Requests[clientfd] = new Http_request();
}

void Server::checksocketTimeout()
{
    time_t current_time = time(NULL);
    std::map<int, Http_request *>::iterator it = Requests.begin();
    while (it != Requests.end())
    {
        int clientfd = it->first;
        if (difftime(current_time, it->second->lastModified) > SOCKET_TIMEOUT)
        {
            DEBUG_MODE &&std::cerr << "socket timeout" << std::endl;
            cleaningup(clientfd);

            it = Requests.lower_bound(clientfd);
        }
        else
        {
            ++it;
        }
    }
}

void Server::eventsloop(int epollfd)
{
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        int event_cont = epoll_wait(epollfd, events, MAX_EVENTS, 0);
        for (int i = 0; i < event_cont; i++)
        {
            int eventfd = events[i].data.fd;
            if (clientsocketfds.find(eventfd) != clientsocketfds.end())
            {
                if (events[i].events & EPOLLIN) // request from client
                {
                    HundleClientRequest(eventfd);
                }
                if (events[i].events & EPOLLOUT) // send response to client
                {
                    HundleClientResponse(eventfd);
                }
                else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
                {
                    DEBUG_MODE &&std::cerr << "Cleaning up fd: " << eventfd << std::endl;
                    cleaningup(eventfd);
                }
            }
            else
                HundleNewConnection(eventfd); // accpt new socket
        }
        checkCGITimeout();
        checksocketTimeout();
    }
}

void Server::HundleClientRequest(int clientfd)
{
    try
    {
        char buffer[BUFFER];
        memset(buffer, 0, BUFFER);
        int bytesRead = recv(clientfd, buffer, BUFFER, 0);
        std::string recvBuff(buffer, bytesRead);
        if (bytesRead > 0)
        {
            this->Requests[clientfd]->pars_request(recvBuff, clientsocketfds[clientfd]);
            this->Requests[clientfd]->updatesockettime();
            if (Requests[clientfd]->getIsCGI() && !Requests[clientfd]->isCGIrunning && Requests[clientfd]->getResponse()->getStatusCode() == "000")
            {
                Requests[clientfd]->isCGIrunning = true;
                executeCGI(clientfd);
                DEBUG_MODE &&DEBUG_MODE &&std::cout << "executing cgi" << std::endl;
            }
        }
        else if (bytesRead == 0)
        {
            if (Requests[clientfd]->getIsCGI() && Requests[clientfd]->isCGIrunning)
            {
                s_cgi cgi = cgis[clientfd];
                if (kill(cgi.pid, SIGKILL) < 0)
                    DEBUG_MODE &&std::cerr << "can not kill cgi process" << std::endl;
                if (unlink(cgi.filename.c_str()) < 0)
                    DEBUG_MODE &&std::cerr << "can not unlink file" << std::endl;
                if (unlink(cgi.inputfilename.c_str()) < 0)
                    DEBUG_MODE &&std::cerr << "cant unlink infile" << std::endl;
                cgis.erase(clientfd);
            }
            cleaningup(clientfd);
            DEBUG_MODE &&std::cerr << "client closed connection" << std::endl;
            return;
        }
        else
        {
            DEBUG_MODE &&std::cerr << "can not read from client" << std::endl;
            cleaningup(clientfd);
            return;
        }
        if (this->Requests[clientfd]->getEndOfRead() && !this->Requests[clientfd]->isCGIrunning)
        {
            changeEventToWrite(clientfd);
        }
        std::stringstream ss(Requests[clientfd]->get_header()["Content-Length"]);
        size_t contentLength;
        ss >> contentLength;

        if (Requests[clientfd]->get_method() == "POST" && (this->Requests[clientfd]->getbodycounter() >= contentLength) && !this->Requests[clientfd]->isCGIrunning && Requests[clientfd]->getIsBinary())
        {
            changeEventToWrite(clientfd);
        }
    }
    catch (...)
    {
        cleaningup(clientfd);
    }
}

void Server::HundleClientResponse(int eventfd)
{
    try
    {
        if (!(Requests[eventfd]->getResponse()->isRunning()))
        {
            if (Requests[eventfd]->getIsCGI() && !Requests[eventfd]->isCGIrunning)
            {
                if (unlink(Requests[eventfd]->getResponse()->GetfileToDelete().c_str()) < 0)
                    DEBUG_MODE &&std::cerr << "cant unlink file" << std::endl;
                cgis.erase(eventfd);
            }
            if (Requests[eventfd]->get_header()["Connection"] == " keep-alive\r")
            {
                DEBUG_MODE &&std::cout << "\033[31m" << "keep alive connection" << "\033[0m" << std::endl;
                connection_alive(eventfd);
                changeEventToRead(eventfd);
            }
            else
            {
                DEBUG_MODE &&std::cout << "\033[32m" << "close connection" << "\033[0m" << std::endl;
                cleaningup(eventfd);
            }
            return;
        }
        std::string status = Requests[eventfd]->getResponse()->getStatusCode();
        if (status != "000" && status != "200")
        {
            Requests[eventfd]->getResponse()->setClientFd(eventfd);
            Requests[eventfd]->getResponse()->onError(status);
            return;
        }

        if (Requests[eventfd]->get_method() == "GET")
            Requests[eventfd]->getResponse()->GetMethod(eventfd);
        else if (Requests[eventfd]->get_method() == "DELETE")
            Requests[eventfd]->getResponse()->DeleteMethod(eventfd);
        else if (Requests[eventfd]->get_method() == "POST")
            Requests[eventfd]->getResponse()->PostMethod(eventfd);
        Requests[eventfd]->updatesockettime();
    }
    catch (...)
    {
        cleaningup(eventfd);
    }
}

void Server::HundleNewConnection(int eventfd)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    int clientfd = accept(eventfd, (struct sockaddr *)&clientaddr, &addrlen);
    if (clientfd < 0)
    {
        DEBUG_MODE &&std::cerr << "can not accept connection from fd" << eventfd << std::endl;
        return;
    }
    Http_request *request = new Http_request();
    this->Requests[clientfd] = request;
    if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0)
    {
        delete request;
        this->Requests.erase(clientfd);
        DEBUG_MODE &&std::cerr << "can not set the socket on non-blocking fd" << clientfd << std::endl;
        close(clientfd);
        return;
    }
    struct epoll_event event;
    event.data.fd = clientfd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &event) < 0)
    {
        delete request;
        this->Requests.erase(clientfd);
        DEBUG_MODE &&std::cerr << "can not add clientfd to epoll" << clientfd << std::endl;
        close(clientfd);
        return;
    }
    clientsocketfds[clientfd] = Socketlistenfds[eventfd];
}