/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHelpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:24 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:42:15 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "request.hpp"

std::string strtrim(std::string str, char c)
{
    size_t pos1;
    size_t pos2;

    pos1 = str.find(c);
    str = str.substr(pos1 + 1);
    pos2 = str.find(c);
    str  = str.substr(0, pos2);
    return (str);
}

std::vector<std::string> split(std::string str, char c)
{
    std::vector<std::string> v;
    std::string              str2;
    size_t                   pos;
    int                      i;
    size_t                      count;

    pos     = str.find(c);
    i       = 0;
    count   = 0;
    while (pos != std::string::npos)
    {
        count++;
        str2 = str.substr(i, pos);
        str = str.substr(pos + 1, str.length());
        if (str2.empty())
            break ;
        v.push_back(str2);
        pos = str.find(c);
    }
    if (!str.empty() && count)
            v.push_back(str);

    return (v);
}
std::string get_file_name(std::vector<std::string> v)
{
    size_t i = -1;
    size_t pos = 0;
    std::string file_name;
    while (++i < v.size())
    {
        pos = v[i].find("filename=");
        if (pos != std::string::npos)
            {
                file_name = v[i].substr(pos + 9);
                file_name = strtrim (file_name, '\"');
                DEBUG_MODE && std::cout << file_name   << "\n" << "len :" << file_name.length() << "\n";
                break ;
            }
    }
    return (file_name);
}

std::string generateRandomfileName(size_t length) {

    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::string shuffledCharacters = characters;

   
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Shuffle the characters
    std::random_shuffle(shuffledCharacters.begin(), shuffledCharacters.end());

    // Select the first 'length' characters from the shuffled set
    return shuffledCharacters.substr(0, length);
}