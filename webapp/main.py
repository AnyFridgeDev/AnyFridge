from fastapi import FastAPI, Request, Query, Form
from fastapi.templating import Jinja2Templates
from fastapi.responses import JSONResponse
import httpx
import aiofiles
import os
import json
import datetime
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")

# root route
@app.get("/")
async def read_items(request: Request, user_id: str = Query(None)):
    if not user_id:
        return templates.TemplateResponse("login.html", {"request": request})
    
    user_file_path = os.path.join("data", f"{user_id}.json")
    if not os.path.exists(user_file_path):
        user_file_path = os.path.join("data", "test_user_real_items.json")
        user_id = "test_user_real_items"
    
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())
    
    scans = user_data.get("scans", [])

    upc_codes = [scan["upc_code"] for scan in scans]
    timestamps = [scan["timestamp"] for scan in scans]

    items = []

    # Fetch data for each UPC code
    async with httpx.AsyncClient() as client:
        for upc_code in upc_codes:

            # Fetch data from Open Food Facts API
            response = await client.get(f"https://world.openfoodfacts.net/api/v2/product/{upc_code}?fields=product_name,image_url,expiration_date")

            # Check if the response is successful
            if response.status_code == 200:

                # Get the product data from the response
                product_data = response.json().get("product", {})
                print(f"Fetched data for UPC {upc_code}: {product_data}")

                # Get the timestamp of the scan, format in neat, readable format
                timestamp = timestamps[upc_codes.index(upc_code)]
                formatted_timestamp = datetime.datetime.fromisoformat(timestamp).strftime("%m/%d/%y %H:%M")

                # Append the product data to the items list
                items.append({
                    "product_name": product_data.get("product_name"),
                    "image_url": product_data.get("image_url"),
                    "expiration_date": product_data.get("expiration_date"),
                    "date_added": formatted_timestamp,
                })
    
    # Render the home.html template with the items list
    return templates.TemplateResponse("home.html", {"request": request, "items": items, "user_id": user_id})  # Ensure this is correctly placed

@app.post("/api/update")
async def update_item(user_id: str = Form(...), code: str = Form(...), action: str = Form(...), index: int = Form(...)):
    logging.info(f"Received request to {action} item with UPC {code} for user {user_id} at index {index}")
    user_file_path = os.path.join("data", f"{user_id}.json")
    
    if not os.path.exists(user_file_path):
        return JSONResponse(content={"error": "User file not found"}, status_code=404)

    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())

    if action == "DELETE":
        scans = user_data.get("scans", [])
        if 0 <= index < len(scans) and scans[index]["upc_code"] == code:
            del scans[index]
            async with aiofiles.open(user_file_path, "w") as user_file:
                await user_file.write(json.dumps(user_data, indent=4))
            return JSONResponse(content={"message": "Item deleted successfully"}, status_code=200)
        else:
            return JSONResponse(content={"error": "Invalid index or UPC code mismatch"}, status_code=400)

    return JSONResponse(content={"error": "Invalid action"}, status_code=400)
