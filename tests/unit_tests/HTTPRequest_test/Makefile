SRCSDIR	= ../../../srcs
vpath	%.cpp ../srcs:testsrcs:$(SRCSDIR):$(SRCSDIR)/server:$(SRCSDIR)/io:$(SRCSDIR)/utils:$(SRCSDIR)/config

SRCS	=	HTTPRequest.cpp \
			AIo.cpp \
			ASocket.cpp \
			ListenSocket.cpp \
			ServerSocket.cpp \
			utils.cpp \
			ClientSocket.cpp \
			Config.cpp \
			LocationDirective.cpp \
			ServerDirective.cpp  \
			Tokens.cpp \
			HTTPRequest_test.cpp

OBJSDIR	=	../objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I../srcs -I./testsrcs -I $(SRCSDIR)/server -I $(SRCSDIR)/io -I $(SRCSDIR)/utils -I $(SRCSDIR)/config -I $(SRCSDIR)/event

NAME	=	unit_test

CC		=	clang++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	+=	-g -fsanitize=address
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
