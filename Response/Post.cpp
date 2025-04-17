/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymomen <ymomen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:23:51 by ymomen            #+#    #+#             */
/*   Updated: 2025/03/04 23:24:00 by ymomen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void Response::PostMethod(int clientfd)
{
    this->clientFd = clientfd;
    if (this->statusCode != "000" && this->statusCode != "200")
        this->onError(statusCode);
    else if (this->isCgi())
    {
        setResponse();
    }
    else
        this->onSuccess("Uploaded Successful");
}