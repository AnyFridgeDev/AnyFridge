"""
The Gradio UI module.
"""

import gradio as gr
from typing import List
from common import load_user_data
import pandas as pd 

def get_user_data_df(user_id: str) -> List[str]:
    user_data = load_user_data(user_id)
    print(user_data)
    df = pd.DataFrame(user_data["scans"], columns=["upc_code", "timestamp"])
    print(df)
    return df

with gr.Blocks() as ui:
    user_id = gr.Textbox(label="User ID", placeholder="Enter your user ID and press Enter")

    upc_codes = gr.DataFrame(
        headers=["upc_code", "timestamp"],
        datatype=["str"],
        label="UPC Codes",
        interactive=False
    )

    # Trigger update when the user presses Enter
    user_id.submit(get_user_data_df, inputs=user_id, outputs=upc_codes)

if __name__ == "__main__":
    ui.launch()
