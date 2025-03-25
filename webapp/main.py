from fastapi import FastAPI, Request, HTTPException, Query
from fastapi.templating import Jinja2Templates
import httpx
import aiofiles
import subprocess
import time
import os
import json

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")


@app.get("/")
async def read_items(request: Request, user_id: str = Query(None)):
    if not user_id:
        return templates.TemplateResponse("login.html", {"request": request})
    
    user_file_path = os.path.join("data", f"{user_id}.json")
    if not os.path.exists(user_file_path):
        user_file_path = os.path.join("data", "test_user.json")
        user_id = "test_user"
    
    async with aiofiles.open(user_file_path, "r") as user_file:
        user_data = json.loads(await user_file.read())
    
    scans = user_data.get("scans", [])
    upc_codes = [scan["upc_code"] for scan in scans]
    
    items = []
    async with httpx.AsyncClient() as client:
        for upc_code in upc_codes:
            response = await client.get(f"https://world.openfoodfacts.net/api/v2/product/{upc_code}?fields=product_name,image_url,expiration_date")
            if response.status_code == 200:
                product_data = response.json().get("product", {})
                print(f"Fetched data for UPC {upc_code}: {product_data}")
                items.append({
                    "product_name": product_data.get("product_name"),
                    "image_url": product_data.get("image_url"),
                    "expiration_date": product_data.get("expiration_date"),
                    "date_added": time.strftime("%m/%d/%y %H:%M")
                })
    
    return templates.TemplateResponse("home.html", {"request": request, "items": items, "user_id": user_id})
