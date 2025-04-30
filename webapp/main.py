<<<<<<< HEAD
from fastapi import FastAPI, Request, Query, Form
=======
from fastapi import FastAPI, Request, HTTPException, Query, Form
>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e
from fastapi.templating import Jinja2Templates
from fastapi.responses import JSONResponse
import httpx
import aiofiles
import os
import json
<<<<<<< HEAD
import datetime
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
=======
from datetime import datetime
from fastapi.responses import RedirectResponse
>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")

<<<<<<< HEAD
# root route
=======

user_id = None

# Start the request handler server
def start_request_handler():
    process = subprocess.Popen(["uvicorn", "request_handler:anyfridge_request_handler", "--host", "0.0.0.0", "--port", "5000"])
    time.sleep(2)  # Give the server some time to start
    return process

>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e
@app.get("/")
async def read_items(request: Request, user_id: str = Query(None)):
    if not user_id:
        return templates.TemplateResponse("login.html", {"request": request})
    
    user_id = user_id.strip()

    # Check if the user ID is valid, i.e. the file for the user exists
    user_file_path = os.path.join("data", f"{user_id}.json")

    # Ensure the 'data' directory exists
    os.makedirs("data", exist_ok=True)  

    # Check if the user file exists, if not create it
    if not os.path.exists(user_file_path):
<<<<<<< HEAD
        user_file_path = os.path.join("data", "test_user_real_items.json")
        user_id = "test_user_real_items"
=======
        async with aiofiles.open(user_file_path, "w") as new_user_file:

            # Create a new user file with empty scans
            await new_user_file.write(json.dumps({"scans": []}))

            # Ensure data is written to disk
            await new_user_file.flush()  
>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e
    
    # Check if the user file exists
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())
    
    # Get the UPC codes from the user's scans
    scans = user_data.get("scans", [])

    upc_codes = [scan["upc_code"] for scan in scans]
    timestamps = [scan["timestamp"] for scan in scans]

    items = []

<<<<<<< HEAD
    # Fetch data for each UPC code
    async with httpx.AsyncClient() as client:
        for upc_code in upc_codes:

            # Fetch data from Open Food Facts API
            response = await client.get(f"https://world.openfoodfacts.net/api/v2/product/{upc_code}?fields=product_name,image_url,expiration_date")
=======
    # Fetch product data for each UPC code
    # Sort scans by timestamp in descending order (newest first)
    scans = sorted(scans, key=lambda scan: scan.get("timestamp", ""), reverse=True)

    # Write the now ordered scans back to the JSON file
    async with aiofiles.open(user_file_path, "w") as user_file:
        await user_file.write(json.dumps({"scans": scans}))

    async with httpx.AsyncClient() as client:
        for scan in scans:
            # Get the UPC code and timestamp from the scan
            upc_code = scan["upc_code"]
            timestamp = scan.get("timestamp")
            date_added = datetime.strptime(timestamp, "%Y-%m-%dT%H:%M:%S.%f").strftime("%m/%d/%Y %H:%M") if timestamp else None
            # Read expiration date from the scan, or set to "None" if it doesn't exist
            expiration_date = scan.get("expiration_date") or "None"

            # Fetch product data from Open Food Facts API
            response = await client.get(f"https://world.openfoodfacts.net/api/v2/product/{upc_code}?fields=product_name,image_url,image_nutrition_url")
>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e

            # Check if the response is successful
            if response.status_code == 200:

                # Get the product data from the response
                product_data = response.json().get("product", {})
<<<<<<< HEAD
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
=======
                items.append({
                    "product_name": product_data.get("product_name"),
                    "image_url": product_data.get("image_url"),
                    "expiration_date": expiration_date,
                    "date_added": date_added,
                    "image_nutrition_url": product_data.get("image_nutrition_url")
                })
    return templates.TemplateResponse("home.html", {"request": request, "items": items, "user_id": user_id})


@app.post("/delete_item")
async def delete_item(user_id: str = Form(...), index: int = Form(...)):
    # Path to the user's file
    user_file_path = os.path.join("data", f"{user_id}.json")

    # Ensure the user file exists
    if not os.path.exists(user_file_path):
        raise HTTPException(status_code=404, detail="User file not found")

    # Load the user's data
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())

    # Get the scans and ensure the index is valid
    scans = user_data.get("scans", [])
    if index < 0 or index >= len(scans):
        raise HTTPException(status_code=400, detail="Invalid index")

    # Remove the item at the specified index
    del scans[index]

    # Update the user's data
    user_data["scans"] = scans

    # Save the updated data back to the file
    async with aiofiles.open(user_file_path, "w") as user_file:
        await user_file.write(json.dumps(user_data))

    # Redirect to the home page with the user_id in the query parameters
    return RedirectResponse(url=f"/?user_id={user_id}", status_code=303)

@app.post("/update-expiration-date")
async def update_expiration_date(user_id: str = Form(...), index: int = Form(...), expiration_date: str = Form(...)):
    print(f"Debug: user_id={user_id}")
    print(f"Debug: index={index}")
    print(f"Debug: expiration_date={expiration_date}")
   
    # Path to the user's file
    user_file_path = os.path.join("data", f"{user_id}.json")

    # Ensure the user file exists
    if not os.path.exists(user_file_path):
        raise HTTPException(status_code=404, detail="User file not found")

    # Load the user's data
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())

    # Get the scans and ensure the index is valid
    scans = user_data.get("scans", [])
    if index < 0 or index >= len(scans):
        raise HTTPException(status_code=400, detail="Invalid index")

    

    # Convert the expiration date to ISO format
    try:
        expiration_date_iso = datetime.strptime(expiration_date, "%Y-%m-%d").strftime("%m-%d-%Y")
    except ValueError:
        raise HTTPException(status_code=400, detail="Invalid expiration date format. Use YYYY-MM-DD.")

    # Update the expiration date for the specified item
    scans[index]["expiration_date"] = expiration_date_iso

    # Save the updated data back to the file
    user_data["scans"] = scans
    async with aiofiles.open(user_file_path, "w") as user_file:
        await user_file.write(json.dumps(user_data))

       
    return RedirectResponse(url=f"/?user_id={user_id}", status_code=303)
>>>>>>> 8e5453dbb63ba04946356a9a78be1a16f689fc7e
