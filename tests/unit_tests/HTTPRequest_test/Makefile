SRCSDIR	= ../../../srcs
vpath	%.cpp $(SRCSDIR):$(SRCSDIR)/server:$(SRCSDIR)/io:$(SRCSDIR)/utils:$(SRCSDIR)/exception:$(SRCSDIR)/config

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

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I $(SRCSDIR)/server -I $(SRCSDIR)/io -I $(SRCSDIR)/utils -I $(SRCSDIR)/exception -I $(SRCSDIR)/config -I $(SRCSDIR)/event

NAME	=	unit_test

#CC		=	c++
CC		=	clang++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	=	-g -fsanitize=address
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
