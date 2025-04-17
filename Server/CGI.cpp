/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:21:45 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 14:33:08 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "server.hpp"

std::string Server::GenerateEnvVar(std::string key, std::string value)
{
    return key + "=" + value;
}

std::vector<std::string> Server::GenerateEnvVars(Http_request *request)
{
    std::vector<std::string> envVars;
    envVars.push_back(GenerateEnvVar("SERVER_PROTOCOL", "HTTP/1.1"));
    envVars.push_back(GenerateEnvVar("SERVER_SOFTWARE", "Webserv"));
    envVars.push_back(GenerateEnvVar("GATEWAY_INTERFACE", "CGI/1.1"));
    envVars.push_back(GenerateEnvVar("REQUEST_METHOD", request->get_method()));
    envVars.push_back(GenerateEnvVar("REQUEST_URI", request->getCgiScriptPath()));
    envVars.push_back(GenerateEnvVar("QUERY_STRING", request->get_query_string()));
    envVars.push_back(GenerateEnvVar("PATH_INFO", request->get_path_info()));
    if (request->get_header().find("Content-Type") != request->get_header().end())
        envVars.push_back(GenerateEnvVar("CONTENT_TYPE", request->get_header()["Content-Type"]));
    if (request->get_header().find("Content-Type") != request->get_header().end())
        envVars.push_back(GenerateEnvVar("CONTENT_LENGTH", request->get_header()["Content-Length"]));
    if (request->get_header().find("Cookie") != request->get_header().end())
        envVars.push_back(GenerateEnvVar("HTTP_COOKIE", request->get_header()["Cookie"]));

    return envVars;
}

std::string Server::genratefilename()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::ostringstream filename;
    filename << "/tmp/" << tv.tv_sec << "-_-" << tv.tv_usec << ".html";
    return filename.str();
}

void Server::executeCGI(int clientfd)
{
    Http_request *request = Requests[clientfd];
    std::string path = request->getCgiScriptPath();
    std::string cgiQuery = request->get_query_string();
    std::string path_info = request->get_path_info();
    std::string filename = genratefilename();
    std::string method = request->get_method();
    std::string CGI_interpreter = request->getCGI_interpreter();
    std::string CGI_extetion = request->getCGI_extetion();
    std::vector<std::string> envVars = GenerateEnvVars(request);
    cgis[clientfd].inputfilename = request->getFN_body_receved();    

    if (method != "GET" && method != "POST")
    {
        request->getResponse()->setStatusCode("405");
        return;
    }

    if (CGI_interpreter.empty())
    {
        request->getResponse()->setStatusCode("404");
        return;
    }

    if (access(CGI_interpreter.c_str(), X_OK) != 0)
    {
        request->getResponse()->setStatusCode("500");
        return;
    }
    int filefd = open(filename.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (filefd < 0)
    {
        request->getResponse()->setStatusCode("500");
        return;
    }
    int filein = open(cgis[clientfd].inputfilename.c_str(), O_RDONLY);
    if (filein < 0)
    {
        filein = 0;
    }
    int pid = fork();
    if (pid < 0)
    {
        request->getResponse()->setStatusCode("500");
        return;
    }
    if (pid == 0)
    {
        dup2(filefd, 2);
        dup2(filefd, 1);
        if (filein != 0)
        {
            dup2(filein, 0);
            close(filein);
        }
        close(filefd);
        std::vector<char *> args;
        std::vector<char *> env;
        args.push_back((char *)(CGI_interpreter.c_str()));
        args.push_back((char *)(path.c_str()));
        args.push_back(NULL);
        for (size_t i = 0; i < envVars.size(); i++)
        {
            env.push_back((char *)(envVars[i].c_str()));
        }
        env.push_back(NULL);
        execve(CGI_interpreter.c_str(), &args[0], &env[0]);
        DEBUG_MODE && std::cerr << "execve failed" << std::endl;
        exit(1);
    }
    else
    {
        if (filein != 0)
            close (filein);
        close(filefd);
        if (unlink(cgis[clientfd].inputfilename.c_str()) < 0)
            DEBUG_MODE && std::cerr <<  "cant unlink infile" << std::endl;
        cgis[clientfd].pid = pid;
        cgis[clientfd].clientfd = clientfd;
        cgis[clientfd].filename = filename;
        cgis[clientfd].start_time = time(NULL);
        cgis[clientfd].status = -1897;
    }
}


void Server::checkCGITimeout()
{
    
    for (std::map<int, s_cgi>::iterator it = cgis.begin(); it != cgis.end(); ++it)
    {
        int child_pid = it->second.pid;
        pid_t result;
        result = waitpid(child_pid, &it->second.status, WNOHANG);
        Requests[it->first]->getResponse()->setIsCgi(true);
        Requests[it->first]->getResponse()->setFullPath(it->second.filename);
        Requests[it->first]->getResponse()->setFileToDelete(it->second.filename);
        if (result == child_pid)
        {
            if (WIFEXITED(it->second.status))
            {
                if (!WEXITSTATUS(it->second.status))
                    DEBUG_MODE && std::cout << "Child process exited normally with status: " << WEXITSTATUS(it->second.status) << std::endl;
                else
                {
                    DEBUG_MODE && std::cout << "Child process exited not normal hh " << WEXITSTATUS(it->second.status) << std::endl;
                    Requests[it->first]->getResponse()->setStatusCode("500");
                }
            }
            else
            {
                DEBUG_MODE && std::cout << "Internal server error" << std::endl;
                if (child_pid != 0 && kill(child_pid, SIGKILL) < 0)
                    DEBUG_MODE && std::cerr << "can not kill cgi process" << std::endl;
                if (waitpid(child_pid, &it->second.status, 0) < 0)
                    DEBUG_MODE && std::cerr << "can not wait for cgi process" << std::endl;
                Requests[it->first]->getResponse()->setStatusCode("500");
            }
            Requests[it->first]->isCGIrunning = false;
            changeEventToWrite(it->first);
            cgis.erase(it->first);
            return;
        }
        else if (result == 0)
        {
            if (time(NULL) - it->second.start_time >= CGI_TIMEOUT)
            {
                DEBUG_MODE && std::cout << "Timeout reached! Killing child process." << std::endl;
                if (child_pid != 0 && kill(child_pid, SIGKILL) < 0)
                    DEBUG_MODE && std::cerr << "can not kill cgi process" << std::endl;
                if (waitpid(child_pid, &it->second.status, 0) < 0)
                    DEBUG_MODE && std::cerr << "can not wait for cgi process" << std::endl;
                Requests[it->first]->getResponse()->setStatusCode("408");
                Requests[it->first]->isCGIrunning = false;
                changeEventToWrite(it->first);
                cgis.erase(it->first);
            }
            return;
        }
        else if (result == -1)
        {
            DEBUG_MODE && std::cerr << "Error occurred while waiting for child process." << std::endl;
            Requests[it->first]->isCGIrunning = false;
            Requests[it->first]->getResponse()->setStatusCode("500");
            if (child_pid != 0 && kill(child_pid, SIGKILL) < 0)
                    DEBUG_MODE && std::cerr << "can not kill cgi process" << std::endl;
             if (child_pid != 0 && waitpid(child_pid, &it->second.status, 0) < 0)
                    DEBUG_MODE && std::cerr << "can not wait for cgi process" << std::endl;
            changeEventToWrite(it->first);
            cgis.erase(it->first);
            return;
        }
    }
    DEBUG_MODE && std::cout << "CGI process is running" << std::endl;
    while(waitpid(-1, NULL, WNOHANG) > 0);
}