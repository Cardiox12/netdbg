/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Selector.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbellavi <bbellavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 18:14:39 by bbellavi          #+#    #+#             */
/*   Updated: 2022/03/29 18:58:36 by bbellavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SELECTOR_HPP
# define SELECTOR_HPP

# include <sys/select.h>
# include <map>
# include <algorithm>
# include <vector>
# include "Socket.hpp"

namespace net {
    class Selector {
        std::map<int, Socket*>	m_sockets;
        fd_set					m_master;
        fd_set					m_copy;
        int						m_max_fd;
    public:
        typedef std::map<int, Socket*>				selector_type;
        typedef typename selector_type::value_type	selector_value_type;
        typedef typename selector_type::iterator	selector_type_iterator;
        typedef std::vector<Socket*>				selector_set_type;


        Selector() : m_sockets(), m_master(), m_copy(), m_max_fd(-1) {
            FD_ZERO(&m_master);
            FD_ZERO(&m_copy);
        }

        ~Selector() {
            std::for_each(m_sockets.begin(), m_sockets.end(),
                [](selector_value_type it){
                    it.second->close();
                    delete it.second;
                });
        }

        void add(Socket *socket){
            if ( socket != NULL ){
                int fd = socket->fd();

                m_max_fd = std::max(m_max_fd, fd);
                m_sockets.insert(std::make_pair(fd, socket));
                FD_SET(fd, &m_master);
            }
        }

        void remove(Socket *socket){
            if ( socket != NULL ){
                m_sockets.erase(socket->fd());
                FD_CLR(socket->fd(), &m_master);
            }
        }

        std::vector<Socket*> select(int seconds = -1) {
            timeval					timeout = (timeval){ .tv_sec = seconds, .tv_usec = 0 };
            std::vector<Socket*>	ready;

            m_copy = m_master;
            if ( seconds == -1 )
                ::select(m_max_fd + 1, &m_copy, NULL, NULL, NULL);
            else
                ::select(m_max_fd + 1, &m_copy, NULL, NULL, &timeout);
            std::for_each(m_sockets.begin(), m_sockets.end(),
                [&](selector_value_type p){
                    if ( FD_ISSET(p.first, &m_copy) ){
                        ready.push_back( p.second );
                    }
                });
            return std::move( ready );
        }
    };
}


#endif /* SELECTOR_HPP */
