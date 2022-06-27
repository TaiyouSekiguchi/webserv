vpath	%.cpp srcs:testsrcs
SRCS	=	ClientSocket.cpp echoserver_test.cpp

OBJSDIR	=	./objs
OBJS	=	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.o))
DEPS    =	$(addprefix $(OBJSDIR)/, $(SRCS:.cpp=.d))

INCLUDE =	-I ./srcs
NAME	=	system_test

CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror
CFLAGS	+=	-MMD -MP
CFLAGS	+=	-std=c++11
LINK	=	-lgtest -lgtest_main
RM		=	rm -rf

WEBSERVDIR	=	../../
WEBSERV		=	$(WEBSERVDIR)/webserv

all			:	$(WEBSERV) $(NAME)

$(NAME)		:	$(OBJS)
				$(CC) $(CFLAGS) $(LINK) $(INCLUDE) -o $@ $(OBJS)

$(OBJSDIR)/%.o	:	%.cpp
			@mkdir -p $(dir $@)
			$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

-include $(DEPS)

$(WEBSERV)	: 	dummy
				make -C $(WEBSERVDIR)

test		:	all
				./$(WEBSERV) &
				@echo
				@./$(NAME) || :
				@pkill $(notdir $(WEBSERV))

clean		:
				make -C $(WEBSERVDIR) clean
				-$(RM) $(OBJS) $(DEPS)

fclean		:	clean
				make -C $(WEBSERVDIR) fclean
				-$(RM) $(NAME)

re			:	fclean all

dummy:

.PHONY		:	all clean fclean re dummy test
