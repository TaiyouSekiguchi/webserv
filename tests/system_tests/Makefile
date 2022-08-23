WEBSERVDIR	=	../../
WEBSERV		=	$(WEBSERVDIR)/webserv

all			:	$(WEBSERV)

test		:	$(WEBSERV)
				./$(WEBSERV) default.conf > /dev/null &
				@echo
				@python -m unittest discover testsrcs || :
				@pkill $(notdir $(WEBSERV))

$(WEBSERV)	: 	dummy
				make -C $(WEBSERVDIR)

clean		:
				make -C $(WEBSERVDIR) clean

fclean		:	clean
				make -C $(WEBSERVDIR) fclean

re			:	fclean all

dummy:

.PHONY		:	all clean fclean re dummy test
