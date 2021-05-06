/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 00:41:24 by jvaquer           #+#    #+#             */
/*   Updated: 2021/05/06 11:31:51 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/IRCserv.hpp"
#include "../../includes/commands.hpp"
#include "../../includes/Disconnect.hpp"

void	kill_command(const std::string &line, std::list<Client>::iterator client_it, const MyServ &serv)
{
	std::vector<std::string> 	args;
	std::string					output;

	args = ft_split(line, " ");
	//Err Bad args number
	if (args.size() < 3)
	{
		client_it->push_to_buffer(create_msg(461, client_it, serv, args[0]));
		return;
	}
	//Err User is not oper
	if (client_it->get_is_oper() == false)
	{
		client_it->push_to_buffer(create_msg(481, client_it, serv));
		return;
	}

	std::list<Client>::iterator target;
	if ((target = find_client_by_iterator(args[1])) == g_aClient.end())
	{
		client_it->push_to_buffer(create_msg(401, client_it, serv, args[1]));
		return;
	}
	else if (&(*target) == &(*client_it))
	{
		client_it->push_to_buffer(create_msg(692, client_it, serv, "Can't KILL yourself"));
		return;
	}
	try
	{
		output = line.substr(line.find_first_of(':'), line.size());
		quit_command("QUIT " + args[1] + " " + output, target, serv);
	}
	catch (const QuitCommandException)
	{
		disconnect(&(*target), target);
	}
	catch (std::exception) { return; }
}
