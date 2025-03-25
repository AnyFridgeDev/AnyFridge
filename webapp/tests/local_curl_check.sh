USER_ID=$1

# Check if the user_id is provided
if [ -z "$USER_ID" ]; then
    echo "Please provide a user_id"
    exit 1
fi

# Start the server locally

## Kill existing servers
pkill -f "uvicorn request_handler:anyfridge_request_handler"
pkill -f "uvicorn main:app"

## Time to shut down
sleep 2

## Start the server
nohup uvicorn request_handler:anyfridge_request_handler --host 127.0.0.1 --port 12000 > request_handler.log 2>&1 &
nohup uvicorn main:app --host 127.0.0.1 --port 12001 --reload > main.log 2>&1 &

## Let the servers start
sleep 2

curl -X POST "http://127.0.0.1:12000/api/update" \
     -H "Content-Type: application/json" \
     -d '{
           "user_id": "'"$USER_ID"'",
           "upc_code": "038000222634",
           "action": "POST"
         }'

# Open the browser with this url
open "http://127.0.0.1:12001/?user_id=$USER_ID"