/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_serv.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoulin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:27:54 by lmoulin           #+#    #+#             */
/*   Updated: 2021/03/18 19:42:18 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/IRCserv.hpp"
#include <stdio.h>

void		sig_handler(int signal)
{
	if (signal == SIGINT)
	{
		for (std::vector<std::pair<SOCKET, Client> >::iterator it = g_tryPair.begin(); it != g_tryPair.end(); ++it)
			closesocket(it->first);
		closesocket(g_serv_sock);
		exit(0);
	}
}

int			setup_server()
{
	SOCKET		sock;
	SOCKADDR_IN sin;

	signal(SIGINT, sig_handler);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);

	if (sock == INVALID_SOCKET)
		throw std::exception();
	std::cout << "Socket created" << std::endl;

	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	if (bind(sock, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR)
		throw std::exception();
	std::cout << "Socket successfully binded to port number " << PORT << std::endl;

	if (listen(sock, 5) == SOCKET_ERROR)
		throw std::exception();
	std::cout << "Started listening for connection on port number " << PORT << "...\n";

	g_serv_sock = sock;
	return (0);
}
