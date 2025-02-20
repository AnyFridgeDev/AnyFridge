# AnyFridge

## GitHub
[GitHub](https://github.com/AnyFridgeDev/AnyFridge)

## Authors
- [Isaac Fletcher](https://github.com/isaac-fletcher)
- [Ethan Anderson](https://github.com/Elan456)
- [Caden Allen](https://github.com/coolguycaden)

The repository URL should be added at the top of all your team’s turnedin documents from now on. Your repository should also contain a README
file that provides a short project overview, and describes how the repository is organized (basically enough to help an absolute newcomer find
their way around without asking a lot of questions).

## Project Overview

This is the development repository for AnyFridge. AnyFridge is a compact device that lets you give your regular fridge smart fridge features without the need to buy an entirely new fridge. It is a UPC barcode scanner that mounts magnetically to your fridge to make keeping track of your store purchases extremely convenient. Each item you scan will be cataloged in a web app that lets you view and manage your inventory levels.

## Organization

This repository is organized into the following sections:
    
    hardware/ - Stores system schematics and PCB for custom system hardware
        
        libraries/ - Stores custom libraries containing custom components in the schematics
    
    software/ - Stores firmware and web application code for the system
        
        firmware/ - Contains custom firmware written for the ESP32

        web/ - Contains Python web app to handle inventory tracking management
    
    docs/ - Stores documentation about the project hardware and software
