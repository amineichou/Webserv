/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:36 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/15 15:51:24 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void Response::DeleteMethod(int clientfd)
{
    this->clientFd = clientfd;
    if (this->statusCode != "000")
        onError(this->statusCode);
    if (resourceType == TYPE_FILE)
    {
        if (access(this->fullPath.c_str(), F_OK) == 0)
        {
            if (unlink(this->fullPath.c_str()) == 0)
                onSuccess("File was deleted successfully");
            else
                onError("500");
        }
        else
            onError("403");
    }
    else if (resourceType == TYPE_DIR)
        onError("405");
}
