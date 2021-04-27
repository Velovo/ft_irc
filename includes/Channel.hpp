/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:41:44 by jvaquer           #+#    #+#             */
/*   Updated: 2021/04/26 17:56:42 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <unistd.h>
# include <iostream>
# include <string>
# include <vector>
# include "./Client.hpp"

class Client;
bool	pattern_match(std::string str, std::string pattern);

typedef struct Ban_id
{
	std::string		nickname;
	std::string		username;
	std::string		hostname;
	time_t			ban_date;
}		t_ban_id;

class Channel
{
	private:

		std::string		_name;
		std::string		_topic;
		std::string		_mode;
		
		//mode attributes
		std::string		_password;
		size_t			_limit;
		time_t			_creation;
		
	public:
		std::vector<Client*>		_operator;
		std::vector<Client*>		_users;
		std::vector<Client*>		_invite;
		std::vector<t_ban_id>		_ban;
		std::vector<Client*>		_voice;

		Channel() {}

		Channel(std::string name)
		{
			_name = name;
			_topic = "";
			time(&_creation);
		}

		~Channel() {}

		/*
		 ** getter
		*/
		std::string				get_password() const { return (_password); }
		std::string				get_name() const { return _name; }
		std::string				get_topic() const { return _topic; }
		std::string				get_mode() const { return _mode; }
		size_t					get_limit() const { return _limit; }
		time_t					get_creation_date() const { return _creation; }

		/*
		 ** setter
		*/

		void			set_password(std::string pass) { _password = pass; }
		void			set_name(std::string name) { _name = name; }
		void			set_mode(std::string mode) { _mode = mode; }
		void			set_limit(size_t limit) { _limit = limit; }
		void			set_topic(std::string topic) { _topic = topic; }

		void			remove_user_operator(const std::string usr_nickname)
		{
			for (std::vector<Client*>::iterator it = _operator.begin(); it != _operator.end(); )
			{
				if ((*it)->get_nickname() == usr_nickname)
				{
					_operator.erase(it);
					return ;
				}
				else
					it++;
			}
		}

		void			remove_user_ban(const t_ban_id &cli)
		{
			for (std::vector<t_ban_id>::iterator it = _ban.begin(); it != _ban.end(); it++)
			{
				if (pattern_match(cli.nickname, (*it).nickname))
					if (pattern_match(cli.username, (*it).username))
						if (pattern_match(cli.hostname, (*it).hostname))
						{
							_ban.erase(it);
							return ;
						}
			}
		}

		void			remove_user_voice(const std::string usr_nickname)
		{
			for (std::vector<Client*>::iterator it = _voice.begin(); it != _voice.end(); )
			{
				if ((*it)->get_nickname() == usr_nickname)
				{
					_voice.erase(it);
					return ;
				}
				else
					it++;				
			}
		}

		Channel			&operator=(const Channel &other)
		{
			_name = other.get_name();
			_password = other.get_password();
			_topic = other.get_topic();
			_users = other._users;
			return (*this);
		}
		Client			&operator[](int idx)
		{
			return (*_users[idx]);
		}

		bool			is_operator(Client cli)
		{
			for (size_t i = 0; i < _operator.size(); i++)
				if (*_operator[i] == cli)
					return (true);
			return (false);
		}
		bool			is_operator(Client *cli)
		{
			for (size_t i = 0; i < _operator.size(); i++)
				if (_operator[i]->get_nickname() == cli->get_nickname())
					return (true);
			return (false);
		}

		bool			is_voice(Client cli)
		{
			for (size_t i = 0; i < _voice.size(); i++)
				if (*_voice[i] == cli)
					return (true);
			return (false);
		}

		bool			is_ban(Client cli)
		{
			for (size_t i = 0; i < _ban.size(); i++)
			{
				if (pattern_match(cli.get_nickname(), _ban[i].nickname))
					if (pattern_match(cli.get_username(), _ban[i].username))
						if (pattern_match(cli.get_hostname(), _ban[i].hostname))
							return (true);
			}
			return (false);
		}

		bool			is_ban_struct(const t_ban_id &cli)
		{
			for (std::vector<t_ban_id>::iterator it = _ban.begin(); it != _ban.end(); it++)
			{
				if (pattern_match(cli.nickname, (*it).nickname))
					if (pattern_match(cli.username, (*it).username))
						if (pattern_match(cli.hostname, (*it).hostname))
							return true;
			}
			return false;
		}
		
		bool			is_user_in_chan(const Client cli)
		{
			for (size_t i = 0; i < _users.size(); ++i)
			{
				if (*_users[i] == cli)
					return true;
			}
			return false;
		}

		bool			is_mode(char c)
		{
			return (_mode.find(c) != std::string::npos ? true : false);
		}

		void			send_to_all(std::string msg)
		{
			for (size_t i = 0; i < _users.size(); i++)
				_users[i]->send_reply(msg);
		}

		void			send_to_all_except_one(Client except, const std::string &msg)
		{
			for (size_t i = 0; i < _users.size(); i++)
			{
				if (*_users[i] != except)
					_users[i]->send_reply(msg);
			}
		}
};

#endif
