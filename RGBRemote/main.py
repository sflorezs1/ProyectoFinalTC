from socket import error
from flask import Flask, request, redirect
import socket
from typing import Optional, Tuple
from flask.helpers import url_for
import requests
import nmap
import platform
import subprocess

nm: nmap.PortScanner = nmap.PortScanner()

app: Flask = Flask(__name__)

server_ip: Optional[str] = "nodemcu.local"

last_color1: str = "#000000"
last_color2: str = "#000000"

@app.route("/", methods=['GET'])
def home() -> str:
    colors: Tuple[str, str] = get_prev_colors()
    temp: float = get_temperature()
    return f"""
        <!-- View -->
        <!DOCTYPE html>
        <html>
            <head>
                <title>RGB Remote</title>
                <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css" integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2" crossorigin="anonymous">
                <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.1/css/all.min.css" integrity="sha512-+4zCK9k+qNFUR5X+cKL9EIR+ZOhtIloNl9GIKS57V1MyNsYpYcUrUeQc9vNfzsWfV28IaLL3i96P9sdNyeRssA==" crossorigin="anonymous" />
            </head>
            <body>
                <div class="container py-4">
                    <div class="card text-center text-light bg-primary mb-4">
                        <div class="card-body">
                            <h1 class="card-title">RGB remote control APP</h1>
                        </div>
                    </div>
                    {select_colors(*colors)}
                    {show_temp(temp)}
                </div>
            </body>
        </html>
    """

@app.route("/sent", methods=['GET'])
def send_data() -> str:
    color1: Optional[str] = request.args.get('led_color_1')
    color2: Optional[str] = request.args.get('led_color_2')
    global last_color1, last_color2
    print(color1, color2)
    if color1 and color2:
        print("entered")
        last_color1 = color1
        last_color2 = color2
        try:
            requests.get("http://" + server_ip + f"/set_leds?led_color_1={color1.replace('#', '%23')}&led_color_2={color2.replace('#', '%23')}")
        except ConnectionError as e:
            print(e)
    return redirect(url_for("home"))

def get_prev_colors() -> Tuple[str, str]:
    try:
        colors = requests.get("http://" + server_ip + "/get_leds").json()
        return colors["color_led_1"], colors["color_led_2"]
    except Exception as e:
        print(e)
        return last_color1, last_color2

def get_temperature() -> float:
    try:
        temp: float = float(requests.get("http://" + server_ip + "/get_temperature").content)
        return temp
    except Exception as e:
        print(e)
        return 0.0

def show_temp(temp) -> str:
    def show_thermomether() -> str:
        if temp < 10:
            return '<i class="fa fa-thermometer-empty"></i>'
        elif temp < 20:
            return '<i class="fa fa-thermometer-quarter"></i>' 
        elif temp < 30:
            return '<i class="fa fa-thermometer-half"></i>'
        elif temp < 40:
            return '<i class="fa fa-thermometer-three-quarters"></i>'
        else:
            return '<i class="fa fa-thermometer-full"></i>'

    return f"""
        <div class="card m-2">
            <div class="card-body">
                <h3 class="card-title"> Temperaure on sensor </h3>
                <p class="card-text"> {show_thermomether()} {temp} &deg;C</p>
            </div>
        </div>
    """

def select_colors(color1: str, color2: str) -> str:
    return f"""
        <div class="card m-2">
            <div class="card-body">
                <h2 class="card-title"> LEDs </h2>
                <p class="card-text">
                    <form action="/sent" method="GET">
                        <div class="row">
                            {show_led("LED1", "led_color_1", color1)}
                            {show_led("LED2", "led_color_2", color2)}
                        </div>
                        <button type="submit" class="btn btn-primary"> <i class="fa fa-check"></i> Send </button>
                    </form>
                </p>
            </div>
        </div>
    """

def show_led(name: str, id: str, value: str = "#000000") -> str:
    return f"""
        <div class="card bg-secondary text-light text-center m-2 col w-25">
            <div class="card-body">
                <h3 class="card-title">{name}</h3>
                <p class="card-text">
                    <i class="fa fa-paint-brush"></i>
                    <input type="color" name="{id}" id="{id}" value="{value}">
                </p>
            </div>
        </div>
    """

if __name__ == "__main__":
    app.run()
