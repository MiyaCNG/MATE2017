import sys
import yeti
import time

# Setup the Yeti node
module_id = sys.argv[1]
yeti_node = yeti.Node("node_" + module_id, sys.argv[2])
yeti_node.bootstrap_node(yeti_node.module_conf[module_id]["mon"])
yeti_node.register_module(module_id)

# Released by rdb under the Unlicense (unlicense.org)
# Based on information from:
# https://www.kernel.org/doc/Documentation/input/joystick-api.txt

import os, struct, array
from fcntl import ioctl

# Iterate over the joystick devices.
print('Available devices:')

for fn in os.listdir('/dev/input'):
    if fn.startswith('js'):
        print('  /dev/input/%s' % (fn))

# We'll store the states here.
axis_states = [0.0] * 6
button_states = [False] * 12


axis_map = []
button_map = []

# Open the joystick device.
fn = '/dev/input/js0'
print('Opening %s...' % fn)
jsdev = open(fn, 'rb')

# Get the device name.
#buf = bytearray(63)
buf = array.array('b', [0] * 64)
ioctl(jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
js_name = buf.tostring()
print('Device name: %s' % js_name)

# Main event loop
last_beat = 0
while True:
    time.sleep(0.001)
    if time.time() - last_beat > 1:
        yeti_node.send_heartbeat(module_id)
        last_beat = time.time()
    curr_running_state = yeti_node.get_running_state(module_id)
    if curr_running_state == "running":
        try:
            yeti_node.set_state("axes", axis_states, module_id)
            yeti_node.set_state("buttons", button_states, module_id)

            evbuf = jsdev.read(8)

            if evbuf:
                time1, value, type, number = struct.unpack('IhBB', evbuf)

                if type & 0x80:
                    pass

                if type & 0x01:
                    button_states[number] = value

                if type & 0x02:
                    fvalue = value / 32767.0
                    axis_states[number] = fvalue

        except Exception as e:
            yeti_node.report_module_exception(module_id, e)
