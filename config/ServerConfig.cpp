/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:37 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 14:51:40 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ServerConfig.hpp"

// these are default server config
ServerConfig::ServerConfig()
{
    this->limitClientBodySize = 0;
}

ServerConfig::~ServerConfig()
{
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{
    if (this != &other)
    {
        this->Ports = other.Ports;
        this->serverID = other.serverID;
        this->serverNames = other.serverNames;
        this->globalRoot = other.globalRoot;
        this->limitClientBodySize = other.limitClientBodySize;
        this->errPages = other.errPages;
        this->routes = other.routes;
    }
    return *this;
}

int ServerConfig::serversCount = 0;

// setters
void ServerConfig::setServerID(std::string str)
{
    this->serverID = str;
}

int ServerConfig::setPorts(std::string value, size_t lineCount)
{
    std::string usrHost;
    std::string usrPort;
    int accPort;
    size_t twoPoints = value.find(':');
    if (twoPoints != std::string::npos)
    {

        usrHost = value.substr(0, twoPoints);
        for (size_t i = 0; i < usrHost.length(); i++) // check user host for other chars in port
        {
            if (!std::isdigit(usrHost[i]) && usrHost[i] != '.')
            {
                printStyledError("parsing error", "unvalid synthax, host can have only positive numbers", lineCount);
                return 1;
            }
        }
        usrPort = value.substr(twoPoints + 1, value.length() - twoPoints);
        if (isStrNumber(usrPort))
        {
            printStyledError("parsing error", "unvalid synthax, port can have only positive numbers", lineCount);
            return 1;
        }
        accPort = atoi(usrPort.c_str());
    }
    else
    {
        usrHost = "";
        if (isStrNumber(value))
        {
            printStyledError("parsing error", "unvalid synthax, port can have only positive numbers", lineCount);
            return 1;
        }
        accPort = atoi(value.c_str()); // acctual port number from value assuming there's no user host
    }
    if (accPort < 0 || accPort > MAX_PORT)
    {
        printStyledError("parsing error", "port number is out if range", lineCount);
        return 1;
    }
    for (size_t i = 0; i < this->Ports.size(); i++) // check for duplicate port
    {
        if (Ports[i].first == usrHost && Ports[i].second == accPort)
        {
            printStyledError("parsing error", "found a duplicate port", lineCount);
            return 1;
        }
    }
    this->Ports.push_back(std::make_pair(usrHost, accPort));
    return 0;
}

void ServerConfig::setServerNames(std::string srvNames)
{
    this->serverNames = srvNames;
}

int ServerConfig::setLimitClientBodySize(long lcbs, size_t lineCount)
{
    if (lcbs < 0)
    {
        printStyledError("Parsing error", "invalid size or unit, try [ NUMBER (B, KB, MB, GB, TB) ]", lineCount);
        return 1;
    }
    this->limitClientBodySize = lcbs;
    return 0;
}

void ServerConfig::setGlobalRoot(std::string rfolder)
{
    this->globalRoot = rfolder;
}

void ServerConfig::setServersCount(int i)
{
    this->serversCount = i;
}

void ServerConfig::setRoutes(const Route r)
{
    this->routes.push_back(r);
}

int ServerConfig::setErrPages(std::string value, size_t lineCount)
{
    size_t commaPos = value.find(',');
    if (commaPos == std::string::npos)
    {
        printStyledError("parsing error", "expected a  `,` after error page code", lineCount);
        return 1;
    }
    std::string code = value.substr(0, commaPos);
    for (size_t i = 0; i < code.size(); i++)
    {
        if (std::isdigit(code[i]) != 1)
        {
            printStyledError("parsing error", "error code not valid", lineCount);
            return 1;
        }
    }
    if (atoi(code.c_str()) < 100 || atoi(code.c_str()) > 599)
    {
        printStyledError("parsing error", "error code is not valide", lineCount);
        return 1;
    }
    std::string path = value.substr(commaPos + 1, value.length() - commaPos);
    if (path.length() == 0)
    {
        printStyledError("parsing error", "pleae specify error page or `default` for default server page", lineCount);
        return 1;
    }
    this->errPages.insert(std::make_pair(code, path));

    return 0;
}

// getters
std::string ServerConfig::getServerID(void) const
{
    return this->serverID;
}

std::vector<std::pair<std::string, int> > ServerConfig::getPorts(void) const
{
    return this->Ports;
}

std::string ServerConfig::getServerNames(void) const
{
    return this->serverNames;
}

unsigned long ServerConfig::getLimitClientBodySize(void) const
{
    return this->limitClientBodySize;
}

std::string ServerConfig::getGlobalRoot(void) const
{
    return this->globalRoot;
}

size_t ServerConfig::getServersCount(void) const
{
    return this->serversCount;
}

std::vector<Route> ServerConfig::getRoutes(void) const
{
    return this->routes;
}

std::map<std::string, std::string> ServerConfig::getErrPages(void) const
{
    return this->errPages;
}