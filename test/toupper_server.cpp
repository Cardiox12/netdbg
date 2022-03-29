/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   toupper_server.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbellavi <bbellavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 18:35:10 by bbellavi          #+#    #+#             */
/*   Updated: 2022/03/29 20:35:25 by bbellavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include "../network/Socket.hpp"
# include "../network/Selector.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int		main(int argc, char *argv[]){
    using namespace net;

	if ( argc != 3 ){
		std::cout << "usage: toupper_server host port" << std::endl;
		return EXIT_FAILURE;
	}
	Socket					                *server = new Socket(AF_INET, SOCK_STREAM);
	std::string				                host = argv[1];
	std::string				                port = argv[2];
	std::vector<std::pair<int, Socket*>>	clients;
	Selector				                selector;
    int                                     optval = 1;

	server->setsockopt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	server->bind(host, port);
	selector.add(server);
	
	server->listen();
	
	while ( true ){
		clients = selector.select();

		std::for_each(clients.begin(), clients.end(), [&](std::pair<int, Socket*> p){
            Socket *client = p.second;

			// Accept socket
			if ( client->fd() == server->fd() ){
				Socket *conn = server->accept();

				selector.add(conn);
				std::cout << "New connection from: " << conn->storage() << std::endl;
			} else {
				std::string buffer;

				if ( client->recv(buffer) == 0){
					selector.remove(client);
				} else {
					std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
					client->send(buffer);
				}
			}
		});
	}
}
#pragma clang diagnostic pop