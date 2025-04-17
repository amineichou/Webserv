/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:24:04 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:49:06 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "./Response.hpp"

Response::Response()
{
    this->statusCode = "000";
    this->running = true;
    this->isChunked = false;
    this->isHtmlResponse = false;
    this->isUserErrorPage = false;

    this->offset = 0;
    this->fullPath = "";
    this->isListDir = 0;
    this->resourceType = 0;
    this->isCgiOutput = false;

    // map to code strings for header respnse
    this->statusCodeStrings["200"] = "OK";
    this->statusCodeStrings["301"] = "Moved Permanently";
    this->statusCodeStrings["400"] = "Bad Request";
    this->statusCodeStrings["403"] = "Forbidden";
    this->statusCodeStrings["404"] = "Not Found";
    this->statusCodeStrings["405"] = "Not Allowed";
    this->statusCodeStrings["408"] = "Timeout";
    this->statusCodeStrings["500"] = "Internal Server Error";

    // map to file extensions vs MIME
    extToMime[""] = "text/html";
    extToMime["html"] = "text/html";
    extToMime["htm"] = "text/html";
    extToMime["css"] = "text/css";
    extToMime["js"] = "application/javascript";
    extToMime["json"] = "application/json";
    extToMime["jpg"] = "image/jpeg";
    extToMime["jpeg"] = "image/jpeg";
    extToMime["png"] = "image/png";
    extToMime["gif"] = "image/gif";
    extToMime["txt"] = "text/plain";
    extToMime["pdf"] = "application/pdf";
    extToMime["xml"] = "application/xml";
    extToMime["zip"] = "application/zip";
    extToMime["tar"] = "application/x-tar";
    extToMime["gz"] = "application/gzip";
    extToMime["mp3"] = "audio/mpeg";
    extToMime["wav"] = "audio/wav";
    extToMime["mp4"] = "video/mp4";
    extToMime["avi"] = "video/x-msvideo";
    extToMime["mov"] = "video/quicktime";
    extToMime["svg"] = "image/svg+xml";
    extToMime["ico"] = "image/x-icon";
    extToMime["bmp"] = "image/bmp";
    extToMime["tiff"] = "image/tiff";
    extToMime["csv"] = "text/csv";

    DEBUG_MODE && std::cout << "RESPONSE CREATED" << std::endl;
}

Response::~Response()
{
    DEBUG_MODE && std::cout << "RESPONSE DESTROYED" << std::endl;
}

void Response::setStatusCode(std::string newStatusCode)
{
    this->statusCode = newStatusCode;
}

void Response::setFullPath(std::string path)
{
    this->fullPath = path;
}
void Response:: setisListDir(bool type)
{
    this->isListDir = type;
}

void Response::setResourceType(int type)
{
    this->resourceType = type;
}

void Response::setRequestedPath(std::string n_path)
{
    this->requestedPath = n_path;
}

void Response::setRedirection(std::pair<std::string, std::string> red)
{
    this->redirection = red;
}

void Response::setUserErrorPages(std::map<std::string, std::string> usrDefinedErrorPages)
{
    this->userErrorPages = usrDefinedErrorPages;
}

void Response::setClientFd(int cfd)
{
    this->clientFd = cfd;
}

void Response::setCgiFileOutput(std::string fname)
{
  this->cgiFileOutput = fname;
}

void Response::setIsCgi(bool bcgi)
{
  this->isCgiOutput = bcgi;
}

void Response::setFileToDelete(std::string file)
{
  this->fileTodelete = file;
}

std::string Response::GetfileToDelete(void)
{
  return this->fileTodelete;
}

std::string Response::getFullPath(void) const
{
    return this->fullPath;
}

std::string Response::getCgiFileOutput(void) const
{
  return this->cgiFileOutput;
}

bool Response::isCgi(void) const
{
  return this->isCgiOutput;
}

std::string Response::getFileExtension(const std::string filePath) const
{
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == filePath.length() - 1) {
        return "";
    }
    std::string extension = filePath.substr(dotPos + 1);
    return extension;
}

std::string Response::getStatusCode(void) const 
{
  return this->statusCode;
}

bool Response::isRedirection() const {
    if (this->redirection.first.empty())
      return false;
    return true;
}

bool Response::isRunning(void) const
{
  return this->running;
}


void Response::onSuccess(std::string description)
{
    this->statusCode = "200";

    std::string htmlResponse  = "<!DOCTYPE html>\n"
                                "<html lang=\"en\">\n"
                                "<head>\n"
                                "    <meta charset=\"UTF-8\">\n"
                                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                                "    <link rel=\"icon\" type=\"image/png\" href=\"/assets/success.png\">\n"
                                "    <title>" + description + "</title>\n"
                                "    <style>\n"
                                "        body {\n"
                                "            font-family: Arial, sans-serif;\n"
                                "            text-align: center;\n"
                                "            background-color: #e6f9e6;\n"
                                "            color: #2d6a2d;\n"
                                "            margin: 0;\n"
                                "            padding: 0;\n"
                                "            display: flex;\n"
                                "            justify-content: center;\n"
                                "            align-items: center;\n"
                                "            height: 100vh;\n"
                                "        }\n"
                                "        .container {\n"
                                "            max-width: 600px;\n"
                                "            padding: 20px;\n"
                                "            border-radius: 10px;\n"
                                "        }\n"
                                "        h1 {\n"
                                "            font-size: 80px;\n"
                                "            margin: 0;\n"
                                "            color: #4CAF50;\n"
                                "        }\n"
                                "        p {\n"
                                "            font-size: 22px;\n"
                                "            margin-top: 10px;\n"
                                "        }\n"
                                "    </style>\n"
                                "</head>\n"
                                "<body>\n"
                                "    <div class=\"container\">\n"
                                "        <img src=\"/assets/success.png\" width=\"50px\" height=\"auto\" />\n"
                                "        <h1>" + this->statusCode + "</h1>\n"
                                "        <p>"+ description + "</p>\n"
                                "    </div>\n"
                                "</body>\n"
                                "</html>";

    this->statusCode = "200";
    this->running = false;
    this->isHtmlResponse = true;
    std::ostringstream encoding;
    encoding << std::hex << htmlResponse.length() << std::dec;
    
    setHeader();
    std::string fullResponse = this->Header 
                    + encoding.str() + "\r\n"
                    + htmlResponse + "\r\n"
                    + "0\r\n\r\n";

    send(clientFd, fullResponse.c_str(), fullResponse.length(), 0);
}

void Response::onError(std::string code)
{
    if ((this->userErrorPages.find(code) != this->userErrorPages.end()) && !this->isUserErrorPage)
    {
        this->statusCode = code;
        this->isUserErrorPage = true; // bro preventing us from an infinite recurcive looop 😂
        std::ifstream userErrorFile(this->userErrorPages[code].c_str());
        if (!userErrorFile.is_open())
        {
            onError("404");
            return ;
        }
        this->fullPath = this->userErrorPages[code];
        DEBUG_MODE && std::cout << "serving this error page>" << this->fullPath << std::endl;
        setResponse();
        return;
    }

    std::string htmlResponse =  "<!DOCTYPE html>\n"
                                "<html lang=\"en\">\n"
                                 "<head>\n"
                                 "    <meta charset=\"UTF-8\">\n"
                                 "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                                 "    <link rel=\"icon\" type=\"image/png\" href=\"/assets/failed.png\">\n"
                                 "    <title>" + code + " " + statusCodeStrings[code] + "</title>\n"
                                 "    <style>\n"
                                 "        body {\n"
                                 "            font-family: Arial, sans-serif;\n"
                                 "            text-align: center;\n"
                                 "            background-color: #f8f8f8;\n"
                                 "            color: #333;\n"
                                 "            margin: 0;\n"
                                 "            padding: 0;\n"
                                 "            display: flex;\n"
                                 "            justify-content: center;\n"
                                 "            align-items: center;\n"
                                 "            height: 100vh;\n"
                                 "        }\n"
                                 "        .container {\n"
                                 "            max-width: 600px;\n"
                                 "        }\n"
                                 "        h1 {\n"
                                 "            font-size: 100px;\n"
                                 "            margin: 0;\n"
                                 "            color: #ff6b6b;\n"
                                 "        }\n"
                                 "        p {\n"
                                 "            font-size: 20px;\n"
                                 "        }\n"
                                 "    </style>\n"
                                 "</head>\n"
                                 "<body>\n"
                                 "    <div class=\"container\">\n"
                                 "        <h1>" + code + "</h1>\n"
                                 "        <p>"+ statusCodeStrings[code] + "</p>\n"
                                 "    </div>\n"
                                 "</body>\n"
                                 "</html>";

    this->statusCode = code;
    this->running = false;
    this->isHtmlResponse = true;
    this->isUserErrorPage = false;
    std::ostringstream encoding;
    encoding << std::hex << htmlResponse.length() << std::dec;
    
    setHeader();
    std::string fullResponse = this->Header 
                    + encoding.str() + "\r\n"
                    + htmlResponse + "\r\n"
                    + "0\r\n\r\n";
    send(clientFd, fullResponse.c_str(), fullResponse.length(), 0);

}

void Response::setHeader(void)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusCodeStrings[statusCode] << "\r\n";
    if (this->isHtmlResponse)
        responseStream << "Content-Type: " << extToMime["html"] << "\r\n";
    else
        responseStream << "Content-Type: " << extToMime[getFileExtension(fullPath)] << "\r\n";
    responseStream << "Transfer-Encoding: chunked\r\n";
    responseStream << "Connection: Close\r\n";
    responseStream << "Accept-Ranges: none\r\n";
    responseStream << "\r\n";

    this->Header = responseStream.str();
}

int Response::sendChunk(char *buffer, size_t bufferSize) const
{
    std::ostringstream encoding;
    encoding << std::hex << bufferSize << std::dec << "\r\n";

    if (send(clientFd, encoding.str().c_str(), encoding.str().length(), 0) < 0)
        return -1;
    if (send(clientFd, buffer, bufferSize, 0) < 0)
        return -1;
    if (send(clientFd, "\r\n", 2, 0) < 0)
        return -1;
    return 1;
}

void Response::setCgiResponse(void)
{
    std::ifstream file(fullPath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << fullPath << std::endl;
        onError("404");
        return;
    }

    std::ostringstream header;
    std::string fileContent;
    std::string line;
    bool headersParsed = false;

    header << "HTTP/1.1 200 OK\r\n";

    while (std::getline(file, line))
    {
        if (line.empty()) {
            headersParsed = true;
            break;
        }
        header << line << "\r\n";
    }

    while (std::getline(file, line))
    {
        fileContent += line;
    }
    
    file.close();

    if (!headersParsed) {
        header.str("");
    }

    header << "Content-type: text/html\r\n";
    header << "Content-length: " << fileContent.size() << "\r\n";
    header << "\r\n";

    if (this->statusCode == "000")
        statusCode = "200";

    std::string response = header.str() + fileContent;

    if (send(clientFd, response.c_str(), response.size(), 0) < 0) {
        this->running = false;
        return;
    }

    this->running = false;
}


void Response::setResponse(void)
{
    if (this->isCgi() && (this->statusCode == "000" || this->statusCode == "200"))
    {
        setCgiResponse();
        return;
    }

    std::ifstream file(fullPath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << fullPath << std::endl;
        onError(this->statusCode == "000" ? "404" : this->statusCode);
        return;
    }

    char buffer[BUFFER_SIZE_SEND];

    if (this->offset != 0)
        file.seekg(this->offset);

    file.read(buffer, BUFFER_SIZE_SEND);
    std::streamsize readBytes = file.gcount();
    
    if (this->statusCode == "000")
        statusCode = "200";

    if (readBytes < 0) {
        onError("500");
        return;
    }

    this->offset += readBytes;

    if (this->isChunked)
    {
        if(sendChunk(buffer, readBytes) < 0)
            this->running = false;
    }
    else {
        this->isChunked = true;
        setHeader();
        if (send(clientFd, this->Header.c_str(), this->Header.length(), 0) < 0 || sendChunk(buffer, readBytes) < 0)
            this->running = false;
    }

    if (file.eof())
    {
        this->running = false;
        send(clientFd, "0\r\n\r\n", 5, 0);
    }

    file.close();
}
