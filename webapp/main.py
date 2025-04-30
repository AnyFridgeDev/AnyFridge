from fastapi import FastAPI, Request, HTTPException, Query, Form
from fastapi.templating import Jinja2Templates
import httpx
import aiofiles
import os
import json
from datetime import datetime
from fastapi.responses import RedirectResponse
import asyncio

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")

is_data_monitor_running = False
current_last_timestamp = None

async def monitor_user_data(file_path, user_id):
    global current_last_timestamp
    while True:

        
        async with aiofiles.open(file_path, "r") as user_file:
            user_data = json.loads(await user_file.read())
        
        scans = user_data.get("scans", [])

        #Check if the last timestamp has changed
        if scans[-1].get("timestamp") != current_last_timestamp:
            print("Data has changed")
            current_last_timestamp = scans[-1].get("timestamp")
            

        await asyncio.sleep(3) 
            

@app.get("/")
async def read_items(request: Request, user_id: str = Query(None)):
    global is_data_monitor_running

    # Stop the data monitor if it's running, will restart it later
    # if is_data_monitor_running == True:
    #     for task in asyncio.all_tasks():
    #         if task.get_coro().__name__ == "monitor_user_data":
    #             task.cancel()
    #     is_data_monitor_running = True

    if not user_id:
        return templates.TemplateResponse("login.html", {"request": request})

    # Check if the user ID is valid, i.e. the file for the user exists
    user_file_path = os.path.join("data", f"{user_id}.json")

    # Ensure the 'data' directory exists
    os.makedirs("data", exist_ok=True)  

    # Check if the user file exists, if not create it
    if not os.path.exists(user_file_path):
        async with aiofiles.open(user_file_path, "w") as new_user_file:

            # Create a new user file with empty scans
            await new_user_file.write(json.dumps({"scans": []}))

            # Ensure data is written to disk
            await new_user_file.flush()  
    
    # Check if the user file exists
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())
    
    # Get the UPC codes from the user's scans
    scans = user_data.get("scans", [])

    items = []

    # Fetch product data for each UPC code
    # Sort scans by timestamp in descending order (newest first)
    scans = sorted(scans, key=lambda scan: scan.get("timestamp", ""), reverse=True)


    # Write the now ordered scans back to the JSON file
    async with aiofiles.open(user_file_path, "w") as user_file:
        await user_file.write(json.dumps({"scans": scans}))


    # Start the data monitor with new user id
    if is_data_monitor_running == False:
        global current_last_timestamp
        current_last_timestamp = scans[-1].get("timestamp")
        asyncio.create_task(monitor_user_data(user_file_path, user_id))
        is_data_monitor_running = True
        print("Data monitor started")

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

            # Check if the response is successful
            if response.status_code == 200:

                # Get the product data from the response
                product_data = response.json().get("product", {})
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
        expiration_date_iso = datetime.strptime(expiration_date, "%Y-%m-%d").strftime("%m/%d/%Y")
    except ValueError:
        raise HTTPException(status_code=400, detail="Invalid expiration date format. Use YYYY-MM-DD.")

    # Update the expiration date for the specified item
    scans[index]["expiration_date"] = expiration_date_iso

    # Save the updated data back to the file
    user_data["scans"] = scans
    async with aiofiles.open(user_file_path, "w") as user_file:
        await user_file.write(json.dumps(user_data))

       
    return RedirectResponse(url=f"/?user_id={user_id}", status_code=303)
