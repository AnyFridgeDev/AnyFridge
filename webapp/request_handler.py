"""
Handles all the requests from the AnyFridge device
"""

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import os
import json
import uvicorn
import threading
from typing import List
import datetime 

from common import load_user_data, save_user_data

anyfridge_request_handler = FastAPI()

class RequestData(BaseModel):
    user_id: str
    upc_code: str
    action: str  # "POST" or "DELETE"



@anyfridge_request_handler.post("/update")
async def update_item(data: RequestData):
    user_id = data.user_id
    upc_code = data.upc_code
    action = data.action.upper()

    if action not in ["POST", "DELETE"]:
        raise HTTPException(status_code=400, detail="Invalid action")
    
    scan = {
        "upc_code": upc_code,
        "timestamp": datetime.datetime.now().isoformat()
    }

    user_data = load_user_data(user_id)

    if "scans" not in user_data:
        user_data["scans"] = []

    if action == "POST":
        user_data["scans"].append(scan)
    elif action == "DELETE":
        # Remove the oldest scan with the given UPC code
        for i, scan in enumerate(user_data["scans"]):
            if scan["upc_code"] == upc_code:
                del user_data["scans"][i]
                break
        else:
            raise HTTPException(status_code=404, detail="UPC code not found")

    save_user_data(user_id, user_data)
    return {"message": "Success", "data": user_data}

@anyfridge_request_handler.get("/get/{user_id}")
async def get_items(user_id: str):
    user_data = load_user_data(user_id)
    return user_data

if __name__ == "__main__":
    # threading.Thread(target=gradio_ui).start()
    uvicorn.run(anyfridge_request_handler, host="0.0.0.0", port=5000)
