# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/02/26 16:25:58 by jvaquer           #+#    #+#              #
#    Updated: 2021/03/23 11:21:19 by lucas            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Serv

C++ = clang++

FLAG = -Wall -Werror -Wextra -std=c++98

FLAGS =  -lssl -lcrypto

#srcs file
SRCS =			$(addprefix $(DIR_SRCS), $(SRC)) 
DIR_SRCS = ./srcs/
SRC = main.cpp parser.cpp get_client.cpp parse_conf.cpp \
	  run_server.cpp setup_serv.cpp init_command.cpp \
	  create_error_msg.cpp

OBJ = $(SRCS:.cpp=.o)

#libft file
SRCS_LIB = $(addprefix $(DIR_LIB), $(SRC_LIB))
DIR_LIB = ./srcs/libft/
SRC_LIB = ft_atoi.cpp ft_bzero.cpp ft_split.cpp

OBJ_LIB = $(SRCS_LIB:.cpp=.o)

#command file
SRCS_CMD = $(addprefix $(DIR_CMD), $(SRC_CMD))
DIR_CMD = ./srcs/commands/
SRC_CMD = nick.cpp user.cpp time.cpp \
		  motd.cpp info.cpp version.cpp \
		  pass.cpp join.cpp

OBJ_CMD = $(SRCS_CMD:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ) $(OBJ_LIB) $(OBJ_CMD)
		$(C++) $(FLAGS) $(OBJ) $(OBJ_LIB) $(OBJ_CMD) -o $(NAME)

clean :
		rm -rf $(OBJ) $(OBJ_CMD) $(OBJ_LIB)

fclean : clean
		rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re
