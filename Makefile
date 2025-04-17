# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: moichou <moichou@student.1337.ma>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/04 23:24:36 by ymomen            #+#    #+#              #
#    Updated: 2025/04/16 12:02:51 by moichou          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CMD = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
SRC = main.cpp Server/server.cpp Server/CGI.cpp  config/ServerConfig.cpp config/parseConfig.cpp config/Route.cpp Request/request.cpp Request/requestGeters.cpp Request/valideRequest.cpp Response/Response.cpp Response/Get.cpp Response/Delete.cpp Response/Post.cpp Request/requestHelpers.cpp Request/ChunkedMethodes.cpp Request/BoundaryMethodes.cpp
HEADER = Server/server.hpp config/ServerConfig.hpp config/Route.hpp Request/request.hpp Response/Response.hpp
OBJ = $(SRC:.cpp=.o)
NAME = webserv
RM = rm -f

all: $(NAME)

$(NAME): $(OBJ)
		$(CMD) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(HEADER)
	$(CMD) $(FLAGS) -c $< -o $@

clean :
		$(RM) $(OBJ)

fclean:clean
		$(RM) $(NAME)

re: fclean all

