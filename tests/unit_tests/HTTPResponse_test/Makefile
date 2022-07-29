SRCSDIR	= ../../../srcs
vpath	%.cpp	testsrcs: \
				$(SRCSDIR)/config: \
				$(SRCSDIR)/socket: \
				$(SRCSDIR)/utils: \
				$(SRCSDIR)/server

SRCS	=	ASocket.cpp \
			EventQueue.cpp \
			ClientSocket.cpp \
			ServerSocket.cpp \
			ListenSocket.cpp \
			Tokens.cpp \
			Config.cpp \
			LocationDirective.cpp \
			ServerDirective.cpp \
			HTTPServer.cpp \
			HTTPRequest.cpp \
			utils.cpp \
			HTTPMethod.cpp \
			Stat.cpp \
			Dir.cpp \
			HTTPResponse.cpp
			
SRCS	+=	Response_test.cpp 

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I $(SRCSDIR)/exception \
			-I $(SRCSDIR)/socket \
			-I $(SRCSDIR)/config \
			-I $(SRCSDIR)/utils \
			-I $(SRCSDIR)/server

NAME	=	unit_test

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror
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

.PHONY		:	all clean fclean re debug address leak test