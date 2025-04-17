/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:30 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:03:27 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

class Route
{
private:
    bool autoindex;
    std::string index;
    std::string path;
    std::string uploadDir;
    std::string root;
    std::vector<std::string> acceptedMethods;
    std::pair<std::string, std::string> redirection;
    std::vector<std::pair<std::string, std::string> > cgi;

public:
    Route();
    Route &operator=(const Route &other);
    Route(const Route &other);
    ~Route();
// getters
    bool getAutoIndex(void) const;
    std::string getIndex(void) const;
    std::string getPath(void) const;
    std::string getUploadDir(void) const;
    std::vector<std::string> getAcceptedMethods(void) const;
    std::string getRoot(void) const;
    std::pair<std::string, std::string> getRedirection(void) const;
    std::vector<std::pair<std::string, std::string> > getCgi(void) const;

// setters
    void setAutoIndex(bool x);
    void setIndex(std::string x_index);
    void setPath(std::string x_path);
    void setUploadDir(std::string x_uploadDir);
    void setAcceptedMethods(std::string x_acceptedMethods);
    void setRoot(std::string x_root);
    int  setRedirection(std::string redirectionLine);
    int  setCgi(std::string);
};