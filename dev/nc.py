#!/usr/bin/env python3
import socket
import threading
import time
import sys

HOST = '10.12.6.8'
PORT = 2000

commands = [
    ("PASS a\r\n", 0.5),
    ("NICK sm222\r\n", 0.5),
    ("USER antoine 0 * :sm222\r\n", 0.5),
    ("JOIN #a\r\n", 0.5),
    ("PRIVMSG #a :000\r\n", 0.5),
    ("PRIVMSG #a :001\r\n", 0.5),
    ("PRIVMSG #a :002\r\n", 0.5),
    ("PRIVMSG #a :003\r\n", 0.5),
    ("PRIVMSG #a :004\r\n", 0.5),
    ("PRIVMSG #a :005\r\n", 0.5),
    ("PRIVMSG #a :006\r\n", 0.5),
    ("PRIVMSG #a :007\r\n", 0.5),
    ("PRIVMSG #a :008\r\n", 0.5),
    ("PRIVMSG #a :009\r\n", 0.5)
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
        #recv_thread.join()
