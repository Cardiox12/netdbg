/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbellavi <bbellavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 23:03:44 by bbellavi          #+#    #+#             */
/*   Updated: 2022/03/29 23:03:46 by bbellavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/Socket.hpp"
#include "network/Selector.hpp"
#include <iostream>

void release_socket(net::Socket **socket){
    if ( *socket != nullptr ){
        (*socket)->close();
        delete *socket;
        *socket = nullptr;
    }
}

int main(int argc, char *argv[]) {
    using namespace net;

    if ( argc != 3 ){
        fprintf(stderr, "usage: netdbg host port\n");
        return EXIT_FAILURE;
    }
    Selector                                selector;
    auto                                    server = new Socket(AF_INET, SOCK_STREAM);
    std::string                             host = argv[1];
    std::string                             port = argv[2];
    std::string                             buff;
    int                                     optval = 1;

    server->setsockopt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    try {
        server->connect(host, port);
    } catch ( std::exception &e ){
        fprintf(stderr, "%s\n", e.what());
        Socket::release(&server);
        return EXIT_FAILURE;
    }

    selector.add(server);
    selector.add(STDIN_FILENO);

    printf("Connected to %s:%s\n", host.c_str(), port.c_str());
    while (true) {

        std::string                             buff;
        std::vector<std::pair<int, Socket*>>    ready = selector.select();

        ready = selector.select();

        std::for_each(ready.begin(), ready.end(),[&](std::pair<int, Socket*> p){
            Socket *client = p.second;

            if ( client == nullptr && p.first == STDIN_FILENO ){
                std::getline(std::cin, buff);
                buff += "\n";

                printf("Sending  [%d bytes]: %s", buff.size(), buff.c_str());
                server->send(buff);
                printf("Sent     [%d bytes].\n", buff.size());
            } else {
                if ( client->recv(buff) == 0 ){
                    printf("Peer has closed the connection.\n");
                    release_socket(&client);
                    exit(EXIT_SUCCESS);
                }
                printf("Received [%d bytes]: %.*s\n", buff.size(), buff.size(), buff.c_str());
            }
        });
    }
    return EXIT_SUCCESS;
}