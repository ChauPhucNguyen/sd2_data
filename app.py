from multiprocessing.connection import answer_challenge
from turtle import speed
from flask import Flask, render_template, jsonify
import random

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('home.html')

@app.route('/about')
def about():
    return render_template('about.html')

@app.route('/projectFiles')
def projectFiles():
    return render_template('projectFiles.html')

@app.route('/team')
def team():
    return render_template('team.html')

@app.route('/videoDemo')
def videoDemo():
    return render_template('videoDemo.html')

@app.route('/data')
def get_data():
    data = {
        "speed": random.uniform(0.5,1.5),
        "steeringAngle": random.uniform(-30,30),
        "batteryLevel": random.uniform(0.0, 100.0),
        "batteryConsumption" : random.uniform(0.01,0.05),
    }
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)