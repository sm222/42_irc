#!/usr/bin/python3

import subprocess
import random

IP          = "157.208.28.99"
PORT        = 6667
PW          = "a"
SERVNAME    = "TEST"

def run():
    name =  "a" + str(random.randint(1, 99999999))
    cmd = f"docker run -it --rm weechat/weechat weechat -r '/server add {SERVNAME} {IP}/{PORT} -notls -password={PW}; /set irc.server.{SERVNAME}.username {name}; /set irc.server.{SERVNAME}.nicks {name}; /connect {SERVNAME}'"
    
    try:
        subprocess.run(cmd, shell=True, check=True)
    except Exception as e:
        print(f"[-] Error -> {e}")

if __name__ == "__main__":
    run()
