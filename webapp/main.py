from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates

app = FastAPI()

# sets template directory
templates = Jinja2Templates(directory="templates")

@app.get("/")
def home(request: Request):

    """
    The Home Page of the Dashboard    
    """

    return templates.TemplateResponse("home.html", {
        "request": request,
    })

@app.post("/data")
def add_item():

    """
    Add an item to the user's data
    """
    
    return {
        "Data": "Item Added"
    }
