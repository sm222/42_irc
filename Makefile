CC		= c++
FLAGS	= -Wall -Wextra -Werror 	# std98 is VERY strict on linux...
#FLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -g
#FLAGS	= -Wall -Wextra -Werror -std=c++98 -O2

TARGET		= ircserv
OBJECTS		:= $(patsubst src/%.cpp,obj/%.o,$(wildcard src/*.cpp))

$(TARGET): $(shell mkdir -p obj) $(OBJECTS)
	@$(CC) $(OBJECTS) $(FLAGS) -o $@			&& echo "\033[38;5;46m"		"[+] [$(TARGET)] Compilation Completed"		"!\033[0m"

$(OBJECTS): obj/%.o : src/%.cpp
	@$(CC) $(FLAGS) -c $< -o $@					&& echo "\033[38;5;121m"	"[-] Compiling $< ..."						"\033[0m"

clean:
	@rm -f $(OBJECTS)							&& echo "\033[38;5;49m"		"[+] Cleanup complete !"					"\033[0m"

fclean: clean
	@rm -f $(TARGET)							&& echo "\033[38;5;49m"		"[+] Executable removed !"					"\033[0m"

re: fclean $(TARGET)

leak: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

run: fclean $(TARGET)
	@./$(TARGET)

docker:
	cd weechat_docker && docker build -t weechat-image .
	docker run -it --rm weechat-image 
	@if [ -z "$$(docker ps -a -q --filter ancestor=weechat-image )" ]; then \
		if docker images weechat-image  | awk '{ print $$1 }' | grep -q weechat-image ; then \
			echo "No active containers. Removing the weechat-image ..."; \
			docker rmi weechat-image:latest; \
		else \
			echo "weechat-image does not exist."; \
		fi \
	else \
		echo "There are active containers using the weechat-image. Deletion canceled."; \
	fi

ip:
	@ifconfig | grep 'inet 10'

wee:
	@docker run -ti weechat/weechat

.PHONY: all clean fclean re leak run docker
