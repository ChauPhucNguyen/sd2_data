from multiprocessing.connection import answer_challenge
from turtle import speed
from flask import Flask, render_template, jsonify
import random

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/data')
def get_data():
    data = {
        "speed": random.uniform(0.5,1.5),
        "steeringAngle": random.uniform(-30,30),
        "batteryLevel": random.uniform(0.0, 100.0),
    }
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)