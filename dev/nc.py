#!/usr/bin/env python3
import socket
import time
host = "10.12.1.11"
port = 2000
# Send message with a Sleep after next message
# { STRING, TIMER }
Messages_And_Timer = [
    ("PASS a\r\n", 1),
    ("USER a 0 * :b\r\n", 1),
    ("PING 0000-0000-0000\r\n", 5),
    ("JOIN #test\r\n", 1),
]
def send_strings():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((host, port))
            for string, timer in Messages_And_Timer:
                s.sendall(string.encode('utf-8'))
                time.sleep(timer)
            print("[+] Sent !")
    except Exception as e:
        print(f"[-] Error -> {e}")
if __name__ == "__main__":
    send_strings()