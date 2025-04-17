/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valideRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymomen <ymomen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:29 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:50:07 by ymomen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "request.hpp"

void Http_request::chooseRightconfig(std::vector<ServerConfig *>configs)
{
    if (configs.size() == 1 || this->header.find("Host") == this->header.end())
    {
        this->server_config = configs[0];
        return;
    }

    for (size_t i = 0; i < configs.size(); i++)
    {
        std::string host = this->header["Host"];
        std::string host_config ;
         std::stringstream s(configs[i]->getServerNames());

        host.erase(std::remove_if(host.begin(), host.end(), iswspace), host.end());
        while(std::getline(s, host_config, ' '))
        {
            if (host_config == host)
            {
                this->server_config = configs[i];
                return;
            }
        }

    }
    this->server_config = configs[0];
    return;
}


static std::string normalizePath(const std::string& path) {
    std::string result;
    bool lastWasSlash = false;

    for (size_t i = 0; i < path.length(); i++) {
        if (path[i] == '/') {
            if (!lastWasSlash) result += '/';
            lastWasSlash = true;
        } else {
            result += path[i];
            lastWasSlash = false;
        }
    }

    return result;
}

Route Http_request::GetMatchedRoute()
{
    size_t best_match_index = -1;
    size_t best_match_length = 0;
    const std::vector<Route> &routes = this->server_config->getRoutes(); // Get reference to avoid copying
    path = normalizePath(path);
    for (size_t i = 0; i < routes.size(); i++)
    {
        const std::string &route_path = routes[i].getPath();

        // Check if route_path is a prefix of request path
        if (path.find(route_path) == 0)
        {

            if (route_path != "/" && (path.size() != route_path.size() && path[route_path.size()] != '/'))
                continue;
            // Update best match if it's more specific (longer match)
            if (route_path.length() > best_match_length)
            {
                best_match_index = i;
                best_match_length = route_path.length();
            }
        }
    }

    if (best_match_index != static_cast<size_t>(-1)) // Ensure we found a match
    {
        return routes[best_match_index]; // Return a copy of the Route
    }
    throw "404";
}

bool Http_request::isMethodAllowed(const std::vector<std::string> &AcceptedMtd)
{
        for(size_t i = 0; i < AcceptedMtd.size(); i++)
        {
            if (AcceptedMtd[i] == this->get_method())
                return (true);
        }
        return false;
}

static void  ValidatUploadDir(const std::string &UploadDir)
{
    if (UploadDir.empty())
        throw "400";
    struct stat dir;
    if (stat(UploadDir.c_str(), &dir) != 0)
        throw "400"; //dir not found
    if (!(dir.st_mode & S_IFDIR))
        throw "400"; //not a dir
    if (access(UploadDir.c_str(), W_OK) != 0)
        throw "403"; // no write permission
}

static void ValidatContentLenght(Http_request *rqst, unsigned long limit_body)
{
    if (rqst->get_header().find("Content-Length") == rqst->get_header().end())
        throw "415";
    std::stringstream s(rqst->get_header()["Content-Length"]);
    size_t contnetLength;
    s >> contnetLength;
    if (contnetLength > limit_body)
        throw "413";
}
/*
@fileexists: this function return wheater the path is afile or dir
   *path: path to check
   Return: 1 if file 2 if dir 0 not found
*/
static int fileexists(const std::string &path)
{
    struct stat info;
    int ret = 0;

    if(stat(path.c_str(), &info) == 0)
    {
        ret = 1;
        if (S_ISDIR(info.st_mode))
        {
            ret = 2;
        }
    }
    return ret;
}

static void  ValidatIndex(const Route &matchedRoute, std::string &path, Http_request &request, std::string root)
{
        std::string index = matchedRoute.getIndex();
        std::string rootpath = root + path;
        int is_dirfile = fileexists(rootpath);
     if (is_dirfile > 0)
     {
        request.getResponse()->setResourceType(1);
        if (is_dirfile == 2)
        {
            request.getResponse()->setResourceType(2);
            std::string indexpath = rootpath + index;
            if (rootpath[rootpath.size() - 1] != '/')
                indexpath = rootpath + "/" + index;
            if (!index.empty() && access(indexpath.c_str(), F_OK) != 0)
            {
                throw "404"; // index file not found
            }

            if (!indexpath.empty() && fileexists(indexpath) == 1 && access(indexpath.c_str(), R_OK) == 0)// serve indix file
            {
                request.getResponse()->setFullPath(indexpath);
                path = path + "/" + index; 
                DEBUG_MODE && std::cout <<"Serving indix file :"<<indexpath<<std::endl;
                return;
            }
            if (matchedRoute.getAutoIndex())
            {
                if (access(rootpath.c_str(), F_OK) == 0 && access(rootpath.c_str(), R_OK) != 0)
                    throw "403"; // no read permission
                request.getResponse()->setFullPath(rootpath);
                request.getResponse()->setisListDir(true);
                DEBUG_MODE && std::cout << "listing dircroty in path :" << rootpath <<std::endl;
                return;
            }
            throw "403"; // Autoindex off & no index file → "403" Forbidden
        }
        else
        {
            if (access(rootpath.c_str(), F_OK) == 0 && access(rootpath.c_str(), R_OK) != 0)
                throw "403"; // no read permission
            request.getResponse()->setFullPath(rootpath);
            DEBUG_MODE && std::cout << "Serve file : "<<rootpath<<std::endl;
            return ;
        }
    }
    throw "404"; // Path does not exist → "404" Not Found
}

static void updatepath(std::string &path, const std::string &route)
{
    if (path.find(route) == 0)
        path = path.substr(route.size());
    if (path[0] != '/')
        path = "/" + path;
}

bool Http_request :: isCGIRequest(std::string root)
{
    CGI_extetion = "";
    CGI_interpreter = "";
    if (matchedroute.getCgi().size() < 1)
        return (false);
    std::vector< std::pair < std::string,std::string> > cgiExtation =  matchedroute.getCgi();
    size_t dotpos = path.find('.');
    if (dotpos == std::string::npos || dotpos + 1 == std::string::npos)
        return (false);
    dotpos++;
    std::string extention;
    size_t nextslash = path.find('/', dotpos);
    if (nextslash != std::string::npos)
    {
        extention = path.substr(dotpos, nextslash - dotpos);
        path_info = path.substr(nextslash + 1);
    }
    else
    {
        extention = path.substr(dotpos);
        path_info = "";
    }
    if (extention != "sh" && extention != "py" && extention != "php")
        return false;
    for (size_t i = 0; i < cgiExtation.size(); i++)
    {
        if (extention == cgiExtation[i].first)
        {
            if (!path_info.empty())
                path = path.substr(0, nextslash);
            CGI_extetion = extention;
            CGI_interpreter = cgiExtation[i].second; 
            cgiScriptPath = root + path;
            return true;
        }
    }
    return false;
}
void Http_request::validatingData(std::vector<ServerConfig *>configs)
{

    chooseRightconfig(configs);
    this->response->setUserErrorPages(this->server_config->getErrPages());
    matchedroute = GetMatchedRoute();
    this->getResponse()->setRedirection(matchedroute.getRedirection());
    if (!isMethodAllowed(matchedroute.getAcceptedMethods()))
        throw "405";
    std::string root = matchedroute.getRoot();
    if (root.empty()) {
        root = this->server_config->getGlobalRoot();
    }
    this->response->setRequestedPath(path);
    updatepath(path, matchedroute.getPath());
    this->is_CGI = isCGIRequest(root);
    if (get_method() == "GET" || get_method() == "DELETE")
    {
        this->end_of_read = 1;
        ValidatIndex(matchedroute, path, *this, root);
        if (!this->is_CGI)
            this->is_CGI = isCGIRequest(root);
    }
    if (get_method() == "POST")
    {
        ValidatContentLenght(this,server_config->getLimitClientBodySize());
        if (get_header().find("Transfer-Encoding") != get_header().end() && !get_header()["Transfer-Encoding"].empty())
        {
            std::stringstream s(get_header()["Transfer-Encoding"]);
            if (s.str() != " chunked\r")
                throw "501";
        }
        if (!is_CGI)
        {
            ValidatUploadDir(root + "/" + matchedroute.getUploadDir());
            this->uploadDir = root + "/" + matchedroute.getUploadDir() + "/";
            DEBUG_MODE && std::cout <<"uploaddir : "<< uploadDir<<std::endl;
        }
        else 
        {
            this->path = root + path;
        }
    }
}
