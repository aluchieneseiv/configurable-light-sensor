#!/usr/env python3
from flask import Flask, render_template, request, redirect
from time import sleep
from threading import Thread
from smbus import SMBus
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
OUTPUT_PIN = 7

GPIO.setup(OUTPUT_PIN, GPIO.OUT)
GPIO.output(OUTPUT_PIN, 1)

on_threshold = 800
off_threshold = 1000

bus = SMBus(1)
def read_sensor():
    return bus.read_word_data(0x5, 0)

sensor_value = read_sensor()

def update_value():
    global sensor_value
    sensor_value = read_sensor()

def my_thread():
    while True:
        while sensor_value > on_threshold:
            sleep(1)
            update_value()
        print('Turning lights on')
        GPIO.output(OUTPUT_PIN, 0)

        while sensor_value < off_threshold:
            sleep(1)
            update_value()
        print('Turning lights off')
        GPIO.output(OUTPUT_PIN, 1)

app = Flask(__name__)

@app.route('/', methods=['GET'])
def root():
    return render_template('index.html',
        on_threshold=on_threshold, off_threshold=off_threshold,
        current_value=sensor_value)

@app.route('/set-threshold', methods=['GET'])
def set_threshold():
    global on_threshold, off_threshold
    try:
        on_threshold = request.args.get('onThreshold', type=int)
        off_threshold = request.args.get('offThreshold', type=int)
    except Exception:
        pass
    return redirect('/')

@app.route('/get-sensor-value', methods=['GET'])
def get_sensor_value():
    return str(sensor_value)

if __name__ == '__main__':
    t = Thread(target=my_thread)
    t.daemon = True
    t.start()

    app.run(host='0.0.0.0')
