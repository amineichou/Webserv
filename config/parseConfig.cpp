/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:07 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 14:51:31 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

void printStyledError(const std::string &errorType, const std::string &discription, int lineNumber)
{
    const char *RED = "\033[31m";
    const char *BOLD = "\033[1m";
    const char *RESET = "\033[0m";

    std::cerr << RED << (lineNumber ? "line " : "") << lineNumber << ": " << RESET
              << RED << BOLD << errorType << ": " << RESET
              << discription << std::endl;
}

bool isStrNumber(const std::string &str)
{
    if (str.empty())
        return 1;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
            return 1;
    }
    return 0;
}

/*
    remove all spaces/tabs from a string
*/
static void removeSpaces(std::string &old)
{
    std::string res;

    for (size_t i = 0; i < old.size(); ++i)
    {
        if (old[i] != ' ' && old[i] != '\t' && old[i] != '\n')
        {
            res += old[i];
        }
    }
    old = res;
}

/*
    remove comment form a string
    a comment starts with a #
    ends with new line
*/
static void removeComments(std::string &line)
{
    size_t where = line.find("#");
    if (where != std::string::npos)
    {
        if (where == 0)
            line = "";
        else
            line = line.substr(0, where);
    }
}

/*
    this function checks File extension
    and return true if it matches otherwise false
*/
static bool checkFileExtension(const std::string &fileName, std::string extension)
{
    if (fileName.length() >= extension.length())
    {
        return fileName.substr(fileName.length() - extension.length()) == extension;
    }
    return false;
}

/*
    -> returns 0 on success
    -> returns 1 on failure
*/
static int parseField(std::string line, const size_t lineNumber, std::string &value)
{
    removeSpaces(line);
    size_t equalPos = line.find('=');
    if (line[line.length() - 1] != ';')
    {
        printStyledError("Parsing error", "missing a ;", lineNumber);
        return 1;
    }
    if (equalPos == std::string::npos)
    {
        printStyledError("Parsing error", "missing a =", lineNumber);
        return 1;
    }
    value = line.substr(equalPos + 1, line.length() - equalPos - 2); // Extract value without `;`
    if (value == "")
    {
        printStyledError("parsing error", "value can't be left empty", lineNumber);
        return 1;
    }
    return 0;
}

/*
    by giving a string example (500MB)
    convert it to bytes and return a
    number
*/
static long int convertToBytes(const std::string &sizeStr)
{
    long long KB = 1024;
    long long MB = KB * 1024;
    long long GB = MB * 1024;
    long long TB = GB * 1024;

    // Extract the numeric part and unit part
    size_t i = 0;
    while (i < sizeStr.length() && (std::isdigit(sizeStr[i]) || sizeStr[i] == '.'))
        ++i;

    if (i == 0)
        return -1;

    std::string numberPart = sizeStr.substr(0, i);
    std::string unitPart = sizeStr.substr(i);

    double number = atof(numberPart.c_str());

    long long bytes = 0;
    if (unitPart == "KB")
        bytes = number * KB;
    else if (unitPart == "MB")
        bytes = number * MB;
    else if (unitPart == "GB")
        bytes = number * GB;
    else if (unitPart == "TB")
        bytes = number * TB;
    else if (unitPart == "B")
        bytes = number;
    else
        return -1;

    return bytes;
}

/*
    not only parsing params
    but fill out singleServer too
    -> returns 0 on success
    -> returns 1 on failure
    -> returns 2 on routes (if found)
*/
static int parseFillParams(size_t &lineCount, ServerConfig &singleServer, std::string &param)
{
    std::string value;
    if (param.find("port") != std::string::npos)
    {
        if (parseField(param, lineCount, value))
            return 1;
        return singleServer.setPorts(value, lineCount);
    }
    else if (param.find("servernames") != std::string::npos)
    {
        if (parseField(param, lineCount, value))
            return 1;

        singleServer.setServerNames(value);
    }
    else if (param.find("bodysizelimit") != std::string::npos)
    {
        if (parseField(param, lineCount, value))
            return 1;
        return singleServer.setLimitClientBodySize(convertToBytes(value), lineCount);
    }
    else if (param.find("globalroot") != std::string::npos)
    {
        if (parseField(param, lineCount, value))
            return 1;
        singleServer.setGlobalRoot(value);
    }
    else if (param.find("errorpage") != std::string::npos)
    {
        if (parseField(param, lineCount, value))
            return 1;
        return singleServer.setErrPages(value, lineCount);
    }
    else if (param.find("route") != std::string::npos)
        return 2;
    else if (!param.empty() && param.find(']') == std::string::npos)
    {
        printStyledError("parssing error", "unexpected synthax", lineCount);
        return 1;
    }
    return 0;
}

int checkConfigValues(const std::vector<ServerConfig> &Servers)
{
    for (size_t i = 0;  i < Servers.size(); i++)
    {
        if (Servers[i].getGlobalRoot().empty())
        {
            printStyledError("parsing error", "globalroute can't be unset", i);
            return -1;
        }
        else if (Servers[i].getLimitClientBodySize() == 0)
            printStyledError("warning", "bodysizelimit will be set 0", i);
        if (Servers[i].getServerNames().empty())
        {
            printStyledError("parsing error", "missing servernames", i);
            return -1;
        }
        else if (Servers[i].getPorts().size() == 0)
        {
            printStyledError("parsing error", "no port to listen", i);
            return -1;
        }
        else if (Servers[i].getRoutes().size() == 0)
             printStyledError("warning", "no route was declared", i);
    }
    return 0;
}

int parseConfig(std::string fileName, std::vector<ServerConfig> &Servers)
{
    std::string line;
    std::ifstream configFile(fileName.c_str());

    if (!checkFileExtension(fileName, ".conf"))
    {
        printStyledError("configuration error", "no input files", 0);
        return 1;
    }

    if (!configFile.is_open())
    {
        printStyledError("access errors", "Could not open file" + fileName, 0);
        return 1;
    }

    size_t serversFound = 0;
    size_t lineCounter = 1;
    size_t where;
    bool serverEndReached = false;

    while (std::getline(configFile, line))
    {
        removeComments(line);
        removeSpaces(line);
        if ((where = line.find('[')) != std::string::npos)
        {
            if (where == 0)
            {
                printStyledError("Parsing error", "no server name found", lineCounter);
                return 1;
            }
            ServerConfig singleServer;
            if (line.substr(0, where) != "server")
            {
                printStyledError("parsing error", "server decleration has to start with `server`[... ", lineCounter);
                return 1;
            }
            singleServer.setServerID(line.substr(0, where));
            while (std::getline(configFile, line))
            {
                lineCounter++;
                removeComments(line);
                removeSpaces(line);
                if (parseFillParams(lineCounter, singleServer, line) == 1)
                    return 1; 
                size_t routeKeyPos;
                if ((routeKeyPos = line.find("route")) != std::string::npos)
                {
                    Route somewhere;
                    removeSpaces(line);
                    routeKeyPos += 5; // 5 is "route" length
                    size_t bracketPos = line.find('{');
                    bool RouteEndReached = false;
                    if (bracketPos == std::string::npos)
                    {
                        printStyledError("parsing error", "expected a `{`", lineCounter);
                        return 1;
                    }
                    somewhere.setPath(line.substr(routeKeyPos, line.length() - routeKeyPos - 1));
                    std::string value;
                    while (std::getline(configFile, line))
                    {
                        removeComments(line);
                        removeSpaces(line);
                        if (line.find("autoindex") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            if (value == "on")
                                somewhere.setAutoIndex(true);
                            else if (value == "off")
                                somewhere.setAutoIndex(false);
                            else
                            {
                                printStyledError("parsing error", "unknown value for autoindex", lineCounter);
                                return 1;
                            }
                        }
                        else if (line.find("uploaddir") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            somewhere.setUploadDir(value);
                        }
                        else if (line.find("acceptedmethods") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            somewhere.setAcceptedMethods(value);
                        }
                        else if (line.find("index") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            somewhere.setIndex(value);
                        }
                        else if (line.find("root") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            somewhere.setRoot(value);
                        }
                        else if (line.find("return") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            if (somewhere.setRedirection(value))
                            {
                                printStyledError("parsing erorr", "redirection `return` is malformatted. \ntry `return = code, path;`\n300 <= code <= 399", lineCounter);
                                return 1;
                            }
                        }
                        else if (line.find("cgiextension") != std::string::npos)
                        {
                            if (parseField(line, lineCounter, value))
                                return 1;
                            if (somewhere.setCgi(value))
                            {
                                printStyledError("parsing erorr", "cgiextension is malformatted.", lineCounter);
                                return 1;
                            }
                        }
                        else if (line.find('}') != std::string::npos)
                        {
                            lineCounter++;
                            RouteEndReached = true;
                            break;
                        }
                        else if (!line.empty())
                        {
                            lineCounter++;
                            printStyledError("parsing error", "unexpected synthax", lineCounter);
                            return 1;
                        }
                        lineCounter++;
                    }
                    if (!RouteEndReached)
                    {
                        printStyledError("parsing error", "expected a `}`", lineCounter);
                        return 1;
                    }
                    singleServer.setRoutes(somewhere);
                }
                if (line == "]")
                {
                    Servers.push_back(singleServer);
                    serversFound++;
                    serverEndReached = true;
                    break;
                }
            }
            if (!serverEndReached)
            {
                printStyledError("parssing error", "expected only a `]` at the end", lineCounter);
                return 1;
            }
            serverEndReached = false;
            singleServer.setServersCount(serversFound); 
        }
        else if (line.length())
        {
            printStyledError("parssing error", "unexpected synthax", lineCounter);
            return 1;
        }
        lineCounter++;
    }

    // check for empty config
    if (!serversFound)
    {
        printStyledError("error", "no server configuration found.", 0);
        return 1;
    }

    // check errors regarding values set in server
    if (checkConfigValues(Servers) == -1)
        return 1;

    configFile.close();
    return 0;
}