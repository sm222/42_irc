#!/bin/bash
# Define the messages in an array
messages=(
    "Message 1"
    "Message 2^M"
    "Message 3"
    # Add more messages as needed
)
# Define the address and port
HOST="10.12.6.2"
PORT="2000"
# Loop to construct the message stream
for message in "${messages[@]}"; do
    echo "$message"
done | nc $HOST $PORT