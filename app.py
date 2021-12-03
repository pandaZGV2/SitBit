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

        plt.plot(x[-10:], y[-10:])
        plt.savefig(f"static/{sensor}.jpg")
        plt.clf()

    alerts = {
        0: ["Slouching for too long.", "red"],
        1: ["Sitting for too long.", "red"],
        2: ["Temperature and/or humidity too high.", "red"],
        3: ["Currently Not Seated.", "yellow"],
        4: ["Currently Slouching", "orange"],
        5: ["You're doing great! Maintain that excellent posture!", "green"],
    }

    uri = f"http://127.0.0.1:8080/~/in-cse/in-name/Alerts/node1/la"
    headers = {"X-M2M-Origin": "admin:admin", "Content-type": "application/json"}
    response = requests.get(uri, headers=headers)

    data = json.loads(response.text)
    alert_id = data["m2m:cin"]["con"]
    alert = alerts[int(alert_id)][0]
    alert_color = alerts[int(alert_id)][1]

    return alert


if __name__ == "__main__":
    app.run(debug=True)
