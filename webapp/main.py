from fastapi import FastAPI, Request, HTTPException, Query
from fastapi.templating import Jinja2Templates
import httpx
import subprocess
import time

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")

def start_request_handler():
    process = subprocess.Popen(["uvicorn", "request_handler:anyfridge_request_handler", "--host", "0.0.0.0", "--port", "5000"])
    time.sleep(2)  # Give the server some time to start
    return process

@app.get("/")
async def read_items(request: Request, user_id: str = Query(None)):
    if not user_id:
        return templates.TemplateResponse("login.html", {"request": request})
    
    process = start_request_handler()
    try:
        async with httpx.AsyncClient(timeout=5.0) as client:
            response = await client.get(f"http://127.0.0.1:5000/get/{user_id}")
            response.raise_for_status()
            user_data = response.json()
            items = user_data.get("items", [])
    except (httpx.HTTPStatusError, httpx.RequestError) as exc:
        process.terminate()  # Ensure the server is stopped after the request
        return templates.TemplateResponse("login.html", {"request": request, "error": "Invalid user ID or request timed out"})
    finally:
        process.terminate()  # Ensure the server is stopped after the request
    
    return templates.TemplateResponse("home.html", {"request": request, "items": items})