/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucas <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/23 10:57:31 by lucas             #+#    #+#             */
/*   Updated: 2021/03/30 16:21:26 by lucas            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/IRCserv.hpp"
#include "../../includes/MyServ.hpp"
#include "../../includes/commands.hpp"


// Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
//               / "0"
//
//               channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring
//                [ ":" chanstring ]
//                key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
//                  ; any 7-bit US_ASCII character,
//                 ; except NUL, CR, LF, FF, h/v TABs, and " "

bool	error_chan_name(const std::string tmp, const std::string::iterator it_s)
{
	std::string::const_iterator it = tmp.begin();

	if (*it != '&' && *it != '#' && *it != '+' && *it != '!')
		return (true);
	if (*it_s == 0x07 || *it_s == 0x08 || *it_s == '\r' || *it_s == '\n' ||
			*it_s == 0x0a || *it_s == ' ' || *it_s == ',' || (*it_s == ':' && it_s != tmp.begin() + 1))
		return (true);
	return (false);
}

bool error_chan_key(const std::string::iterator it_k)
{
	if (!(*it_k > 0 && *it_k < 6) || *it_k == 7 || *it_k == 8 ||
		*it_k == 12 || *it_k == 14 || *it_k == 31 || (*it_k > 30 && *it_k <= 127))
		return (false);
	return (true);
}

int		check_name_and_key(std::map<std::string, std::string> &chan)
{
	std::map<std::string, std::string>::iterator	it = chan.begin();
	std::string					tmp;

	while (it != chan.end())
	{
		tmp = it->first;
		for (std::string::iterator it_s = tmp.begin(); it_s != tmp.end(); it_s++)
		{
			if (error_chan_name(tmp, it_s))
			{
				it = chan.erase(it);
				return (0);
			}
		}
		for (std::string::iterator it_k = it->second.begin(); it_k != it->second.end(); it_k++)
		{
			if (it->second.compare("") && error_chan_key(it_k))
			{
				it = chan.erase(it);
				return (0);
			}
		};
		it++;
	}
	return (chan.size());
}

int		try_enter_chan(const std::map<std::string, std::string>::iterator it, const size_t &client_idx,
															bool &enter)
{
	int		i = find_channel(it->first);

	for (std::vector<Client>::iterator it = g_vChannel[i]._users.begin(); it != g_vChannel[i]._users.end(); it++)
	{
		if (*it == g_aClient[client_idx].second)
		{
			enter = true;
			return (0);
		}
	}
	if (it->second == g_vChannel[i].get_password())
		return (1);
	return (0);
}

void	create_channel(const std::map<std::string, std::string>::iterator it, const size_t &client_idx, bool &enter)
{
	Channel		chan(it->first, it->second);

	chan._users.push_back(g_aClient[client_idx].second);
	chan.set_operator(g_aClient[client_idx].second);
	chan.set_mode("+nt");
	g_vChannel.push_back(chan);
	enter = true;
}

void	add_client_to_channel(const std::map<std::string, std::string>::iterator it, const size_t &client_idx, bool &enter)
{
	int		i = find_channel(it->first);

	g_vChannel[i]._users.push_back(g_aClient[client_idx].second);
	enter = true;
}

void	make_channel_pair(const std::vector<std::string> &params, std::map<std::string, std::string> &tmp,
										std::vector<std::string> &chan_name)
{
	std::vector<std::string>	key;

	chan_name = ft_split(params[1], ",");
	if (params.size() > 2)
		key = ft_split(params[2], ",");
	if (key.size() < chan_name.size())
		while (key.size() < chan_name.size())
			key.push_back("");
	for (size_t i = 0; i < chan_name.size(); i++)
		tmp.insert(std::make_pair(chan_name[i], key[i]));
}

void	join_command(const std::string &line, const size_t &client_idx, const MyServ &serv)
{
	std::vector<std::string>			params;
	std::vector<std::string>			chan_name;
	std::map<std::string, std::string>	tmp;
	bool								enter;

	params = ft_split(line, " ");
	if (params.size() < 2)
	{
		g_aClient[client_idx].second.send_reply(create_msg(461, client_idx, serv, params[0]));
		return ;
	}
	make_channel_pair(params, tmp, chan_name);
	if (!check_name_and_key(tmp))
	{
		g_aClient[client_idx].second.send_reply(create_msg(476, client_idx, serv, chan_name[0]));
		return ;
	}
	enter = false;
	if (tmp.empty())
		return ;
	for (std::map<std::string, std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		if (find_channel(it->first) == -1)
		{
			// std::cout << "executed" << std::endl;
			create_channel(it, client_idx, enter);
			g_aClient[client_idx].second.send_reply(":" + g_aClient[client_idx].second.get_nickname() + "!"
				+ g_aClient[client_idx].second.get_username() + "@" + g_aClient[client_idx].second.get_hostname() + " JOIN " + it->first + "\r\n");
			names_command("names " + it->first, client_idx, serv);
		}
		else
		{
			if (try_enter_chan(it, client_idx, enter))
			{
				add_client_to_channel(it, client_idx, enter);
				names_command("names " + it->first, client_idx, serv);
			}
		}
	}
	if (enter == false)
		g_aClient[client_idx].second.send_reply(create_msg(403, client_idx, serv, chan_name[0]));
}
