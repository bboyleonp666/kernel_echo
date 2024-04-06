#!/bin/sh

usage() {
    echo "Usage: $0 <socket_port> <message>"
    exit 1
}

# main
[ $# -ne 2 ] && usage && exit 1
[ "$1" = '' ] && usage && exit 1
[ "$2" = '' ] && usage && exit 1
socket_port="$1"
message="$2"

# Start echo server
echo "--- Start echo server ---"
./server "$socket_port" &
echo_server_pid="$!"
echo "echo_server PID: $echo_server_pid"
sleep 1
echo "--------------------------"
echo

# Send message to echo server
echo "--- Send message to echo server ---"
./client "$socket_port" "$message"

# Stop echo server
echo "--- Stop echo server ---"
kill -9 "$echo_server_pid"
