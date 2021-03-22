/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/22 13:56:08 by jvaquer           #+#    #+#             */
/*   Updated: 2021/03/22 20:39:13 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"
#include "../../includes/IRCserv.hpp"
#include <vector>

static bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end())
	{
		if (!std::isdigit(*it))
			return false;
		++it;
	}
	return true;
}

void		check_realname(const std::string str, const size_t &client_idx, const Server &serv)
{
	if (str.size() < 1)
	{
		g_aClient[client_idx].second.send_reply(create_error(461, client_idx, serv, "USER"));
		throw std::exception();
	}
}

static void	check_user_registered(const std::string str, const std::vector<std::string> vect, const size_t &client_idx, const Server &serv)
{
	if (vect.size() < 4)
	{
		g_aClient[client_idx].second.send_reply(create_error(461, client_idx, serv, "USER"));
		throw std::exception();
	}
	if (!is_number(vect[2]))
		throw std::exception();
	for (size_t i = 0; i != g_aClient.size(); i++)
	{
		if (g_aClient[i].second.get_username() == vect[1] ||
			g_aClient[i].second.get_realname() == str)
		{
			g_aClient[client_idx].second.send_reply(create_error(462, client_idx, serv, str));throw std::exception();
		}
	}
}

void	user_command(const std::string &line, const size_t &client_idx, const Server &serv)
{
	std::vector<std::string> params;
	std::vector<std::string> args;
	std::string realname, line_new;

	try
	{
		//Split with : to store already realname
		params = ft_split(line, ":");
		realname = params[1];
		line_new = params[0];
		//On resplit par ' ' la premiere string recuperee par le premier split
		//comme ca les parametres sont bien separes
		args = ft_split(line_new, " ");
		check_realname(realname, client_idx, serv);
		check_user_registered(realname, args, client_idx, serv);
		g_aClient[client_idx].second.set_username(args[1]);
		g_aClient[client_idx].second.set_realname(realname);
		g_aClient[client_idx].second.set_mode(args[2]);
		std::cout << "* User Registered correctly" << std::endl;
	}
	catch(const std::exception &e) {return ; }
	
	//ERR_ALREADYREGISTRED
	//ERR_NEEDMOREPARAMS
}
