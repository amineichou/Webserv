/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryMethodes.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:22:53 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:43:58 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

 
 #include "request.hpp"

void Http_request::set_boundryKey(void)
{
    std::string boundary;

    if (!this->header["Content-Type"].empty())
    {
        boundary = this->header["Content-Type"];
        size_t pos = boundary.find("boundary=");
        boundary = boundary.substr(pos + 9, boundary.length());
        this->boundryKey = "\r\n--" + boundary + "\n";
        boundary = boundary.substr(0, boundary.length() - 1);
        this->endBoundryKey = "\r\n--" + boundary +"--\r\n";
    }
    else
        return ;
}
size_t Http_request:: set_crlf_counter(std::string body)
{
    size_t count = 0;
    size_t pos1 = 0;
    
    size_t pos = body.find("\r\n");
    pos1 = body.find(this->boundryKey);
    if (pos != pos1)
    {
        this->crlf_counter = 0;
        return (pos);
    }
    if(pos != std::string::npos  && this->crlf_counter == 0)
        count++;
    while (pos != std::string::npos && count < 5 )
    {
        count++;
        pos = body.find("\r\n", pos + 2);
    }
    if (pos1 != std::string::npos && pos != std::string::npos  )
        this->crlf_counter = count;
    else
        this->crlf_counter = 0;
    return pos;
}




void Http_request::BoundaryBody(std::string request_line)
{
    size_t pos;
    this->body += request_line;
    if (request_line.size() <= 2)
        return ;

    if (this->boundryKey.empty())
         this->set_boundryKey();
   
    std::string buffer_write;
    buffer_write.clear();  
    pos = set_crlf_counter(this->body);
    if (this->crlf_counter == 5 && this->file_name.empty())
        {
            std::string name_file;
            name_file = this->uploadDir ;
            check_boundry_header(pos);
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
                    DEBUG_MODE && std::cout << "ERREUR :IMPOSSIBLE D'OUVRIR LE FICHIER !" << std::endl;
                    throw "400";
                }
            }
            else
            {
                DEBUG_MODE && std::cout << "Bad request  numb 73\n";
                throw "400";
                
            }
        }
    buffer_write = this->body;
    size_t pos1 = buffer_write.find(this->boundryKey);
    size_t pos2 = buffer_write.find(this->endBoundryKey);

    if (pos1 != std::string::npos  && pos2 != pos1)
        {
            buffer_write = buffer_write.substr(0, pos1);
            this->crlf_counter = 0;
            this->file << buffer_write ;
            this->body = this->body.substr(pos1);
            this->file_name.clear();
            this->file.close();
        }
   if (pos1 == std::string::npos)
        {
            if (pos2 != std::string::npos)
            {
                this->body = this->body.substr(0, pos2);
                this->file << this->body;
                this->file.close();
                this->file_name.clear();
                this->crlf_counter = 0;
                this->end_of_read = 1;

            }
            else
                {
                    this->file << buffer_write;
                    this->body.clear();
                }
        }
}