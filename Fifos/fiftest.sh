#!/bin/bash

# Define FIFO
FIFO=/tmp/communication_fifo

# Create FIFO if it doesn't exist
if [ ! -p $FIFO ]; then
    mkfifo $FIFO
fi

# Function for parent process
parent_process() {
    echo "Parent process started"
    for ((i = 1; i <= 5; i++)); do
        echo "Parent: Sending message $i to child"
        echo "Message $i" >$FIFO
        read response < $FIFO
        echo "Parent: Received response from child: $response"
    done
    echo "Parent process finished"
}

# Function for child process
child_process() {
    echo "Child process started"
    while true; do
        read message < $FIFO
        echo "Child: Received message: $message"
        if [[ $message == "quit" ]]; then
            echo "Child process received quit signal. Exiting."
            break
        fi
        echo "Child: Sending response to parent"
        echo "Response to $message" >$FIFO
    done
    echo "Child process finished"
}

# Run child process in background
child_process &

# Store PID of child process
child_pid=$!

# Run parent process
parent_process

# Send quit signal to child
echo "quit" >$FIFO

# Wait for child process to finish
wait $child_pid

# Remove FIFO
rm $FIFO
