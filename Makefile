################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          #
#          Wolf Richter <wolf@cs.cmu.edu>                                      #
#                                                                              #
################################################################################

default: HTTP_server

update: clean HTTP_server

echo_server:
	@gcc echo_server.c -o echo_server -Wall -Werror

echo_client:
	@gcc echo_client.c -o echo_client -Wall -Werror

HTTP_server:
	@gcc HTTP_server.c -o HTTP_server -Wall -Werror

clean:
	@rm HTTP_server
