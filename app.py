import datetime
from flask import Flask, render_template_string
from flask import render_template
from onem2m import *
import requests
import datetime

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")


@app.route('/data')
def data():
    import json
    import matplotlib.pyplot as plt
    uri = "http://127.0.0.1:8080/~/in-cse/in-name/Ultrasonic_Sensor/node1/?rcn=4"
    headers = {
    'X-M2M-Origin': 'admin:admin',
    'Content-type': 'application/json'
    }
    response = requests.get(uri, headers=headers)
    json = json.loads(response.text)
    print(response.text)
    x = list()
    y = list()
    for i in json['m2m:cnt']['m2m:cin']:
        y.append(float(i["con"]))
        x.append(datetime.datetime.strptime(i["ct"], '%Y%m%dT%H%M%S'))
    plt.plot(x, y)
    plt.savefig('static/myimg.jpg')
    return str(json['m2m:cnt']['m2m:cin'])

if __name__ == "__main__":
    app.run(debug=False)