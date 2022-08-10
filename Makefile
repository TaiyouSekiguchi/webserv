vpath	%.cpp	srcs: \
				srcs/server: \
				srcs/io: \
				srcs/config: \
				srcs/utils: \
				srcs/webserv: \
				srcs/event
SRCS	=	main.cpp \
			WebServ.cpp \
			EventQueue.cpp \
			AIo.cpp \
			ASocket.cpp \
			ListenSocket.cpp \
			ServerSocket.cpp \
			RegularFile.cpp \
			Config.cpp \
			LocationDirective.cpp \
			ServerDirective.cpp \
			Tokens.cpp \
			HTTPServer.cpp \
			HTTPRequest.cpp \
			HTTPMethod.cpp \
			HTTPResponse.cpp \
			AEvent.cpp \
			AcceptClientEvent.cpp \
			HTTPServerEvent.cpp \
			utils.cpp \
			Stat.cpp \
			Dir.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I srcs/server \
			-I srcs/utils \
			-I srcs/io \
			-I srcs/exception \
			-I srcs/config \
			-I srcs/webserv \
			-I srcs/event
NAME	=	webserv

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	+=	-MMD -MP
CFLAGS	+=	-std=c++98
RM		=	rm -rf

all			:	$(NAME)

$(NAME)		:	$(OBJS)
				$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(OBJS)

$(OBJSDIR)/%.o	:	%.cpp
			@mkdir -p $(dir $@)
			$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

-include $(DEPS)

clean		:
				-$(RM) $(OBJS) $(DEPS)

fclean		:	clean
				-$(RM) $(NAME)

re			:	fclean all

print		:	CFLAGS	+= -D PRINT_FLAG
print		:	re

debug		:	CFLAGS	+=	-g
debug		:	re

address		:	CFLAGS	+=	-g -fsanitize=address
address		:	re

ifeq ($(shell uname),Darwin)
leak		:	CC		=	/usr/local/opt/llvm/bin/clang++
endif
leak		:	CFLAGS	+=	-g -fsanitize=leak
leak		:	re

.PHONY		:	all clean fclean re debug address leak
