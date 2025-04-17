/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 23:24:30 by ymomen            #+#    #+#             */
/*   Updated: 2025/04/16 11:55:52 by moichou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server/server.hpp"



int main(int ac, char **av) 
{
    std::vector<ServerConfig> config;
    ServerConfig defaultServ;
    Route defaultRoute;
    if (ac <= 1)
    {
        printStyledError("warning", "no confing was passed to the server. setting up default values", 0);
        defaultServ.setGlobalRoot("html");
        std::cout << "setting up `html` as global route" << std::endl;
        defaultServ.setServerNames("localhost");
        defaultServ.setPorts("8080", 0);
        std::cout << "server `localhost` running on port 8080" << std::endl;
        defaultServ.setServersCount(1);
        std::cout << "making a route for `/` with autoindex on" << std::endl;
        defaultRoute.setPath("/");
        defaultRoute.setIndex("index.html");
        defaultRoute.setAutoIndex(true);
        defaultRoute.setAcceptedMethods("GET");
        defaultServ.setRoutes(defaultRoute);

        config.push_back(defaultServ);
    }

    else if (ac > 1 && parseConfig(av[1], config))
        return 1;

    signal(SIGPIPE, SIG_IGN);

    try {
        Server server;
        server.creatsockets(config);
        server.listenonsockets();
        server.configEpool();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...)
    {
        std::cerr <<"Uknowing error"<<std::endl;
    }
    return 0;
}