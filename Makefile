CC		= c++
#FLAGS	= -Wall -Wextra -Werror -g
FLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -g
# FLAGS	= -Wall -Wextra -Werror -std=c++98 -O2
#FLAGS	= -g 

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

run: re $(TARGET) ip
	@./$(TARGET) 2000 a

docker-clean:
	@docker ps -q --filter ancestor=weechat/weechat | xargs docker stop

docker-clear:
	@docker kill $$(docker ps -q) || true
	@docker system prune -af || true

ip:
	@ifconfig -l | xargs -n1 ipconfig getifaddr | grep 10. || true
	@echo -n 'cd ' 
	@pwd

client:
	@docker run -ti weechat/weechat

wee:
	@./dev/run_weechat.sh

wee-d:
	@./dev/run_weechat.sh -d

term:
	@./dev/double_term.sh

shit: term run

.PHONY: all clean fclean re leak run docker
