/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedMethodes.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:00 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 14:33:21 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

void Http_request::set_ChunkedBoundryKey(void)
{
    std::string boundary;

    if (!this->header["Content-Type"].empty())
    {
        boundary = this->header["Content-Type"];
        size_t pos = boundary.find("boundary=");
        boundary = boundary.substr(pos + 9, boundary.length());
        this->boundryKey = "--" + boundary + "\n";
        boundary = boundary.substr(0, boundary.length() - 1);
        this->endBoundryKey = "\r\n--" + boundary + "--\r\n";
    }
    else
        return;
}
size_t Http_request::set_crlf_ChunkedCounter(std::string body)
{
    size_t count = 0;
    size_t pos1 = 0;

    size_t pos = body.find("\r\n");
    pos1 = body.find(this->boundryKey);
    if (pos != std::string::npos) //&&this->crlf_counter == 0
        count++;
    while (pos != std::string::npos && count < 4) //&& this->crlf_counter == 0
    {
        count++;
        pos = body.find("\r\n", pos + 2);
    }
    if (pos1 != std::string::npos && pos != std::string::npos && count == 4 && pos1 < pos)
        this->crlf_counter = count;
    else
        this->crlf_counter = 0;
    return pos;
}

/*IN BOUDARY IT'S NECESSARY TO HAVE BOTH C0NTENT-DISPOSITION (file-name included) AND CONTENT-TYPE */

void Http_request::check_boundry_header(size_t pos)
{
    std::string request_line;
    std::string line;
    int i = 0;

    if (pos == std::string::npos || pos == 0)
    {
        this->error = BAD_REQUEST_400;
        throw "400";
    }

    std::vector<std::string> content_disposition;

    request_line = this->body;

    size_t first_pos = request_line.find(this->boundryKey);

    if (first_pos != std::string::npos)
        request_line = request_line.substr(this->boundryKey.length(), pos);
    else
    {
        this->error = BAD_REQUEST_400;
        throw "400";
    }

    this->body = this->body.substr(pos + 2);
    std::istringstream request_stream(request_line);
    while (std::getline(request_stream, line) && (i < 2))
    {
        if (i == 0)
        {
            line = line.substr(0, line.length() - 1);
            pos = line.find("Content-Disposition:");
            if (pos != std::string::npos) // 20 Content-Disposition length();
                line = line.substr(pos + 20);
            else
            {
                DEBUG_MODE &&std::cout << "Content-Disposition doesn't exist\n";
                this->error = BAD_REQUEST_400;
                throw "400";
            }

            content_disposition = split(line, ';');
            this->file_name = get_file_name(content_disposition);
            if (this->file_name.empty())
            {
                DEBUG_MODE &&std::cout << this->crlf_counter;
                DEBUG_MODE &&std::cout << "file name doesn't exist\n";
                this->error = BAD_REQUEST_400;
                throw "400";
            }
        }
        else
        {
            line = line.substr(0, line.length() - 1);
            pos = line.find("Content-Type:");
            if (pos == std::string::npos)
            {
                DEBUG_MODE &&std::cout << "Content-Type doesn't exist\n";
                this->error = BAD_REQUEST_400;
                throw "400";
            }
        }
        i++;
    }
}

void Http_request::ChunkedBoundaryBody(std::string request_line)
{
    size_t pos;
    if (this->boundryKey.empty())
        this->set_ChunkedBoundryKey();
    this->body += request_line;
    std::string buffer_write;
    buffer_write.clear();
    pos = set_crlf_ChunkedCounter(this->body);
    if (this->crlf_counter == 4 && file_name.empty())
    {
        std::string name_file;
        name_file = this->uploadDir;

        if (file_name.empty())
            check_boundry_header(pos);
        else
            return;
        if (this->error)
            throw "400";
        if (this->error == 0)
        {

            if (this->file.is_open())
            {
                this->file.close();
            }
            name_file += this->file_name;
            this->file.open(name_file.c_str());
            if (!this->file)
            {
                DEBUG_MODE &&std::cout << "ERREUR :IMPOSSIBLE D'OUVRIR LE FICHIER !" << std::endl;
                throw "400";
            }
        }
    }
    buffer_write = this->body;
    size_t pos1 = buffer_write.find(this->boundryKey);
    size_t pos2 = buffer_write.find(this->endBoundryKey);

    if (pos1 != std::string::npos && pos2 != pos1)
    {
        if (pos1 != 0)
        {
            this->file << buffer_write;
            buffer_write = buffer_write.substr(0, pos1);
        }
        this->crlf_counter = 0;
        this->file_name.clear();
        this->body = this->body.substr(pos1);
        this->file.close();
    }
    if (pos1 == std::string::npos) //|| pos2 == pos1
    {
        if (pos2 != std::string::npos)
        {
            this->body = this->body.substr(0, pos2);
            this->file << this->body;
            this->file.close();
            this->crlf_counter = 0;
            this->end_of_read = 1;
        }
        else
        {
            if (buffer_write.size() != 2)
                this->file << buffer_write;
            this->body.clear();
        }
    }
}

int Http_request::get_chunked_size()
{
    size_t pos;
    std::string str;
    int size;
    char *endPointer;

    pos = this->chunked_body.find("\r\n");
    if (pos != std::string::npos)
    {
        str = this->chunked_body.substr(0, pos);
        this->chunked_body = this->chunked_body.substr(pos + 2); /* (pos + 2) : to skip crlf \r\n ater the hexadecimal number (the chunck size)*/
    }
    if (str.empty())
    {
        this->again = 1;
        return (0);
    }
    size = std::strtol(str.c_str(), &endPointer, 16);
    if (endPointer == str.c_str())
    {
        DEBUG_MODE &&std::cout << str << "| " << this->chunked_body;
        std::cerr << "Erreur : conversion impossible." << std::endl;
        this->error = BAD_REQUEST_400;
        throw "400";
    }
    return (size);
}

void Http_request::chunked_and_boundary(std::string request_line)
{
    std::string new_body;
    new_body.clear();
    this->chunked_body += request_line;
    while (!this->chunked_body.empty() && this->end_of_read != 1)
    {
        if (this->chunked_size == 0 && !this->error)
        {
            if (this->check_1 == 0) /*this check_1 to skip the first \r\n*/
            {
                this->chunked_body = this->chunked_body.substr(2);
                this->chunked_size = get_chunked_size();
                this->check_1 = 1;
            }
            else
                this->chunked_size = get_chunked_size();
            if (this->again == 1)
            {
                this->again = 0;
                break;
            }
            if (this->chunked_size == 0)
            {
                DEBUG_MODE &&std::cout << "body : " << this->end_of_read;
                return;
            }
        }
        if (chunked_body.size() >= this->chunked_size && !this->error)
        {
            new_body = this->chunked_body.substr(0, this->chunked_size);
            this->chunked_body = this->chunked_body.substr(this->chunked_size + 2);
            this->chunked_size = 0;
            this->ChunkedBoundaryBody(new_body);
        }
        else

        {
            new_body = chunked_body.substr(0, chunked_body.size());
            this->chunked_size -= (new_body.size());
            this->chunked_body.clear();
            this->ChunkedBoundaryBody(new_body);
        }
    }
}

std::string Http_request::get_ChunkedFileExtention(void)
{
    std::string exetention;
    exetention.clear();
    if (this->header["Content-Type"].empty())
    {
        DEBUG_MODE &&std::cout << "Content Type doesn't exist" << std::endl;
        this->error = BAD_REQUEST_400;
        return exetention;
    }
    else
    {
        exetention = this->header["Content-Type"].substr(0, this->header["Content-Type"].length() - 1);
        exetention = strtrim(exetention, ' ');
        if (exetention == "text/html")
            exetention = ".html";
        else if (exetention == "text/css")
            exetention = ".css";
        else if (exetention == "application/javascript")
            exetention = ".js";
        else if (exetention == "application/json")
            exetention = ".json";
        else if (exetention == "application/xml")
            exetention = ".xml";
        else if (exetention == "text/plain")
            exetention = ".txt";
        else if (exetention == "text/csv")
            exetention = ".csv";
        else if (exetention == "text/markdown")
            exetention = ".md";
        else if (exetention == "image/png")
            exetention = ".png";
        else if (exetention == "image/jpeg")
            exetention = ".jpg";
        else if (exetention == "image/gif")
            exetention = ".gif";
        else if (exetention == "image/svg+xml")
            exetention = ".svg";
        else if (exetention == "image/x-icon")
            exetention = ".ico";
        else if (exetention == "image/webp")
            exetention = ".webp";
        else if (exetention == "audio/mpeg")
            exetention = ".mp3";
        else if (exetention == "audio/wav")
            exetention = ".wav";
        else if (exetention == "video/mp4")
            exetention = ".mp4";
        else if (exetention == "video/webm")
            exetention = ".webm";
        else if (exetention == "application/pdf")
            exetention = ".pdf";
        else if (exetention == "application/zip")
            exetention = ".zip";
        else if (exetention == "application/x-tar")
            exetention = ".tar";
        else if (exetention == "application/gzip")
            exetention = ".gz";
        else if (exetention == "application/x-bzip2")
            exetention = ".bz2";
        else if (exetention == "application/x-rar-compressed")
            exetention = ".rar";
        else if (exetention == "application/octet-stream")
            exetention = ".bin";
        else if (exetention == "application/msword")
            exetention = ".doc";
        else if (exetention == "application/vnd.openxmlformats-officedocument.wordprocessingml.document")
            exetention = ".docx";
        else if (exetention == "application/vnd.ms-excel")
            exetention = ".xls";
        else if (exetention == "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")
            exetention = ".xlsx";
        else if (exetention == "application/vnd.ms-powerpoint")
            exetention = ".ppt";
        else if (exetention == "application/vnd.openxmlformats-officedocument.presentationml.presentation")
            exetention = ".pptx";
        else
        {
            this->error = BAD_REQUEST_400;
            exetention.clear();
        }
    }
    return (exetention);
}

void Http_request::BinaryChunked(std::string request_line)
{
    std::string new_body;
    if (this->file_name.empty())
    {
        this->ChunkedFileExtention = get_ChunkedFileExtention();
        if (this->error)
            throw "400";
        if (!this->ChunkedFileExtention.empty())
        {
            std::string name_file;
            name_file = this->uploadDir;
            this->file_name = generateRandomfileName(10) + this->ChunkedFileExtention;
            name_file += this->file_name;
            if (this->file.is_open())
            {
                this->file.close();
            }
            this->file.open(name_file.c_str()); // uploadDir.c_str()
            if (!this->file)
            {
                DEBUG_MODE &&std::cout << "ERREUR :IMPOSSIBLE D'OUVRIR LE FICHIER !" << std::endl;
                throw "400";
            }
        }
    }

    new_body.clear();
    this->chunked_body += request_line;
    while (!this->chunked_body.empty() && !this->error)
    {
        if (this->error)
            throw "400";
        if (this->chunked_size == 0 && !this->error)
        {
            if (this->check_1 == 0) /*this check_1 to skip the first \r\n*/
            {
                this->chunked_body = this->chunked_body.substr(2);
                this->chunked_size = get_chunked_size();
                this->check_1 = 1;
            }
            else
                this->chunked_size = get_chunked_size();
            if (this->again == 1)
            {
                this->again = 0;
                break;
            }
            if (this->chunked_size == 0)
            {
                this->file.close();
                this->end_of_read = 1;
                DEBUG_MODE &&std::cout << "body : " << this->end_of_read << std::endl;
            }
        }
        if (chunked_body.size() > this->chunked_size) //&& !this->erro r
        {

            new_body = this->chunked_body.substr(0, this->chunked_size);
            this->file << new_body;
            this->chunked_body = this->chunked_body.substr(this->chunked_size + 2);
            this->chunked_size = 0;
        }
        else

        {
            new_body = chunked_body.substr(0, chunked_body.size());
            this->file << new_body;
            this->chunked_size -= (new_body.size());
            this->chunked_body.clear();
        }
    }
}

void Http_request::Binary(std::string request_line)
{
    this->body = request_line;
    if (this->file_name.empty())
    {
        this->ChunkedFileExtention = get_ChunkedFileExtention();
        if (this->error)
            throw "400";
        if (!this->ChunkedFileExtention.empty())
        {
            this->file_name = generateRandomfileName(10) + this->ChunkedFileExtention;
            //    std::string uploadDir = "/home/tkannane/goinfre/TAHA/";
            //    uploadDir += this->file_name;
            this->uploadDir += this->file_name;
            if (this->file.is_open())
            {
                this->file.close();
            }
            this->file.open(uploadDir.c_str()); // uploadDir.c_str()
            if (!this->file)
            {
                DEBUG_MODE &&std::cout << "ERREUR :IMPOSSIBLE D'OUVRIR LE FICHIER !" << std::endl;
                throw "400";
            }
        }
    }
    if (this->check_1 == 0) /*this check_1 to skip the first \r\n*/
    {
        this->body = this->body.substr(2);
        this->check_1 = 1;
    }
    this->isbinary = true;
    this->bodycounter += this->body.size();
    this->file << this->body;
    // this->body = this->body.substr(request_line.size());
    if (this->body.empty())
    {
        this->end_of_read = 1;
        this->file.close();
    }
}

void Http_request::SimpleForm(std::string request)
{
    this->body = request;
    if (request.size() <= 2)
        return;
    if (request.empty())
        this->cookie_file.close();
    if (this->check_1 == 0) /*this check_1 to skip the first \r\n*/
    {
        this->body = this->body.substr(2);
        this->check_1 = 1;
    }
    this->FN_body_receved = "/tmp/body_receved_" + generateRandomfileName(10);
    this->cookie_file.open(FN_body_receved.c_str()); // uploadDir.c_str()
    if (!cookie_file)
        DEBUG_MODE &&std::cout << "ERREUR :IMPOSSIBLE D'OUVRIR LE FICHIER !" << std::endl;
    cookie_file << this->body;
    this->cookie_file.close();

    this->bodycounter += body.size();
    this->body.clear();
    std::stringstream ss(this->get_header()["Content-Length"]);
    size_t contentLength;
    ss >> contentLength;
    if ((this->getbodycounter() >= contentLength))
    {
        this->end_of_read = 1;
    }
}
