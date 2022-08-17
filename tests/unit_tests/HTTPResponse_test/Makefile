SRCSDIR	= ../../../srcs
vpath	%.cpp	testsrcs: \
				$(SRCSDIR)/config: \
				$(SRCSDIR)/io: \
				$(SRCSDIR)/server: \
				$(SRCSDIR)/utils

SRCS	=	Config.cpp \
			LocationDirective.cpp \
			ServerDirective.cpp \
			Tokens.cpp \
			AIo.cpp \
			ASocket.cpp \
			ServerSocket.cpp \
			ListenSocket.cpp \
			RegularFile.cpp \
			HTTPRequest.cpp \
			HTTPResponse.cpp \
			HTTPMethod.cpp \
			Stat.cpp \
			Dir.cpp \
			URI.cpp \
			CGI.cpp \
			CGIEnv.cpp \
			Pipe.cpp \
			utils.cpp
			HTTPServer.cpp

SRCS	+=	ClientSocket.cpp
SRCS	+=	Response_test.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I $(SRCSDIR)/exception \
			-I $(SRCSDIR)/io \
			-I $(SRCSDIR)/event \
			-I $(SRCSDIR)/config \
			-I $(SRCSDIR)/utils \
			-I $(SRCSDIR)/server \
			-I $(SRCSDIR)/webserv

NAME	=	unit_test

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	+=	-g -fsanitize=address
CFLAGS	+=	-MMD -MP
CFLAGS	+=	-std=c++11
LINK	=	-lgtest -lgtest_main
RM		=	rm -rf

all			:	$(NAME)

$(NAME)		:	$(OBJS)
				$(CC) $(CFLAGS) $(LINK) $(INCLUDE) -I /usr/local/opt/curl/include -lcurl -o $@ $(OBJS)

$(OBJSDIR)/%.o	:	%.cpp
			@mkdir -p $(dir $@)
			$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

-include $(DEPS)

test		:	all
				@echo
				@./$(NAME)

clean		:
				-$(RM) $(OBJS) $(DEPS)

fclean		:	clean
				-$(RM) $(NAME)

re			:	fclean all

ifeq ($(shell uname),Darwin)
leak		:	CC		=	/usr/local/opt/llvm/bin/clang++
endif
leak		:	CFLAGS	+=	-g -fsanitize=leak
leak		:	re

.PHONY		:	all clean fclean re leak test
