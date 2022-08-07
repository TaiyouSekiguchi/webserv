all		:	test

test	:	compile
			make test -C config_test		&& echo
			make test -C HTTPMethod_test	&& echo
			make test -C HTTPRequest_test	&& echo
			make test -C HTTPResponse_test	&& echo
			make test -C URI_test			&& echo
			make test -C utils_test			&& echo
			make test -C WebServ_test

compile	:
			make -C config_test
			make -C HTTPMethod_test
			make -C HTTPRequest_test
			make -C HTTPResponse_test
			make -C URI_test
			make -C utils_test
			make -C WebServ_test

.PHONY		:	all test compile
