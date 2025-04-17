/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymomen <ymomen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:47 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/15 18:59:36 by ymomen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Response.hpp"

std::string Response::getRouteUrl(const std::string url) const
{
    size_t firstSlash = url.find('/');
    if (firstSlash == std::string::npos)
        return url;

    size_t secondSlash = url.find('/', firstSlash + 1);
    if (secondSlash == std::string::npos)
        return url;

    return url.substr(0, secondSlash);
}

std::string Response::getFileNameUrl(const std::string url) const
{
    size_t lastSlash = url.find_last_of('/');
    if (lastSlash == std::string::npos)
        return url;
    std::string res = url.substr(lastSlash + 1);
    return res;
}

void Response::handleDirectoryListing(void)
{
    DIR *thisDir;
    struct dirent *dp;
    if ((thisDir = opendir(this->fullPath.c_str())) != NULL)
    {
        std::ostringstream htmlResponse;
        htmlResponse << "<!DOCTYPE html>\n"
                        "<html lang=\"en\">\n"
                        "  <head>\n"
                        "    <meta charset=\"UTF-8\" />\n"
                        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
                        "    <link rel=\"icon\" type=\"image/x-icon\" href=\"/assets/success.png\">\n"
                        "    <title>Directory Listing</title>\n"
                        "    <link href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css\" rel=\"stylesheet\">\n"
                        "    <style>\n"
                        "      body {\n"
                        "        font-family: Arial, sans-serif;\n"
                        "        color: #333;\n"
                        "        margin: 0;\n"
                        "        padding: 0;\n"
                        "      }\n"
                        "\n"
                        "      .directory-list {\n"
                        "        max-width: 600px;\n"
                        "        margin: 50px auto;\n"
                        "        padding: 20px;\n"
                        "      }\n"
                        "\n"
                        "      h1 {\n"
                        "        text-align: center;\n"
                        "        font-size: 1.8em;\n"
                        "        margin-bottom: 20px;\n"
                        "        color: #ff6b6b;\n"
                        "      }\n"
                        "\n"
                        "      ul {\n"
                        "        list-style-type: none;\n"
                        "        padding: 10px;\n"
                        "      }\n"
                        "\n"
                        "      li {\n"
                        "        margin: 15px 0;\n"
                        "        font-size: 1.1em;\n"
                        "        display: flex;\n"
                        "        align-items: center;\n"
                        "        padding: 10px 0;\n"
                        "      }\n"
                        "\n"
                        "      a {\n"
                        "        text-decoration: none;\n"
                        "        color: #007bff;\n"
                        "        transition: color 0.2s;\n"
                        "        margin-left: 10px;\n"
                        "      }\n"
                        "\n"
                        "      a:hover {\n"
                        "        color: #0056b3;\n"
                        "      }\n"
                        "\n"
                        "      .icon {\n"
                        "        color: #333;\n"
                        "        font-size: 1.2em;\n"
                        "      }\n"
                        "\n"
                        "    </style>\n"
                        "  </head>\n"
                        "  <body>\n"
                        "    <div class=\"directory-list\">\n"
                        "      <h1><i class=\"fas fa-folder-open\"></i> Directory Listing For " + this->requestedPath + "</h1>\n"
                        "      <ul>\n";

       if (!this->requestedPath.empty() && this->requestedPath[this->requestedPath.length() - 1] != '/')
            this->requestedPath += "/";

        while ((dp = readdir(thisDir)) != NULL)
        {
            std::string elementName = dp->d_name;

            if (elementName == "." || elementName == "..")
                continue;

            if ((int)dp->d_type == DT_DIR)
                htmlResponse << "<li><i class=\"fas fa-folder icon\"></i> <a href=\"" << this->requestedPath << dp->d_name << "\">" << dp->d_name <<  "</a></li>\n";
            else
                htmlResponse << "<li><i class=\"fas fa-file icon\"></i> <a href=\"" << this->requestedPath << dp->d_name << "\">" << dp->d_name << "</a></li>\n";
        }
        htmlResponse << "      </ul>\n"
                        "    </div>\n"
                        "  </body>\n"
                        "</html>\n";

        this->statusCode = "200";
        this->running = false;
        this->isHtmlResponse = true;
        std::ostringstream encoding;
        encoding << std::hex << htmlResponse.str().length() << std::dec;
    
        setHeader();
        std::string fullResponse = this->Header 
                                + encoding.str() + "\r\n"
                                + htmlResponse.str() + "\r\n"
                                + "0\r\n\r\n";

        send(clientFd, fullResponse.c_str(), fullResponse.length(), 0);
        closedir(thisDir);
    }
    else
        onError("404");
}

void Response::handleRedirection(void)
{
    std::ostringstream response;
    response << "HTTP/1.1 " << redirection.first << " Moved Permanently\r\n"
             << "content-Length: 0\r\n"
             << "location: " << redirection.second << "\r\n"
             << "cache-Control: no-store, no-cache\r\n"
             << "connection: close\r\n";

    this->running = false;
    send(clientFd, response.str().c_str(), response.str().length(), 0);
}

void Response::GetMethod(int clientfd)
{
    if (this->isChunked)
    {
        setResponse();
        return;
    }

    this->clientFd = clientfd;
    
    if (this->isRedirection())
    {
        handleRedirection();
        return ;
    }

    if (this->statusCode != "000")
        onError(this->statusCode);
    else if (this->isListDir == true)
        handleDirectoryListing();
    else
        setResponse();
}
