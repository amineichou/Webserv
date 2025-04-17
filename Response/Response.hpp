/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:24:09 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 12:05:27 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <cstdlib>
#include <sys/stat.h>

#include "../config/ServerConfig.hpp"

#define BUFFER_SIZE_SEND 16384

#define TYPE_FILE 1
#define TYPE_DIR 2

class Http_request;

class Response
{
private:
  int         clientFd;
  std::string statusCode;
  std::string fullPath;
  std::string requestedPath;
  std::string Header;
  std::string cgiFileOutput;
  std::pair<std::string, std::string> redirection;
  std::map<std::string, std::string> userErrorPages;
  size_t      offset;
  size_t      chunkCount;
  int         resourceType;
  bool        running;
  bool        isChunked;
  bool        isHtmlResponse;
  bool        isUserErrorPage;
  bool        isListDir;
  bool        isCgiOutput;
  std::string fileTodelete;
  /**
  * opens a fileName for reading
  * reads a `BUFFER_SIZE_SEND` and make a ckunk and sends it to clientFd
  */
  void setResponse(void);
  
  /**
  * Handles directory listing
  * and sets body and header of the
  * response
  */
  void handleDirectoryListing(void);
  
  void handleRedirection(void);
  std::map<std::string, std::string> statusCodeStrings;
  std::map<std::string, std::string> extToMime;
  
  std::string getFileExtension(const std::string filePath) const;
  std::string getRouteUrl(const std::string url) const;
  std::string getFileNameUrl(const std::string url) const;

  void setCgiResponse(void);
  
  
  /**
  * returns 1 if it's a file
  * returns 2 if it's a directory
  */
  void setHeader(void);
  
  public:
  Response();
  ~Response();

  /**
   * It sets statusCode to `code`
   * And returns a generated resopnse for spicified `code` error
   * That will set to the Body and calls setHeader
   * This will set runnung to false and send data to clientFd
   */
  void onError(std::string code);

  /**
   * It sets statusCode to 200
   * And returns a generated resopnse for spicified description
   * That will set to the Body and calls setHeader
   * This will set runnung to false and send data to clientFd
   */
  void onSuccess(std::string description);

  bool isRunning(void) const;
  bool isRedirection() const;

  std::string getCgiFileOutput(void) const;
  bool isCgi(void) const;

  std::string getStatusCode(void) const;
  std::string getFullPath(void) const;

  /**
   * Calculate buffer size (chunk) convert it to hex
   * Sends encoding hexa and then
   * Sends buffer to client and then and end of chunk \r\n
   * If send fails it returns -1 else 0
   */
  int sendChunk(char *buffer, size_t bufferSize) const;

  /// METHODS
  void GetMethod(int clientfd);
  void DeleteMethod(int clientfd);
  void PostMethod(int clientfd);

  // setters
  void setStatusCode(std::string newStatusCode);
  void setFullPath(std::string );
  void setisListDir(bool type);
  void setResourceType(int);
  void setRequestedPath(std::string);
  void setRedirection(std::pair<std::string, std::string>);
  void setUserErrorPages(std::map<std::string, std::string>);
  void setClientFd(int cfd);
  void setCgiFileOutput(std::string fname);
  void setIsCgi(bool bcgi);

  void setFileToDelete(std::string file);
  std::string GetfileToDelete(void);
};
#include "../Request/request.hpp"
#endif /*response*/ 
