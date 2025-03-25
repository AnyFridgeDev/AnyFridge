USER_ID=$1

curl -X POST "https://af.ethananderson.dev/api/update" \
     -H "Content-Type: application/json" \
     -d '{
           "user_id": "'"$USER_ID"'",
           "upc_code": "038000222634",
           "action": "POST"
         }'

# Open the browser with this url
open "https://af.ethananderson.dev/?user_id=$USER_ID"