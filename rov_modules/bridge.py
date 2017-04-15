import sys
import serial
import yeti
import time


class Bridge(yeti.IterativeModule):
    first_start = True

    thruster_mapping = {
        "left_rear": 1,
        "right_rear": 2,
        "left_front": 3,
        "right_front": 4,
        "left_vert": 5,
        "right_vert": 6
    }

    def start(self):
        if self.first_start:
            self.serial = serial.Serial(port="/dev/ttyS1", baudrate=115200)

            self.first_start = False
            self.reset_outputs()
        if not self.serial.is_open:
            self.serial.open()

    def reset_outputs(self):
        for thruster_key in self.thruster_mapping:
            self.set_state("{}_thruster_state".format(thruster_key), 0)

    def update(self):
        for thruster_key in self.thruster_mapping:
            thruster_float = min(1, max(self.get_state("{}_thruster_state".format(thruster_key)), -1))
            thruster_val = int(thruster_float*2400)
            value_bytes = thruster_val.to_bytes(4, byteorder='little', signed=True)
            self.set_hardware_state(
                "t{}".format(self.thruster_mapping[thruster_key]),
                value_bytes
            )
            time.sleep(0.01)
        #print(self.get_hardware_state("h"))

    def set_hardware_state(self, key, value_bytes):
        self.serial.write('s'.encode('utf-8'))

        key_bytes = key.encode('utf-8')
        self.serial.write(len(key_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(key_bytes)

        self.serial.write(len(value_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(value_bytes)

    def get_hardware_state(self, key):
        print("Getting state!")
        self.serial.write('g'.encode('utf-8'))

        key_bytes = key.encode('utf-8')
        self.serial.write(len(key_bytes).to_bytes(1, byteorder='little'))
        self.serial.write(key_bytes)

        print("Awaiting response!")
        prefix = self.serial.read(1) # read 'r' message
        assert prefix == "r".encode('utf-8')
        print("Got a prefix!!")
        key_len = int.from_bytes(self.serial.read(1), byteorder='little')
        print("Reading {} bytes for key.".format(key_len))
        key_bytes = self.serial.read(key_len)

        value_len = int.from_bytes(self.serial.read(1), byteorder='little')
        print("Reading {} bytes for data.".format(value_len))
        value_bytes = self.serial.read(value_len)

        return value_bytes

    def stop(self):
        self.reset_outputs()
        if self.serial.is_open:
            self.serial.close()
        print("Halting problem solved!")

if __name__ == "__main__":
    yeti.bootstrap_module(Bridge, sys.argv[1], sys.argv[2])

