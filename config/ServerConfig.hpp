/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:43 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:03:38 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "Route.hpp"
 
#define MAX_PORT 65535

class ServerConfig
{
private:
    std::vector<std::pair<std::string, int> > Ports;
    std::string serverID;
    std::string serverNames;
    std::string globalRoot;
    unsigned long limitClientBodySize;
    std::map<std::string, std::string> errPages;
    std::vector<Route> routes;

public:
    static int serversCount;
    ServerConfig();
    ~ServerConfig();
    // setters
    int setPorts(std::string value, size_t lineCount);
    int setLimitClientBodySize(long lcbs, size_t lineCount);
    int setErrPages(std::string value, size_t lineCount);
    void setServerID(std::string str);
    void setServerNames(std::string srvNames);
    void setGlobalRoot(std::string rfolder);
    void setServersCount(int i);
    void setRoutes(const Route r);

    // getters
    std::string getServerID(void) const;
    std::vector<std::pair<std::string, int> > getPorts(void) const;
    std::string getServerNames(void) const;
    unsigned long getLimitClientBodySize(void) const;
    std::string getGlobalRoot(void) const;
    size_t getServersCount(void) const;
    std::vector<Route> getRoutes(void) const;
    std::map<std::string, std::string> getErrPages(void) const;

    ServerConfig &operator=(const ServerConfig &other);
};

/**
 * Prints a styled error message with type, description, line number, and content.
 *
 * Usage:
 * printStyledError("Syntax Error", "Unexpected token", 42, "return x;");
 */
void printStyledError(const std::string &errorType, const std::string &discription, int lineNumber);

/*
    ### Config File Parser
    opens fileName for reading, it parses every server config
    and fill out each of the vector Servers classess
    * returns 1 on failure
    * returns 0 on success
*/
int parseConfig(std::string fileName, std::vector<ServerConfig> &Servers);

/**
 * Check if a string has only digits
 * returns
 * 1 if empty or has a non digit
 * 0 if only digits
 */
bool isStrNumber(const std::string &str);