#include "../includes/IRCserv.hpp"
#include "../includes/MyServ.hpp"
#include <ctime>

//called at each message received
static void re_init_serv_class(MyServ &serv)
{
	FD_ZERO(&serv.get_readfs());
	FD_ZERO(&serv.get_writefs());
	FD_ZERO(&serv.get_exceptfs());
	{
		serv.set_max_fd(g_aClient.empty() ? 0 : g_aClient[0].first);
		if (serv.get_max_fd() < g_serv_sock)
			serv.set_max_fd(g_serv_sock);
	}
	serv.set_timeout(3);
}

static void push_fd_to_set(MyServ &serv)
{
	//push to server to read set
	FD_SET(g_serv_sock, &serv.get_readfs());
	//push all client fd to all 3 set
	for (std::vector<std::pair<SOCKET, Client> >::iterator ite = g_aClient.begin(); ite != g_aClient.end(); ++ite)
	{
		FD_SET(ite->first, &serv.get_readfs());
		// FD_SET(*ite, &serv.get_writefs());
		// FD_SET(*ite, &serv.get_exceptfs());
	}
}

void		disconnect_client(size_t &i)
{
	closesocket(g_aClient[i].first);
	std::cout << "* User disconnected from: " << inet_ntoa(g_aClient[i].second.sock_addr.sin_addr)
		<< ":" << ntohs(g_aClient[i].second.sock_addr.sin_port) << std::endl;
	g_aClient.erase(g_aClient.begin() + i);
	i--;
}

void ping_if_away(const size_t &client_idx, const MyServ &serv)
{
	time_t time_compare; //may be optimized better

	time(&time_compare);
	//si je lui ai pas deja envoye un ping et si ca fait plus de 30sec que je l'ai pas ping
	if (g_aClient[client_idx].second.get_ping_status() == false && time_compare - g_aClient[client_idx].second.get_last_activity() > PING)
	{
		g_aClient[client_idx].second.send_reply("PING :" + serv.get_hostname() + "\r\n");
		g_aClient[client_idx].second.set_ping_status(true);
	}
}

bool kick_if_away(size_t &client_idx, const MyServ &serv)
{
	time_t time_compare; //may be optimized better

	time(&time_compare);
	if (g_aClient[client_idx].second.get_ping_status() == true && time_compare - g_aClient[client_idx].second.get_last_activity() > TIMEOUT)
	{
		disconnect_client(client_idx);
		return true;
	}
	return false;
}

void run_server(MyServ &serv)
{
	int readyfd;
	char c[BUFF_SIZE + 1];

	while (1)
	{
		re_init_serv_class(serv);
		push_fd_to_set(serv);

		readyfd = select(serv.get_max_fd() + 1, &serv.get_readfs(), &serv.get_writefs(), &serv.get_exceptfs(), &serv.get_timeout());

		try_accept_user(&serv);
		for (size_t i = 0; i != g_aClient.size(); ++i)
		{
			ping_if_away(i, serv);
			//si je l'ai kick car ca fait trop longtemps qu'il a pas rep alors forcement je vais pas check ses demandes
			if (kick_if_away(i, serv) == true)
				;
			else if (FD_ISSET(g_aClient[i].first, &serv.get_readfs()))
			{
				ft_bzero((char *)c, sizeof(c));
				int ret = recv(g_aClient[i].first, &c, BUFF_SIZE, 0);
				if (ret <= 0)
					disconnect_client(i);
				else if (ret > 0)
				{
					c[ret] = '\0';
					try
					{
						parser(c, i, serv);
					}
					catch(const IncorrectPassException &e){ disconnect_client(i); }
					catch(const QuitCommandException &e){ disconnect_client(i); }
				}
			}
		}
	}
}
