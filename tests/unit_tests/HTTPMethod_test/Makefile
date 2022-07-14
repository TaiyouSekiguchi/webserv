SRCSDIR	= ../../../srcs
vpath	%.cpp	srcs:\
				testsrcs:\
				$(SRCSDIR)/server:\
				$(SRCSDIR)/config:\
				$(SRCSDIR)/socket:\
				$(SRCSDIR)/utils
SRCS	=	ClientSocket.cpp \
			ListenSocket.cpp \
			ServerSocket.cpp \
			ASocket.cpp \
			Config.cpp \
			Tokens.cpp \
			ServerDirective.cpp \
			LocationDirective.cpp \
			HTTPRequest.cpp \
			HTTPMethod.cpp \
			utils.cpp

SRCS	+=	GET_test.cpp \
			DELETE_test.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I./srcs \
			-I$(SRCSDIR)/server \
			-I$(SRCSDIR)/config \
			-I$(SRCSDIR)/socket \
			-I$(SRCSDIR)/exception \
			-I$(SRCSDIR)/utils
NAME	=	unit_test

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -g
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
