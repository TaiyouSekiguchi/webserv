all			:	test

test		:	unit_test nl system_test

unit_test:
	make test -C unit_tests

system_test:
	make test -C system_tests

clean:
	make clean -C unit_tests
	make clean -C system_tests

re:		clean all

nl:
	@echo

.PHONY		:	all test unit_test system_test clean re nl
