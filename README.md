# ⚡ ElectricBlinds

<p align="center">
DIY smart control system for electric blinds
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-ESP%20%7C%20Arduino-blue">
  <img src="https://img.shields.io/badge/type-IoT-green">
</p>

---

---

## 📖 About the Project

**ElectricBlinds** is a DIY smart system designed to control window blinds using an embedded microcontroller.

The project includes the **design and implementation of a complete control system**, including the development and construction of a **dedicated control device**.

User–device communication is handled through a **web-based application hosted directly on the device’s built-in server**. The web interface provides an **intuitive and responsive user experience**, accessible from any device with a web browser.

The backend logic running on the embedded server manages **real-time communication between the user interface and the motor control system**, enabling:

- precise positioning of blinds  
- scheduled operations  
- automation features  

The entire device — including the **mechanical enclosure and mounting components — was designed and 3D printed by me**.

This project combines **embedded systems, web development, and hardware design** to create a fully functional smart home device.

---

## ✨ Features

- Control electric blinds
- Smart home integration
- Scheduled automation
- Manual control with buttons
- Local network control
- Custom automation logic

---
## ⚙️ Hardware

The control device is built using the following components:

- **ESP32** – main microcontroller and web server
- **Stepper Motor 17HS4023** – drives the blinds
- **A4988 Motor Driver** – controls the stepper motor
- **LM2596 Step-Down Converter** – voltage regulation

The enclosure and mechanical components were **custom designed and 3D printed**.

---

## 🖥 Web Interface

The device hosts its own **embedded web application**.

Features of the interface:

- simple and responsive design
- real-time control
- accessible from phones, tablets, and computers
- no external server required

---
