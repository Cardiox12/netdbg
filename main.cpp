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
    Selector        selector;
    auto            server = new Socket(AF_INET, SOCK_STREAM);
    std::string     host = argv[1];
    std::string     port = argv[2];

    try {
        server->connect(host, port);
    } catch ( std::exception &e ){
        std::cerr << e.what() << std::endl;
        Socket::release(&server);
        return EXIT_FAILURE;
    }

    selector.add(server);

    std::cout << "Connected to " << host << ":" << port << std::endl;
    while (true) {
        
    }
    return 0;
}
