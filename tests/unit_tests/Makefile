all			:	test

test		:	config nl utils

config:
	make test -C config_test

utils:
	make test -C utils_test

nl:
	@echo

.PHONY		:	all test config utils nl
