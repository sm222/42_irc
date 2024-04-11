#!/usr/bin/env python3

import socket
import threading
import time
import sys

# First arg = PORT, Default = 2000

# Settings
MESSAGE_COUNT           = 30       # Only changing this is the smart way to do it
CHANNEL_NAME            = "#a"
SERVER_PASSWORD         = "a"
THREAD_TO_SPAWN         = 15        # THREAD AND TIME HAS LIMITS - DONT FUCK YOUR SESSION (keep it like this)
TIME_BETWEEN_MESSAGE    = 0.05      # THREAD AND TIME HAS LIMITS - DONT FUCK YOUR SESSION (keep it like this)

# Do not Edit
SEND_THREAD_LIST        = [None] * THREAD_TO_SPAWN
RECV_THREAD_LIST        = [None] * THREAD_TO_SPAWN
IP                      = None
PORT                    = None
RECV_BUFFER_SIZE        = 4096



def SEND(socket : socket, index : int):

    # Connect
    socket.sendall(f"PASS {SERVER_PASSWORD}\r\nNICK BOT{index}\r\nUSER BOT_{index} 0 * :BOT{index}\r\nJOIN {CHANNEL_NAME}\r\n".encode('utf-8'))
    time.sleep(0.2)
    
    # Spam
    for i in range(0, MESSAGE_COUNT):
        try:
            socket.sendall(f"PRIVMSG #a :[{index}][{i}] ---> STRESS TEST <---\r\n".encode('utf-8'))
            time.sleep(TIME_BETWEEN_MESSAGE)
        except:
            print(f'[-] Thread[{index}] Broken Pipe')
            break

    # Close Socket
    socket.close()
        

def RECV(socket: socket, index : int):
    
    # Print Everything Received from Server
    while True:
        try:
            print(f"RECV[{index}]: {socket.recv(RECV_BUFFER_SIZE).decode('utf-8')}")
        except:
          break





def SpawnInstance(index : int): 
    global SEND_THREAD_LIST, RECV_THREAD_LIST
      
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect((IP, PORT))
    except Exception as e:
        print(f"[+] Could not connect to Server... {e}")
        return
    
    SEND_THREAD_LIST[index] = threading.Thread(target=SEND, args=(sock, index))
    #RECV_THREAD_LIST[index] = threading.Thread(target=RECV, args=(sock, index))
    
    SEND_THREAD_LIST[index].start()
    #RECV_THREAD_LIST[index].start()
    

def JoinInstance(index : int):
    global SEND_THREAD_LIST, RECV_THREAD_LIST
    SEND_THREAD_LIST[index].join()
    #RECV_THREAD_LIST[index].join()
    





if __name__ == "__main__":



    # Get Host IP & Port
    try:
        IP = socket.gethostbyname(socket.gethostname())
        PORT = 2000
        if len(sys.argv) == 2:
            PORT = int(sys.argv[2])
            if PORT < 0 or PORT > 65535:
                raise Exception("Port can only be value between 0 and 65535")
    except Exception as e:
        print(f"[-] Error: {e}")
        sys.exit(1)

    # Spawn Instances
    for i in range(0, THREAD_TO_SPAWN):
        SpawnInstance(i)
        
    # Join Instances
    for i in range(0, THREAD_TO_SPAWN):
        JoinInstance(i)
        
