SRCSDIR	= ../../../srcs
vpath	%.cpp	srcs:\
				testsrcs:\
				$(SRCSDIR)/config:\
				$(SRCSDIR)/exception \
				$(SRCSDIR)/io:\
				$(SRCSDIR)/server \
				$(SRCSDIR)/cgi \
				$(SRCSDIR)/utils
SRCS	=	Config.cpp \
			Tokens.cpp \
			ServerDirective.cpp \
			LocationDirective.cpp \
			AIo.cpp \
			ASocket.cpp \
			ListenSocket.cpp \
			RegularFile.cpp \
			ServerSocket.cpp \
			HTTPMethod.cpp \
			HTTPRequest.cpp \
			HTTPResponse.cpp \
			HTTPServer.cpp \
			URI.cpp \
			Dir.cpp \
			Stat.cpp \
			CGI.cpp \
			CGIEnv.cpp \
			Pipe.cpp \
			utils.cpp

SRCS	+=	ClientSocket.cpp
SRCS	+=	GET_test.cpp \
			DELETE_test.cpp \
			POST_test.cpp \
			others_test.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I./srcs \
			-I./testsrcs \
			-I$(SRCSDIR)/config \
			-I$(SRCSDIR)/event \
			-I$(SRCSDIR)/exception \
			-I$(SRCSDIR)/io \
			-I$(SRCSDIR)/server \
			-I$(SRCSDIR)/cgi \
			-I$(SRCSDIR)/utils
NAME	=	unit_test

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -g -fsanitize=address
CFLAGS	+=	-MMD -MP
CFLAGS	+=	-std=c++11
LINK	=	-lgtest -lgtest_main
RM		=	rm -rf

all			:	$(NAME)

$(NAME)		:	$(OBJS)
				$(CC) $(CFLAGS) $(LINK) $(INCLUDE) -o $@ $(OBJS)

$(OBJSDIR)/%.o	:	%.cpp
			@mkdir -p $(dir $@)
			$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

-include $(DEPS)

test		:	all
				@echo
				@./$(NAME) || :

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
