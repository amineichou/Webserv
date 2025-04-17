/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymomen <ymomen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:18 by ymomen            #+#    #+#             */
/*   Updated: 2025/03/04 23:22:19 by ymomen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Route.hpp"

Route::Route() {}
Route::~Route() {}

// getters
bool Route::getAutoIndex(void) const
{
    return this->autoindex;
}

std::string Route::getIndex(void) const
{
    return this->index;
}

std::string Route::getPath(void) const
{
    return this->path;
}

std::string Route::getUploadDir(void) const
{
    return this->uploadDir;
}

std::vector<std::string> Route::getAcceptedMethods(void) const
{
    return this->acceptedMethods;
}

std::string Route::getRoot(void) const
{
    return this->root;
}
std::pair<std::string, std::string> Route::getRedirection(void) const
{
    return this->redirection;
}

std::vector<std::pair<std::string, std::string> > Route::getCgi(void) const
{
    return this->cgi;
}


// setters
void Route::setAutoIndex(bool x)
{
    this->autoindex = x;
}

void Route::setIndex(std::string x_index)
{
    this->index = x_index;
}

void Route::setPath(std::string x_path)
{
    this->path = x_path;
}

void Route::setUploadDir(std::string x_uploadDir)
{
    this->uploadDir = x_uploadDir;
}

void Route::setAcceptedMethods(std::string x_acceptedMethods)
{
    while (x_acceptedMethods.length() > 0)
    {
        size_t comma = x_acceptedMethods.find(',');
        if (comma != std::string::npos)
        {
            this->acceptedMethods.push_back(x_acceptedMethods.substr(0, comma));
            x_acceptedMethods.erase(0, comma + 1);
        }
        else
        {
            this->acceptedMethods.push_back(x_acceptedMethods);
            break;
        }
    }

}

void Route::setRoot(std::string x_root)
{
    this->root = x_root;
}

Route &Route::operator=(const Route &other) {
    if (this != &other) {
        this->autoindex = other.autoindex;
        this->index = other.index;
        this->path = other.path;
        this->uploadDir = other.uploadDir;
        this->root = other.root;
        this->acceptedMethods = other.acceptedMethods;
        this->redirection = other.redirection;
        this->cgi = other.cgi;
    }
    return *this;
}
Route::Route(const Route &other)
{
    if (this != &other)
        *this = other;
}


int Route::setRedirection(std::string redirectionLine)
{
    size_t comma = redirectionLine.find(',');
    if (comma != std::string::npos)
    {
        std::string code = redirectionLine.substr(0, comma);
        int codeToInt = std::atoi(code.c_str());

        if (codeToInt < 300 || codeToInt > 399)
            return 1;

        std::string path = redirectionLine.substr(comma + 1);
        if (path.length() == 0)
            return 1;
        if (path.find("http://") == std::string::npos && path.find("https://") == std::string::npos)
            path = "http://" + path;
        this->redirection.first = code;;
        this->redirection.second = path;
    }
    else
        return 1;
    return 0;
}

int  Route::setCgi(std::string cgiLine)
{
    size_t comma = cgiLine.find(',');
    if (comma != std::string::npos)
    {
        std::string ext = cgiLine.substr(0, comma);
        std::string path = cgiLine.substr(comma + 1);
        if (ext.length() == 0 || path.length() == 0)
            return 1;
        this->cgi.push_back(std::make_pair(ext, path));
    }
    else
        return 1;
    return 0;
}