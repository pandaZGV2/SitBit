import datetime
from flask import Flask
from flask import render_template
from onem2m import *
import requests
import datetime


app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/threshold")
def threshold():
    uri = f"http://127.0.0.1:8080/~/in-cse/in-name/Threshold_Values/node1/la"
    headers = {"X-M2M-Origin": "admin:admin", "Content-type": "application/json"}
    response = requests.get(uri, headers=headers)

    data = json.loads(response.text)
    threshold = data["m2m:cin"]["con"]
    return threshold


@app.route("/data")
def data():
    import json
    import matplotlib.pyplot as plt

    sensors = [
        "Ultrasonic_Sensor",
        "Temperature_Sensor",
        "Humidity_Sensor",
    ]

    plt.style.use("custom.mplstyle")
    for sensor in sensors:
        uri = f"http://127.0.0.1:8080/~/in-cse/in-name/{sensor}/node1/?rcn=4"
        headers = {"X-M2M-Origin": "admin:admin", "Content-type": "application/json"}
        response = requests.get(uri, headers=headers)

        data = json.loads(response.text)
        data = data["m2m:cnt"]["m2m:cin"]

        x = []
        y = []

        for i in data:
            time = datetime.datetime.strptime(i["ct"], "%Y%m%dT%H%M%S")
            value = float(i["con"])
            x.append(time)
            y.append(value)

        plt.plot(x[-5:], y[-5:], marker="o")
        plt.savefig(f"static/{sensor}.jpg")
        plt.clf()

    return "end"


if __name__ == "__main__":
    app.run(debug=True)
