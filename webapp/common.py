"""
Utility functions used across both the request_handler and the ui
"""
import os 
import json 

DATA_DIR = "data"
os.makedirs(DATA_DIR, exist_ok=True)

def get_user_file(user_id):
    return os.path.join(DATA_DIR, f"{user_id}.json")

def create_user(user_id):
    filepath = get_user_file(user_id)
    with open(filepath, "w") as f:
        json.dump({"scans": []}, f)

def load_user_data(user_id):
    filepath = get_user_file(user_id)
    if not os.path.exists(filepath):
        create_user(user_id)
    
    with open(filepath, "r") as f:
        return json.load(f)

def save_user_data(user_id, data):
    with open(get_user_file(user_id), "w") as f:
        json.dump(data, f)

def delete_user(user_id):
    filepath = get_user_file(user_id)
    if os.path.exists(filepath):
        os.remove(filepath)
        return True
    return False
