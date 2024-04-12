#!/usr/bin/env python3
import socket
import threading
import time
import sys

HOST = '127.0.0.1'
PORT = 2000

commands = [
    ("PASS a\r\n", 0.5),
    ("NICK sm222\r\n", 0.5),
    ("USER antoine 0 * :sm222\r\n", 0.5),
    ("JOIN #a,#b,#c,#d\r\n", 0.5),
    ("PRIVMSG #a :000\r\n", 0.01),
    ("PRIVMSG #a :001\r\n", 0.01),
    ("PRIVMSG #a :002\r\n", 0.01),
    ("PRIVMSG #a :003\r\n", 0.01),
    ("PRIVMSG #a :004\r\n", 0.01),
    ("PRIVMSG #a :005\r\n", 0.01),
    ("PRIVMSG #a :006\r\n", 0.01),
    ("PRIVMSG #a :007\r\n", 0.01),
    ("PRIVMSG #a :008\r\n", 0.01),
    ("PRIVMSG #a :009\r\n", 0.01),
    ("PRIVMSG #a :010\r\n", 0.01),
    ("PRIVMSG #a :011\r\n", 0.01),
    ("PRIVMSG #a :012\r\n", 0.01),
    ("PRIVMSG #a :013\r\n", 0.01),
    ("PRIVMSG #a :014\r\n", 0.01),
    ("PRIVMSG #a :015\r\n", 0.01),
    ("PRIVMSG #a :016\r\n", 0.01),
    ("PRIVMSG #a :017\r\n", 0.01),
    ("PRIVMSG #a :018\r\n", 0.01),
    ("PRIVMSG #a :019\r\n", 0.01),
    ("PRIVMSG #a :020\r\n", 0.01),
    ("PRIVMSG #b :000\r\n", 0.01),
    ("PRIVMSG #b :001\r\n", 0.01),
    ("PRIVMSG #b :002\r\n", 0.01),
    ("PRIVMSG #b :003\r\n", 0.01),
    ("PRIVMSG #b :004\r\n", 0.01),
    ("PRIVMSG #b :005\r\n", 0.01),
    ("PRIVMSG #b :006\r\n", 0.01),
    ("PRIVMSG #b :007\r\n", 0.01),
    ("PRIVMSG #b :008\r\n", 0.01),
    ("PRIVMSG #b :009\r\n", 0.01),
    ("PRIVMSG #b :010\r\n", 0.01),
    ("PRIVMSG #b :011\r\n", 0.01),
    ("PRIVMSG #b :012\r\n", 0.01),
    ("PRIVMSG #b :013\r\n", 0.01),
    ("PRIVMSG #b :014\r\n", 0.01),
    ("PRIVMSG #b :015\r\n", 0.01),
    ("PRIVMSG #b :016\r\n", 0.01),
    ("PRIVMSG #b :017\r\n", 0.01),
    ("PRIVMSG #b :018\r\n", 0.01),
    ("PRIVMSG #b :019\r\n", 0.01),
    ("PRIVMSG #b :020\r\n", 0.01),
    ("PRIVMSG #c :000\r\n", 0.01),
    ("PRIVMSG #c :001\r\n", 0.01),
    ("PRIVMSG #c :002\r\n", 0.01),
    ("PRIVMSG #c :003\r\n", 0.01),
    ("PRIVMSG #c :004\r\n", 0.01),
    ("PRIVMSG #c :005\r\n", 0.01),
    ("PRIVMSG #c :006\r\n", 0.01),
    ("PRIVMSG #c :007\r\n", 0.01),
    ("PRIVMSG #c :008\r\n", 0.01),
    ("PRIVMSG #c :009\r\n", 0.01),
    ("PRIVMSG #c :010\r\n", 0.01),
    ("PRIVMSG #c :011\r\n", 0.01),
    ("PRIVMSG #c :012\r\n", 0.01),
    ("PRIVMSG #c :013\r\n", 0.01),
    ("PRIVMSG #c :014\r\n", 0.01),
    ("PRIVMSG #c :015\r\n", 0.01),
    ("PRIVMSG #c :016\r\n", 0.01),
    ("PRIVMSG #c :017\r\n", 0.01),
    ("PRIVMSG #c :018\r\n", 0.01),
    ("PRIVMSG #c :019\r\n", 0.01),
    ("PRIVMSG #c :020\r\n", 0.01),
    ("QUIT\r\n", 0.01)
]

def sendcommands(s):
    for cmd, delay in commands:
        s.sendall(cmd.encode())
        #print(f"Sent {cmd}")
        time.sleep(delay)
    print("[+] Everything has been Sent !")
    time.sleep(1)

def receiveresponses(s):
    while True:
        try:
          response = s.recv(4096).decode()
        except:
          break
          if not response:
            print("[+] Received Garbage -> Goodbye")
            break
        print(f"Recv: {response}")

if __name__ == "__main__":
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        send_thread = threading.Thread(target=sendcommands, args=(s,))
        recv_thread = threading.Thread(target=receiveresponses, args=(s,))
        send_thread.start()
        recv_thread.start()
        send_thread.join()
        sys.exit(1)
        #recv_thread.join()
