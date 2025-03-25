#!/bin/bash

# Script ran on the AWS EC2 instance to start all the servers

echo "Stopping Uvicorn servers..."
pkill -f "uvicorn request_handler:anyfridge_request_handler"
pkill -f "uvicorn main:app"

echo "Restarting request handler..."
nohup uvicorn request_handler:anyfridge_request_handler --host 0.0.0.0 --port 5000 > request_handler.log 2>&1 &

echo "Restarting main FastAPI app..."
nohup uvicorn main:app --host 127.0.0.1 --port 9000 --reload > main.log 2>&1 &

echo "Both servers restarted successfully!"