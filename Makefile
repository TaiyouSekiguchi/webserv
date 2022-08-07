vpath	%.cpp	srcs: \
				srcs/server: \
				srcs/socket: \
				srcs/config: \
				srcs/cgi: \
				srcs/utils
SRCS	=	main.cpp \
			HTTPServer.cpp \
			EventQueue.cpp \
			ASocket.cpp \
			ListenSocket.cpp \
			ServerSocket.cpp \
			Config.cpp \
			LocationDirective.cpp \
			ServerDirective.cpp \
			Tokens.cpp \
			HTTPRequest.cpp \
			utils.cpp \
			HTTPMethod.cpp \
			Stat.cpp \
			Dir.cpp \
			HTTPResponse.cpp \
			CGI.cpp \
			CGIEnv.cpp \
			URI.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I srcs/server \
			-I srcs/utils \
			-I srcs/socket \
			-I srcs/exception \
			-I srcs/cgi \
			-I srcs/config
NAME	=	webserv

#CC		=	c++
CC		=	clang++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	=	-g -fsanitize=address
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
