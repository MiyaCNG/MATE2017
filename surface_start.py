import os
import time
import yeti

conf_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "robot.yml")

mon = yeti.Monitor("surface", conf_path)
mon.start()
mon.start_modules()

while True:
    time.sleep(1000)